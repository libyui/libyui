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

   File:       NCBusyIndicator.h

   Author:     Thomas Goettlicher <tgoettlicher@suse.de>
   Maintainer: Thomas Goettlicher <tgoettlicher@suse.de>

/-*/

#ifndef NCBusyIndicator_h
#define NCBusyIndicator_h

#include <iosfwd>

#include "YBusyIndicator.h"
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
