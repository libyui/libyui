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

#include "NCtoY2Event.h"
#include "NCWidget.h"

#define  YUILogComponent "ncurses"
#include <YUILog.h>


NCtoY2Event::NCtoY2Event( const NCursesEvent & ncev )
	: NCursesEvent( ncev )
{
}


NCtoY2Event &
NCtoY2Event::operator=( const NCursesEvent & ncev )
{
    if ( ncev.isInternalEvent() )
	NCursesEvent::operator=( none );
    else
	NCursesEvent::operator=( ncev );

    return *this;
}


YEvent *
NCtoY2Event::propagate()
{
    switch ( type )
    {
	// Note: libyui assumes ownership of YEvents, so they need to be
	// created on the heap with 'new'. libyui takes care of deleting them.

	case button:

	    if ( widget && widget->isValid() )
		return new YWidgetEvent( dynamic_cast<YWidget *>( widget ), reason );
	    else
		return 0;

	case menu:
	    if ( selection && widget && widget->isValid() )
		return new YMenuEvent( selection );
	    else
		return 0;

	case cancel:
	    return new YCancelEvent();

	case timeout:
	    return new YTimeoutEvent();

	case key:
	    if ( widget && widget->isValid() )
		return new YKeyEvent( keySymbol, dynamic_cast<YWidget *>( widget ) );
	    else
		return 0;

	case none:
	case handled:
	    return 0;

	    // Intentionally omitting 'default' branch so the compiler can
	    // detect unhandled enums
    }

    // If we get this far, there must be an error.

    yuiMilestone() << "Can't propagate through (EventType*)0" << endl;

    yuiDebug() << *this << endl;

    return 0;
}


ostream &
operator<< ( std::ostream & stream, const NCtoY2Event & event )
{
    stream << static_cast<const NCursesEvent &>( event );

    if ( ! event.selection )
    {
	stream << "(-)";
    }
    else
    {
	// 'selection' is used in NCMenuButton and NCRichtText (for hyper links)
	stream << "(" << event.selection->label() << ")";
    }

    return stream << " for " << event.widget;
}

