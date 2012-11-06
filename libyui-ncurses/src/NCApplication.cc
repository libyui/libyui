/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:		NCApplication.cc

  Authors:	Gabriele Mohr <gs@suse.de>
		Stefan Hundhammer <sh@suse.de>

/-*/

#include <ncursesw/curses.h>

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
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
NCApplication::setLanguage( const std::string & language,
			    const std::string & encoding )
{
    // Intentionally NOT calling
    //	  YApplication::setLanguage( language, encoding );
    // This would implicitly overwrite LC_CTYPE which might result in encoding bugs.

    setlocale( LC_NUMERIC, "C" );	// always format numbers with "."
    NCurses::Refresh();

    yuiDebug() << "Language: " << language << " Encoding: " << (( encoding != "" ) ? encoding : "NOT SET" ) << std::endl;

}


std::string
NCApplication::askForSaveFileName( const std::string & startDir,
				   const std::string & filter,
				   const std::string & headline )
{
    NCAskForSaveFileName * filePopup = new NCAskForSaveFileName( wpos( 1, 1 ), startDir, filter, headline );
    YUI_CHECK_NEW( filePopup );

    NCursesEvent retEvent = filePopup->showDirPopup( );
    YDialog::deleteTopmostDialog();

    yuiMilestone() << "Returning: " <<	retEvent.result << std::endl;
    return retEvent.result;
}


std::string
NCApplication::askForExistingFile( const std::string & startDir,
				   const std::string & filter,
				   const std::string & headline )
{
    NCAskForExistingFile * filePopup = new NCAskForExistingFile( wpos( 1, 1 ), startDir, filter, headline );
    YUI_CHECK_NEW( filePopup );

    NCursesEvent retEvent = filePopup->showDirPopup( );
    YDialog::deleteTopmostDialog();

    yuiMilestone() << "Returning: " <<	retEvent.result << std::endl;
    return retEvent.result;
}


std::string
NCApplication::askForExistingDirectory( const std::string & startDir,
					const std::string & headline )
{
    NCAskForExistingDirectory * dirPopup = new NCAskForExistingDirectory( wpos( 1, 1 ), startDir, headline );
    YUI_CHECK_NEW( dirPopup );

    NCursesEvent retEvent = dirPopup->showDirPopup( );
    YDialog::deleteTopmostDialog();

    yuiMilestone() << "Returning: " <<	retEvent.result << std::endl;
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
    /*
     * Following code breaks the console keyboard e.g. for czech language during
     * installation (bnc #433016). According to bnc #367801 comment #18/#19 the
     * line isn't needed at all.
     * "dumpkeys | loadkeys -C "$KBD_TTY" --unicode" has been also removed from kbd
     * initscript. If dumpkeys has to be called for any reason it definitely needs
     * the codepage argument, otherwise it cannot work.
     */
#if 0
    std::string cmd = "/bin/dumpkeys | /bin/loadkeys --unicode";

    if ( NCstring::terminalEncoding() == "UTF-8" )
    {
	int ret = system(( cmd + " >/dev/null 2>&1" ).c_str() );

	if ( ret != 0 )
	{
	    yuiError() << "ERROR: /bin/dumpkeys | /bin/loadkeys --unicode returned: " << ret << std::endl;
	}
    }
#endif
}


void
NCApplication::setConsoleFont( const std::string & console_magic,
			       const std::string & font,
			       const std::string & screen_map,
			       const std::string & unicode_map,
			       const std::string & language )
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
NCApplication::runInTerminal( const std::string & cmd )
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
	yuiError() << cmd << " returned:" << ret << std::endl;
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

void NCApplication::setApplicationTitle ( const std::string& title )
{
  YApplication::setApplicationTitle ( title );
  NCurses::SetTitle(title);
}

