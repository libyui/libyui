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

   File:       NCtoY2Event.h

   Authors:    Michael Andres <ma@suse.de>
	       Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef NCtoY2Event_h
#define NCtoY2Event_h

#include <NCurses.h>	// NCursesEvent, iostream


/**
 * Helper class for translating an NCurses event to a YEvent.
 **/

class NCtoY2Event : public NCursesEvent
{

public:

    friend std::ostream & operator<<( std::ostream & stream, const NCtoY2Event & event );

    /**
     * Default constructor
     **/
    NCtoY2Event() {}

    /**
     * Constructor from an NCursesEvent
     **/
    NCtoY2Event( const NCursesEvent & ncev );

    /**
     * Assignment operator
     **/
    NCtoY2Event & operator=( const NCursesEvent & ncev );

    /**
     * The reason of existence of this class:
     * Translate the NCursesEvent to a YEvent.
     **/
    YEvent * propagate();
};


#endif // ifndef NCtoY2Event_h
