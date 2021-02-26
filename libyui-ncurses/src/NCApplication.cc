/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      	NCApplication.cc

  Authors:	Gabriele Mohr <gs@suse.de>
		Stefan Hundhammer <sh@suse.de>

/-*/


#include <curses.h>


#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "YNCursesUI.h"
#include "NCApplication.h"
#include "NCAskForDirectory.h"
#include "NCAskForFile.h"


NCApplication::NCApplication()
{
    
}


NCApplication::~NCApplication()
{
    
}


void
NCApplication::setLanguage( const string & language,
			    const string & encoding )
{
    // Intentionally NOT calling
    //    YApplication::setLanguage( language, encoding );
    // This would implicitly overwrite LC_CTYPE which might result in encoding bugs.
    
    setlocale( LC_NUMERIC, "C" );	// always format numbers with "."
    NCurses::Refresh();
    
    yuiDebug() << "Language: " << language << " Encoding: " << ((encoding!="")?encoding:"NOT SET") << endl;
  
}


string
NCApplication::askForSaveFileName( const string & startDir,
				   const string & filter,
				   const string & headline )
{
    NCAskForSaveFileName * filePopup = new NCAskForSaveFileName( wpos( 1, 1 ), startDir, filter, headline );
    YUI_CHECK_NEW( filePopup );
    
    NCursesEvent retEvent = filePopup->showDirPopup( );
    YDialog::deleteTopmostDialog();

    yuiMilestone() << "Returning: " <<  retEvent.result << endl;
    return retEvent.result;
}


string
NCApplication::askForExistingFile( const string & startDir,
				   const string & filter,
				   const string & headline )
{
    NCAskForExistingFile * filePopup = new NCAskForExistingFile( wpos( 1, 1 ), startDir, filter, headline );
    YUI_CHECK_NEW( filePopup );
    
    NCursesEvent retEvent = filePopup->showDirPopup( );
    YDialog::deleteTopmostDialog();

    yuiMilestone() << "Returning: " <<  retEvent.result << endl;
    return retEvent.result;
}


string
NCApplication::askForExistingDirectory( const string & startDir,
					const string & headline )
{
    NCAskForExistingDirectory * dirPopup = new NCAskForExistingDirectory( wpos( 1, 1 ), startDir, headline );
    YUI_CHECK_NEW( dirPopup );
    
    NCursesEvent retEvent = dirPopup->showDirPopup( );
    YDialog::deleteTopmostDialog();

    yuiMilestone() << "Returning: " <<  retEvent.result << endl;
    return retEvent.result;
}


void
NCApplication::beep()
{
  ::beep();
}


void NCApplication::redrawScreen()
{
    YNCursesUI::ui()->Refresh();
}


void
NCApplication::initConsoleKeyboard()
{
    string cmd = "/bin/dumpkeys | /bin/loadkeys --unicode";
    if ( NCstring::terminalEncoding() == "UTF-8" )
    {
	int ret = system( (cmd + " >/dev/null 2>&1").c_str() );
	if ( ret != 0 )
	{
	    yuiError() << "ERROR: /bin/dumpkeys | /bin/loadkeys --unicode returned: "<< ret << endl;
	}
    }
}


void
NCApplication::setConsoleFont( const string & console_magic,
			       const string & font,
			       const string & screen_map,
			       const string & unicode_map,
			       const string & language )
{
    /**
     * Moving that code from YNCursesUI to this class turned out to be
     * impossible (or at least a lot more work than it's worth) that I finally
     * gave it up.
     *
     * - sh@suse.de 2008-02-06
     **/
    YNCursesUI::ui()->setConsoleFont( console_magic,
				      font,
				      screen_map,
				      unicode_map,
				      language );
}


int
NCApplication::runInTerminal( const string & cmd )
{ 
    int ret;

    // Save tty modes and end ncurses mode temporarily
    ::def_prog_mode();
    ::endwin();

    // Regenerate saved stdout and stderr, so that app called
    // via system() can use them and draw something to the terminal
    dup2( YNCursesUI::ui()->stdout_save, 1 );
    dup2( YNCursesUI::ui()->stderr_save, 2 );

    // Call external program
    ret = system( cmd.c_str() );

    if ( ret != 0 )
    {
	yuiError() << cmd << " returned:" << ret << endl;
    }

    // Redirect stdout and stderr to y2log again
    YNCursesUI::ui()->RedirectToLog();

    // Resume tty modes and refresh the screen
    ::reset_prog_mode();
    ::refresh();

    return ret;
}


int
NCApplication::displayWidth()
{
    return ::COLS;	// exported from ncurses.h
}

int
NCApplication::displayHeight()
{
    return ::LINES;	// exported from ncurses.h
}

int
NCApplication::displayDepth()
{
    return -1;
}

long
NCApplication::displayColors()
{
    return NCattribute::colors();
}

int
NCApplication::defaultWidth()
{
    return ::COLS;	// exported from ncurses.h
}

int
NCApplication::defaultHeight()
{
    return ::LINES;	// exported from ncurses.h
}

bool
NCApplication::hasFullUtf8Support()
{
    return ( NCstring::terminalEncoding() == "UTF-8" );
}
