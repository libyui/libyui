/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:         YUIQt_builtins.cc

  Author:       Stefan Hundhammer <sh@suse.de>

  Textdomain    "packages-qt"

/-*/

#define USE_QT_CURSORS          1
#define FORCE_UNICODE_FONT      0

#include <sys/stat.h>
#include <unistd.h>

#include <QCursor>
#include <QFileDialog>
#include <QX11Info>
#include <QMessageBox>
#include <QPixmap>
#include <QInputDialog>
#include <qdir.h>

#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YQUI.h"
#include "YEvent.h"
#include "YMacroRecorder.h"
#include "YUISymbols.h"
#include "YQDialog.h"
#include "YQSignalBlocker.h"
#include "YQApplication.h"

#include "utf8.h"
#include "YQi18n.h"

#include <X11/Xlib.h>


#define DEFAULT_MACRO_FILE_NAME         "macro.ycp"



YCPValue YQUI::runPkgSelection( YWidget * packageSelector )
{
    y2milestone( "Running package selection..." );
    YCPValue input = YCPVoid();

    try
    {
	input = evaluateUserInput();
    }
    catch (const std::exception & e)
    {
	y2error( "Caught std::exception: %s", e.what() );
	y2error( "This is a libzypp problem. Do not file a bug against the UI!" );
    }
    catch (...)
    {
	y2error( "Caught unspecified exception." );
	y2error( "This is a libzypp problem. Do not file a bug against the UI!" );
    }

    y2milestone( "Package selection done - returning %s", input->toString().c_str() );

    return input;
}


