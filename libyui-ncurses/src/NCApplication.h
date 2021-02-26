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

  File:	      NCApplication.h

  Author:     Gabriele Mohr <gs@suse.de>

/-*/

#ifndef NCApplication_h
#define NCApplication_h

#include <yui/YApplication.h>


class NCApplication: public YApplication
{

protected:

    friend class YNCursesUI;

    /**
     * Constructor.
     *
     * Use YUI::app() to get the singleton for this class.
     **/
    NCApplication();

    /**
     * Destructor.
     **/
    virtual ~NCApplication();

public:

    /**
     * Set language and encoding for the locale environment ($LANG).
     *
     * 'language' is the ISO short code ("de_DE", "en_US", ...).
     *
     * 'encoding' an (optional) encoding ("utf8", ...) that will be appended if
     *	present.
     *
     * Reimplemented from YApplication.
     **/
    virtual void setLanguage( const std::string & language,
			      const std::string & encoding = std::string() );

    /**
     * Open a directory selection box and prompt the user for an existing
     * directory.
     *
     * 'startDir' is the initial directory that is displayed.
     *
     * 'headline' is an explanatory text for the directory selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected directory name
     * or an empty std::string if the user canceled the operation.
     *
     * Implemented from YApplication.
     **/
    virtual std::string askForExistingDirectory( const std::string & startDir,
					    const std::string & headline );

    /**
     * Open a file selection box and prompt the user for an existing file.
     *
     * 'startWith' is the initial directory or file.
     *
     * 'filter' is one or more blank-separated file patterns, e.g.
     * "*.png *.jpg"
     *
     * 'headline' is an explanatory text for the file selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected file name
     * or an empty std::string if the user canceled the operation.
     *
     * Implemented from YApplication.
     **/
    virtual std::string askForExistingFile( const std::string & startWith,
				       const std::string & filter,
				       const std::string & headline );

    /**
     * Open a file selection box and prompt the user for a file to save data
     * to.  Automatically asks for confirmation if the user selects an existing
     * file.
     *
     * 'startWith' is the initial directory or file.
     *
     * 'filter' is one or more blank-separated file patterns, e.g.
     * "*.png *.jpg"
     *
     * 'headline' is an explanatory text for the file selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected file name
     * or an empty std::string if the user canceled the operation.
     *
     * Implemented from YApplication.
     **/
    virtual std::string askForSaveFileName( const std::string & startWith,
				       const std::string & filter,
				       const std::string & headline );

    /**
     * Beep.
     *
     * Reimplemented from YApplication.
     **/
    virtual void beep();

    /**
     * Redraw the screen.
     *
     * Reimplemented from YApplication.
     **/
    virtual void redrawScreen();

    /**
     * Initialize the (text) console keyboard.
     *
     * Reimplemented from YApplication.
     **/
    virtual void initConsoleKeyboard();

    /**
     * Set the (text) console font according to the current encoding etc.
     * See the setfont(8) command and the console HowTo for details.
     *
     * Reimplemented from YApplication.
     **/
    virtual void setConsoleFont( const std::string & console_magic,
				 const std::string & font,
				 const std::string & screen_map,
				 const std::string & unicode_map,
				 const std::string & language );

    /**
     * Run a shell command (typically an interactive program using NCurses)
     * in a terminal (window).
     *
     * Here in the NCurses UI, this shuts down the NCurses lib, runs the
     * command and then restores the status of the NCurses lib so that the next
     * instance of the NCurses lib from the started command doesn't interfere
     * with the NCurses UI's instance.
     *
     * Reimplemented from YApplication.
     **/
    virtual int runInTerminal( const std::string & command );


    /// @{
    /**
     * To mix TUI (NCurses) with stdio, enclose the UI parts
     * within openUI/closeUI
     *
     * Reimplemented from YApplication.
     */
    virtual void openUI() override;
    virtual void closeUI() override;
    /// @}

    // Display information and UI capabilities.
    //
    // All implemented from YApplication.

    virtual int	 displayWidth();
    virtual int	 displayHeight();
    virtual int	 displayDepth();
    virtual long displayColors();

    virtual int	 defaultWidth();
    virtual int	 defaultHeight();

    virtual bool isTextMode()			{ return true; }

    virtual bool hasImageSupport()		{ return false; }

    virtual bool hasIconSupport()		{ return false; }

    virtual bool hasAnimationSupport()		{ return false; }

    virtual bool hasFullUtf8Support();
    virtual bool richTextSupportsTable()	{ return false; }

    virtual bool leftHandedMouse()		{ return false; }
    
    /**
     * Set the application title
     *
     * Reimplemented from YApplication.
     **/
    virtual void setApplicationTitle(const std::string& title);
};


#endif // NCApplication_h
