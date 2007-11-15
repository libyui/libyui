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

#include <qcursor.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qinputdialog.h>
#include <qvbox.h>

#include <ycp/YCPTerm.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YQUI.h"
#include "YEvent.h"
#include "YMacroRecorder.h"
#include "YUISymbols.h"
#include "YQDialog.h"
#include "YQSignalBlocker.h"

#include "utf8.h"
#include "YQi18n.h"

#include <X11/Xlib.h>


#define DEFAULT_MACRO_FILE_NAME         "macro.ycp"



YCPValue YQUI::setLanguage( const YCPTerm & term)
{
    loadPredefinedQtTranslations();

    if ( term->size() > 0 && term->value(0)->isString() )
	setLangFonts( term->value(0)->asString() );

    return YCPVoid();   // OK (YCPNull() would mean error)
}


YCPString
YQUI::glyph( const YCPSymbol & glyphSymbol )
{
    string sym = glyphSymbol->symbol();
    QChar unicodeChar;

    // Hint: Use 'xfd' to view characters available in the Unicode font.

    if      ( sym == YUIGlyph_ArrowLeft         )       unicodeChar = QChar( 0x2190 );
    else if ( sym == YUIGlyph_ArrowRight        )       unicodeChar = QChar( 0x2192 );
    else if ( sym == YUIGlyph_ArrowUp           )       unicodeChar = QChar( 0x2191 );
    else if ( sym == YUIGlyph_ArrowDown         )       unicodeChar = QChar( 0x2193 );
    else if ( sym == YUIGlyph_CheckMark         )       unicodeChar = QChar( 0x2714 );
    else if ( sym == YUIGlyph_BulletArrowRight  )       unicodeChar = QChar( 0x279c );
    else if ( sym == YUIGlyph_BulletCircle      )       unicodeChar = QChar( 0x274d );
    else if ( sym == YUIGlyph_BulletSquare      )       unicodeChar = QChar( 0x274f );
    else return YCPString( "" );

    QString qstr( unicodeChar );

    return YCPString( toUTF8( qstr ) );
}