void YQUI::makeScreenShot( std::string stl_filename )
{

    //
    // Grab the pixels off the screen
    //

    QWidget * dialog = (QWidget *) YDialog::currentDialog()->widgetRep();
    QPixmap screenShot = QPixmap::grabWindow( dialog->winId() );
    XSync( QX11Info::display(), false );
    QString fileName ( stl_filename.c_str() );
    bool interactive = false;

    if ( fileName.isEmpty() )
    {
	interactive = true;

        // Open a file selection box. Figure out a reasonable default
        // directory / file name.

        if ( screenShotNameTemplate.isEmpty() )
        {
            //
            // Initialize screen shot directory
            //

            QString home = QDir::homePath();
            char * ssdir = getenv("Y2SCREENSHOTS");
            QString dir  = ssdir ? ssdir : "yast2-screen-shots";

            if ( home == "/" )
            {
                // Special case: $HOME is not set. This is normal in the inst-sys.
                // In this case, rather than simply dumping all screen shots into
                // /tmp which is world-writable, let's try to create a subdirectory
                // below /tmp with restrictive permissions.
                // If that fails, trust nobody - in particular, do not suggest /tmp
                // as the default in the file selection box.

                dir = "/tmp/" + dir;

                if ( mkdir( qPrintable(dir), 0700 ) == -1 )
                    dir = "";
            }
            else
            {
                // For all others let's create a directory ~/yast2-screen-shots and
                // simply ignore if this is already present. This gives the user a
                // chance to create symlinks to a better location if he wishes so.

                dir = home + "/" + dir;
                (void) mkdir( qPrintable(dir), 0750 );
            }

            screenShotNameTemplate = dir + "/%s-%03d.png";
        }


        //
        // Figure out a file name
        //

        const char * baseName = moduleName();
        if ( ! baseName ) baseName = "scr";
        int no = screenShotNo[ baseName ];
        fileName.sprintf( qPrintable(screenShotNameTemplate), baseName, no );
        y2debug( "screenshot: %s", qPrintable(fileName) );

	{
	    YQSignalBlocker sigBlocker( _user_input_timer );

	    fileName = YQApplication::askForSaveFileName( fileName,
							  QString( "*.png" ) ,
							  _( "Save screen shot to..." ) );
	}

        if ( fileName.isEmpty() )
        {
            y2debug( "Save screen shot canceled by user" );
            return;
        }

        screenShotNo.insert( baseName, ++no );
    } // if fileName.isEmpty()


    //
    // Actually save the screen shot
    //

    y2debug( "Saving screen shot to %s", qPrintable(fileName) );
    bool success = screenShot.save( fileName, "PNG" );

    if ( ! success )
    {
	y2error( "Couldn't save screen shot %s", qPrintable(fileName) );

	if ( interactive )
	{
	    QMessageBox::warning( 0,					// parent
				  "Error",				// caption
				  QString( "Couldn't save screen shot\nto %1" ).arg( fileName ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  Qt::NoButton,				// button1
				  Qt::NoButton );			// button2
	}
    }

    if ( recordingMacro() )
    {
	macroRecorder->beginBlock();
	YDialog::currentDialog()->saveUserInput( macroRecorder );
	macroRecorder->recordMakeScreenShot( true, qPrintable(fileName) );
	macroRecorder->recordUserInput( YCPVoid() );
	macroRecorder->endBlock();
    }
}


void YQUI::askSaveLogs()
{
    QString fileName = YQApplication::askForSaveFileName( QString( "/tmp/y2logs.tgz" ),	 	// startWith
							  QString( "*.tgz *.tar.gz"  ),		// filter
							  QString( "Save y2logs to..."  ) );	// headline

    if ( ! fileName.isEmpty() )
    {
	QString saveLogsCommand = "/sbin/save_y2logs";

	if ( access( saveLogsCommand.toAscii(), X_OK ) == 0 )
	{
	    saveLogsCommand += " '" + fileName + "'";
	    y2milestone( "Saving y2logs: %s", qPrintable(saveLogsCommand) );
	    int result = system( qPrintable(saveLogsCommand) );

	    if ( result != 0 )
	    {
		y2error( "Error saving y2logs: \"%s\" exited with %d",
			 qPrintable(saveLogsCommand), result );
		QMessageBox::warning( 0,					// parent
				      "Error",					// caption
				      QString( "Couldn't save y2logs to %1 - "
					       "exit code %2" ).arg( fileName ).arg( result ),
				      QMessageBox::Ok | QMessageBox::Default,	// button0
				      QMessageBox::NoButton,			// button1
				      QMessageBox::NoButton );			// button2
	    }
	    else
	    {
		y2milestone( "y2logs saved to %s", qPrintable(fileName) );
	    }
	}
	else
	{
	    y2error( "Error saving y2logs: Command %s not found",
		     qPrintable(saveLogsCommand) );

	    QMessageBox::warning( 0,						// parent
				  "Error",					// caption
				  QString( "Couldn't save y2logs to %1:\n"
					   "Command %2 not found" ).arg( fileName ).arg( saveLogsCommand ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
	}
    }
}


void YQUI::askConfigureLogging()
{
    bool okButtonPressed = false;
    QStringList items;
    items << "Debug logging off"
	  << "Debug logging on";

    QString result = QInputDialog::getItem( _main_win,
                                            _("YaST2 Logging"),
                                            _("Configure YaST2 Logging:"),
                                            items, 0, get_log_debug() ? 1 : 0, &okButtonPressed);
    if ( okButtonPressed )
    {
	set_log_debug( result.endsWith( "on" ) );
	y2milestone( "Changing logging: %s - %s", qPrintable(result),
		     get_log_debug() ? "y2debug on" : "y2debug off" );
    }
}


void YQUI::toggleRecordMacro()
{
    if ( recordingMacro() )
    {
        stopRecordMacro();
        normalCursor();

        QMessageBox::information( 0,                                            // parent
                                  "YaST2 Macro Recorder",                       // caption
                                  "Macro recording done.",                      // text
                                  QMessageBox::Ok | QMessageBox::Default,       // button0
                                  QMessageBox::NoButton,                        // button1
                                  QMessageBox::NoButton );                      // button2
    }
    else
    {
        normalCursor();

        QString filename =
            QFileDialog::getSaveFileName( 0,
                                          "Select Macro File to Record to",
                                          DEFAULT_MACRO_FILE_NAME,              // startWith
                                          "*.ycp"                             // filter
                                          );

        if ( ! filename.isEmpty() )     // file selection dialog has been cancelled
        {
            recordMacro( qPrintable(filename) );
        }
    }
}


void YQUI::askPlayMacro()
{
    normalCursor();

    QString filename =
        QFileDialog::getOpenFileName( 0,
                                      "Select Macro File to Play",
                                      DEFAULT_MACRO_FILE_NAME,          // startWith
                                      "*.ycp" );
    busyCursor();

    if ( ! filename.isEmpty() ) // file selection dialog has been cancelled
    {
        playMacro( qPrintable(filename) );

        // Do special magic to get out of any UserInput() loop right now
        // without doing any harm - otherwise this would hang until the next
        // mouse click on a PushButton etc.

        sendEvent( new YEvent() );

        if ( _do_exit_loop )
        {
            _eventLoop->exit();
        }
    }
}


// EOF
