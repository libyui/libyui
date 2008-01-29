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

   File:       YNCursesUI.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
// -*- c++ -*-

#ifndef YNCursesUI_h
#define YNCursesUI_h

#include <iosfwd>

#include <Y2.h>
#include "YUI.h"

#include "NCApplication.h"

#include "NCurses.h"

class YUI;
class NCDialog;
class NCPackageSelectorPlugin;
class NCWidgetFactory;
class NCOptionalWidgetFactory;
class YSingleChildContainerWidget;


/**
 * @short YaST2 Component: NCursesUI user interface
 * The YaST2 Component realizes a NCursesUI based user interface with an
 * embedded YCP interpreter.
 */
class YNCursesUI : public NCurses, public YUI
{
public:
   /**
    * Initialize data.
     */
   YNCursesUI( int argc, char ** argv, bool with_threads, const char * macro_file );

   /**
     * Cleans up.
     */
   ~YNCursesUI();


  protected:
    /**
     * Create the widget factory that provides all the createXY() methods for
     * standard (mandatory, i.e. non-optional) widgets.
     *
     * Reimplemented from YUI.
     **/
    virtual YWidgetFactory * createWidgetFactory();

    /**
     * Create the widget factory that provides all the createXY() methods for
     * optional ("special") widgets and the corresponding hasXYWidget()
     * methods. 
     *
     * Reimplemented from YUI.
     **/
    virtual YOptionalWidgetFactory * createOptionalWidgetFactory();

    /*
     * Create the YApplication object that provides global methods.
     *
     * Reimplemented from YUI.
     **/
    virtual YApplication * createApplication();
    

    virtual bool want_colors();
    virtual void init_title();

   /**
    * Global reference to the UI
    **/
   static YNCursesUI * _ui;
    
  public:

//    Y2NCursesUI( bool with_threads, const char * macro_file, Y2Component *callback );
//    virtual ~Y2NCursesUI();

   /**
    * Access the global Y2NCursesUI.
    */
    static YNCursesUI * ui() { return _ui; }
    
    /**
     * Idle around until fd_ycp is readable
    */
    virtual void idleLoop( int fd_ycp );

    /**
     * Go into event loop until user input is available or until the specified
     * timeout (in milliseconds) has elapsed. 
     *
     * Reimplemented from YUIInterpreter.
     */
    virtual YEvent * userInput( unsigned long timeout_millisec );

    /**
     * Check the event queue for user input. Don't wait.
     *
     * Reimplemented from YUIInterpreter.
     */
    virtual YEvent * pollInput();

    /**
     * Inherited from YUIInterpreter. Shows and activates a dialog
    */
    virtual void showDialog( YDialog * dialog );

    /**
     * Inherited from YUIInterpreter. Decativates and closes a dialog
    */
    virtual void closeDialog( YDialog * dialog );

    /**
     * Fills the PackageSelector widget and runs package selection.
     */
    virtual YEvent * runPkgSelection( YWidget * packageSelector );
    
    /**
     * UI-specific setConsoleFont() function.
     * Returns YCPVoid() if OK and YCPNull() on error.
    */
    virtual YCPValue setConsoleFont ( const YCPString & console_magic,
				      const YCPString & font,
				      const YCPString & screen_map,
				      const YCPString & unicode_map,
				      const YCPString & encoding );

    virtual YCPValue setKeyboard ( );

    /**
     * UI-specific runInTerminal() function.
     * Returns (integer) return code of external program it spawns 
     * in the same terminal
    */

    virtual int runInTerminal ( const YCPString & module);

    /**
     * UI-specific beep() function.
     * Beeps the system bell.
    */
    virtual void beep();

    /**
     * Returns the package selector plugin singleton of this UI or creates it
     * (including loading the plugin lib) if it does not exist yet.
     **/
    NCPackageSelectorPlugin * packageSelectorPlugin();
    
    /**
     * UI-specific redrawScreen method.
     * This default implementation does nothing.
    */
    virtual void redrawScreen() { Refresh(); }

    /**
     * UI-specific getDisplayInfo() functions.
     * See UI builtin GetDisplayInfo() doc for details.
     **/
    virtual int  getDisplayWidth()           { return cols(); }
    virtual int  getDisplayHeight()          { return lines(); }
    virtual int  getDisplayDepth()           { return -1; }
    virtual long getDisplayColors()          { return NCattribute::colors(); }
    virtual int  getDefaultWidth()           { return cols(); }
    virtual int  getDefaultHeight()          { return lines(); }
    virtual bool textMode()                  { return true; }
    virtual bool hasImageSupport()           { return false; }
    virtual bool hasIconSupport()            { return false; }
    
    virtual bool hasFullUtf8Support();
};

#endif // YNCursesUI_h

