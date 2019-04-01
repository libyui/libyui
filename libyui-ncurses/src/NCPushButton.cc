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

   File:       NCPushButton.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include <yui/YDialog.h>
#include "NCDialog.h"
#include "NCurses.h"
#include "NCPushButton.h"


NCPushButton::NCPushButton( YWidget * parent, const std::string & nlabel )
	: YPushButton( parent, nlabel )
	, NCWidget( parent )
{
    yuiDebug() << std::endl;
    setLabel( nlabel );
    hotlabel = &label;
}


NCPushButton::~NCPushButton()
{
    yuiDebug() << std::endl;
}


int NCPushButton::preferredWidth()
{
    wsze defsize = wGetDefsze();
    return wGetDefsze().W;
}


int NCPushButton::preferredHeight()
{
    return wGetDefsze().H;
}


void NCPushButton::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YPushButton::setEnabled( do_bv );
}


void NCPushButton::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


NCursesEvent NCPushButton::wHandleInput( wint_t key )
{
    NCursesEvent ret;

    switch ( key )
    {
	case KEY_HOTKEY:
	case KEY_RETURN:
	    ret = NCursesEvent::Activated;
	    break;
    }

    return ret;
}


void NCPushButton::setLabel( const std::string & nlabel )
{
    label = NCstring( nlabel );
    label.stripHotkey();
    defsze = wsze( label.height(), label.width() + 2 );
    YPushButton::setLabel( nlabel );
    Redraw();
}


void NCPushButton::wRedraw()
{
    if ( !win )
	return;

    const NCstyle::StWidget & style( widgetStyle() );

    win->bkgd( style.plain );

    win->clear();

    if ( label.height() <= 1 )
    {
	win->printw( 0, 0, "[" );
	win->printw( 0, win->maxx(), "]" );
    }
    else
    {
	win->box();
    }

    label.drawAt( *win, style, wpos( 0, 1 ), wsze( -1, win->width() - 2 ),
		  NC::CENTER );
}

void NCPushButton::activate()
{
	// focus the button
	setKeyboardFocus();
	// and fake pressing the RETURN key
	::ungetch(KEY_RETURN);
}