YCPValue YQUI::runPkgSelection( YWidget * packageSelector )
{
    y2milestone( "Running package selection..." );
    _wm_close_blocked           = true;
    _auto_activate_dialogs      = false;

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

    _auto_activate_dialogs      = true;
    _wm_close_blocked           = false;
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
    XSync( dialog->x11Display(), false );
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

            QString home = QDir::homeDirPath();
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

                if ( mkdir( dir, 0700 ) == -1 )
                    dir = "";
            }
            else
            {
                // For all others let's create a directory ~/yast2-screen-shots and
                // simply ignore if this is already present. This gives the user a
                // chance to create symlinks to a better location if he wishes so.

                dir = home + "/" + dir;
                (void) mkdir( dir, 0750 );
            }

            screenShotNameTemplate = dir + "/%s-%03d.png";
        }


        //
        // Figure out a file name
        //

        const char * baseName = moduleName();
        if ( ! baseName ) baseName = "scr";
        int no = screenShotNo[ baseName ];
        fileName.sprintf( screenShotNameTemplate, baseName, no );
        y2debug( "screenshot: %s", (const char *) fileName );

	{
	    YQSignalBlocker sigBlocker( &_user_input_timer );

	    fileName = askForSaveFileName( fileName, QString( "*.png" ) , _( "Save screen shot to..." ) );
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

    y2debug( "Saving screen shot to %s", (const char *) fileName );
    bool success = screenShot.save( fileName, "PNG" );

    if ( ! success )
    {
	y2error( "Couldn't save screen shot %s", (const char *) fileName );

	if ( interactive )
	{
	    QMessageBox::warning( 0,						// parent
				  "Error",					// caption
				  QString( "Couldn't save screen shot\nto %1" ).arg( fileName ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
	}
    }

    if ( recordingMacro() )
    {
	macroRecorder->beginBlock();
	YDialog::currentDialog()->saveUserInput( macroRecorder );
	macroRecorder->recordMakeScreenShot( true, (const char *) fileName );
	macroRecorder->recordUserInput( YCPVoid() );
	macroRecorder->endBlock();
    }
}


void YQUI::askSaveLogs()
{
    QString fileName = askForSaveFileName( "/tmp/y2logs.tgz",			// startWith
					   "*.tgz *.tar.gz",			// filter
					   "Save y2logs to..." );		// headline

    if ( ! fileName.isEmpty() )
    {
	QString saveLogsCommand = "/sbin/save_y2logs";

	if ( access( saveLogsCommand.ascii(), X_OK ) == 0 )
	{
	    saveLogsCommand += " '" + fileName + "'";
	    y2milestone( "Saving y2logs: %s", saveLogsCommand.ascii() );
	    int result = system( saveLogsCommand.ascii() );

	    if ( result != 0 )
	    {
		y2error( "Error saving y2logs: \"%s\" exited with %d",
			 (const char *) saveLogsCommand, result );
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
		y2milestone( "y2logs saved to %s", (const char *) fileName );
	    }
	}
	else
	{
	    y2error( "Error saving y2logs: Command %s not found",
		     saveLogsCommand.ascii() );

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

    QString result = QInputDialog::getItem( "YaST2 Logging",		// caption
					    "Configure YaST2 Logging:",	// label
					    items,
					    get_log_debug() ? 1 : 0,
					    false,			// editable
					    &okButtonPressed,
					    _main_win );		// parent

    if ( okButtonPressed )
    {
	set_log_debug( result.endsWith( "on" ) );
	y2milestone( "Changing logging: %s - %s", (const char *) result,
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
            QFileDialog::getSaveFileName( DEFAULT_MACRO_FILE_NAME,              // startWith
                                          "*.ycp",                              // filter
                                          0,                                    // parent
                                          0,                                    // (widget) name
                                          "Select Macro File to Record to" );   // caption

        if ( ! filename.isEmpty() )     // file selection dialog has been cancelled
        {
            recordMacro( (const char *) filename );
        }
    }
}


void YQUI::askPlayMacro()
{
    normalCursor();

    QString filename =
        QFileDialog::getOpenFileName( DEFAULT_MACRO_FILE_NAME,          // startWith
                                      "*.ycp",                          // filter
                                      0,                                // parent
                                      0,                                // (widget) name
                                      "Select Macro File to Play" );    // caption
    busyCursor();

    if ( ! filename.isEmpty() ) // file selection dialog has been cancelled
    {
        playMacro( (const char *) filename );

        // Do special magic to get out of any UserInput() loop right now
        // without doing any harm - otherwise this would hang until the next
        // mouse click on a PushButton etc.

        sendEvent( new YEvent() );

        if ( _do_exit_loop )
        {
            qApp->exit_loop();
        }
    }
}



YCPValue YQUI::askForExistingDirectory( const YCPString & startDir,
                                         const YCPString & headline )
{
    normalCursor();

    QString dir_name =
        QFileDialog::getExistingDirectory( fromUTF8( startDir->value() ),
                                           _main_win,                           // parent
                                           "dir_selector",                      // name
                                           fromUTF8( headline->value() ) );     // caption
    busyCursor();

    if ( dir_name.isEmpty() )   // this includes dir_name.isNull()
        return YCPVoid();       // nothing selected -> return 'nil'

    return YCPString( toUTF8( dir_name ) );
}


YCPValue YQUI::askForExistingFile( const YCPString & startWith,
				   const YCPString & filter,
				   const YCPString & headline )
{
    normalCursor();

    QString file_name =
        QFileDialog::getOpenFileName( fromUTF8( startWith->value() ),
                                      fromUTF8( filter->value() ),
                                      _main_win,                        // parent
                                      "file_selector",                  // name
                                      fromUTF8( headline->value() ) );  // caption
    busyCursor();

    if ( file_name.isEmpty() )  // this includes file_name.isNull()
        return YCPVoid();       // nothing selected -> return 'nil'

    return YCPString( toUTF8( file_name ) );
}


YCPValue YQUI::askForSaveFileName( const YCPString & startWith,
				   const YCPString & filter,
				   const YCPString & headline )
{
    normalCursor();

    QString file_name = askForSaveFileName( fromUTF8( startWith->value() ),
                                            fromUTF8( filter->value() ),
                                            fromUTF8( headline->value() ) );
    busyCursor();

    if ( file_name.isEmpty() )          // this includes file_name.isNull()
        return YCPVoid();               // nothing selected -> return 'nil'

    return YCPString( toUTF8( file_name ) );
}



QString YQUI::askForSaveFileName( const QString & startWith,
				  const QString & filter,
				  const QString & headline )
{
    QString file_name;
    bool try_again = false;

    do
    {
        // Leave the mouse cursor alone - this function might be called from
        // some other widget, not only from UI::AskForSaveFileName().

        file_name = QFileDialog::getSaveFileName( startWith,
                                                  filter,
                                                  _main_win,            // parent
                                                  "file_selector",      // name
                                                  headline );           // caption

        if ( file_name.isEmpty() )      // this includes file_name.isNull()
            return QString::null;


        if ( access( (const char *) file_name, F_OK ) == 0 )    // file exists?
        {
            QString msg;

            if ( access( (const char *) file_name, W_OK ) == 0 )
            {
                // Confirm if the user wishes to overwrite an existing file
                msg = ( _( "%1 exists! Really overwrite?" ) ).arg( file_name );
            }
            else
            {
                // Confirm if the user wishes to overwrite a write-protected file %1
                msg = ( _( "%1 exists and is write-protected!\nReally overwrite?" ) ).arg( file_name );
            }

            int button_no = QMessageBox::information( _main_win,
                                                      // Window title for confirmation dialog
                                                      _( "Confirm"   ),
                                                      msg,
                                                      _( "C&ontinue" ),
                                                      _( "&Cancel"   ) );
            try_again = ( button_no != 0 );
        }

    } while ( try_again );

    return file_name;
}


// EOF
