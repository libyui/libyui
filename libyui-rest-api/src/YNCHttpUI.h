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

#ifndef YNCHttpUI_h
#define YNCHttpUI_h

#include "YNCursesUI.h"
#include "NCApplication.h"
#include "NCurses.h"
#include <yui/YUI.h>

class YNCHttpUI: public YNCursesUI
{

public:
    /**
     * Constructor.
     **/
    YNCHttpUI( bool withThreads );

    /**
     * Destructor.
     **/
    // ~YNCHttpUI() { delete ui_; };

    /**
     * Idle around until fd_ycp is readable
     */
    virtual void idleLoop( int fd_ycp );

    /**
     * Access the global Y2NCursesUI.
     */
    static YNCHttpUI * ui() { return _ui; }

protected:
    /**
     * Global reference to the UI
     **/
    static YNCHttpUI * _ui;
};

/**
 * Create a new UI if there is none yet or return the existing one if there is.
 *
 * This is the UI plugin's interface to the outside world, so don't change the
 * name or signature!
 **/
extern "C" {
    YUI * createYNCHttpUI( bool withThreads );
}

#endif // YNCHttpUI_h
