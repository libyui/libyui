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
		     const std::string & label,
		     int timeout = 1000 );
    virtual ~NCBusyIndicator();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const std::string & nlabel );

    virtual void setTimeout( int newTimeout );

    virtual void setAlive( bool newAlive );

    virtual void setEnabled( bool do_bv );

    int timeout()   const   { return _timeout;	}

    void handler( int sig_num );

    static void staticHandler( int sig_num );
};


#endif // NCBusyIndicator_h
