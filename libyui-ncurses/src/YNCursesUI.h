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

   File:       YNCursesUI.h

   Authors:    Michael Andres <ma@suse.de>
	       Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YNCursesUI_h
#define YNCursesUI_h

#include <iosfwd>

#include <yui/YUI.h>
#include "NCApplication.h"
#include "NCurses.h"

class NCPackageSelectorPluginStub;


class YNCursesUI : public NCurses, public YUI
{
public:
    /**
     * Constructor
     **/
    YNCursesUI( bool withThreads );

    /**
     * Destructor
     **/
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

    /**
     * Access the global Y2NCursesUI.
     */
    static YNCursesUI * ui() { return _ui; }

    /**
     * Idle around until fd_ycp is readable
     */
    virtual void idleLoop( int fd_ycp );

    /**
     * Set the (text) console font according to the current encoding etc.
     * See the setfont(8) command and the console HowTo for details.
     *
     * This should really be in NCApplication, but it uses so many non-exported
     * member variables that it's not easy to move it there.
     **/
    virtual void setConsoleFont( const std::string & console_magic,
				 const std::string & font,
				 const std::string & screen_map,
				 const std::string & unicode_map,
				 const std::string & lang );

    /**
     * Fills the PackageSelector widget and runs package selection.
     */
    virtual YEvent * runPkgSelection( YWidget * packageSelector );

    /**
     * Returns the package selector plugin singleton of this UI or creates it
     * (including loading the plugin lib) if it does not exist yet.
     **/
    NCPackageSelectorPluginStub * packageSelectorPlugin();
};


/**
 * Create a new UI if there is none yet or return the existing one if there is.
 *
 * This is the UI plugin's interface to the outside world, so don't change the
 * name or signature!
 **/
YUI * createUI( bool withThreads );


#endif // YNCursesUI_h

