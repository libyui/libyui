/*************************************************************************************************************

 Copyright (C) 2000 - 2010 Novell, Inc.   All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation; either version 2 of the License, or (at your
 option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*************************************************************************************************************/



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////   __/\\\\\\_____________/\\\__________/\\\________/\\\___/\\\________/\\\___/\\\\\\\\\\\_           ////
 ////    _\////\\\____________\/\\\_________\///\\\____/\\\/___\/\\\_______\/\\\__\/////\\\///__          ////
 ////     ____\/\\\______/\\\__\/\\\___________\///\\\/\\\/_____\/\\\_______\/\\\______\/\\\_____         ////
 ////      ____\/\\\_____\///___\/\\\_____________\///\\\/_______\/\\\_______\/\\\______\/\\\_____        ////
 ////       ____\/\\\______/\\\__\/\\\\\\\\\_________\/\\\________\/\\\_______\/\\\______\/\\\_____       ////
 ////        ____\/\\\_____\/\\\__\/\\\////\\\________\/\\\________\/\\\_______\/\\\______\/\\\_____      ////
 ////         ____\/\\\_____\/\\\__\/\\\__\/\\\________\/\\\________\//\\\______/\\\_______\/\\\_____     ////
 ////          __/\\\\\\\\\__\/\\\__\/\\\\\\\\\_________\/\\\_________\///\\\\\\\\\/_____/\\\\\\\\\\\_    ////
 ////           _\/////////___\///___\/////////__________\///____________\/////////______\///////////__   ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                 widget abstraction library providing Qt, GTK and ncurses frontends                  ////
 ////                                                                                                     ////
 ////                                   3 UIs for the price of one code                                   ////
 ////                                                                                                     ////
 ////                                      ***  NCurses plugin  ***                                       ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                              (C) SUSE Linux GmbH    ////
 ////                                                                                                     ////
 ////                                                              libYUI-AsciiArt (C) 2012 Björn Esser   ////
 ////                                                                                                     ////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-/

   File:       NCBusyIndicator.h

   Author:     Thomas Goettlicher <tgoettlicher@suse.de>
   Maintainer: Thomas Goettlicher <tgoettlicher@suse.de>

/-*/

#ifndef NCBusyIndicator_h
#define NCBusyIndicator_h

#include <iosfwd>

#include <yui/YBusyIndicator.h>
#include "NCWidget.h"


class NCBusyIndicator;





class NCBusyIndicator : public YBusyIndicator, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCBusyIndicator & OBJ );

    NCBusyIndicator & operator=( const NCBusyIndicator & );
    NCBusyIndicator( const NCBusyIndicator & );

    typedef long long Value_t;

    NClabel  _label;
    Value_t  _timeout;
    NCursesWindow * _lwin;
    NCursesWindow * _twin;

    void setDefsze();
    void tUpdate();
    void update();

    float	_position;		// the position of the bar
    bool	_rightwards;		// direction the bar moves
    bool	_alive;			// the widget is alive or stalled
    float	_timer_divisor;		// =repaint interval devided by timeout
    float	_timer_progress;	// progress until widget goes to stalled state


protected:


    virtual const char * location() const { return "NCBusyIndicator"; }

    virtual void wCreate( const wrect & newrect );
    virtual void wDelete();

    virtual void wRedraw();

public:

    NCBusyIndicator( YWidget * parent,
		     const string & label,
		     int timeout = 1000 );
    virtual ~NCBusyIndicator();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    virtual void setTimeout( int newTimeout );

    virtual void setAlive( bool newAlive );

    virtual void setEnabled( bool do_bv );

    int timeout()   const   { return _timeout;	}

    void handler( int sig_num );
    
    static void staticHandler( int sig_num );
};


#endif // NCBusyIndicator_h
