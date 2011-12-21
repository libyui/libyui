/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/

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

    friend ostream & operator<<( std::ostream & stream, const NCtoY2Event & event );

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
