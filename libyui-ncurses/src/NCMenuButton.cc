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

   File:       NCMenuButton.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCMenuButton.h"
#include "NCPopupMenu.h"
#include "YNCursesUI.h"


NCMenuButton::NCMenuButton( YWidget * parent,
			    std::string nlabel )
	: YMenuButton( parent, nlabel )
	, NCWidget( parent )
{
    // yuiDebug() << std::endl;
    setLabel( nlabel );
    hotlabel = &label;
}


NCMenuButton::~NCMenuButton()
{
    // yuiDebug() << std::endl;
}


int NCMenuButton::preferredWidth()
{
    return wGetDefsze().W;
}


int NCMenuButton::preferredHeight()
{
    return wGetDefsze().H;
}


void NCMenuButton::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YMenuButton::setEnabled( do_bv );
}


void NCMenuButton::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


NCursesEvent NCMenuButton::wHandleInput( wint_t key )
{
    NCursesEvent ret;

    switch ( key )
    {
	case KEY_HOTKEY:
	case KEY_SPACE:
	case KEY_RETURN:
	case KEY_DOWN:
	    ret = postMenu();
	    break;
    }

    return ret;
}


void NCMenuButton::setLabel( const std::string & nlabel )
{
    label = NCstring( nlabel );
    label.stripHotkey();
    defsze = wsze( label.height(), label.width() + 3 );
    YMenuButton::setLabel( nlabel );
    Redraw();
}


void NCMenuButton::wRedraw()
{
    if ( !win )
	return;

    const NCstyle::StWidget & style( widgetStyle() );

    win->bkgdset( style.plain );

    if ( label.height() > 1 )
    {
	win->box( wrect( 0, win->size() - wsze( 0, 1 ) ) );
    }

    win->printw( 0, 0, "[" );

    win->printw( 0, win->maxx(), "]" );

    label.drawAt( *win, style, wpos( 0, 1 ), wsze( -1, win->width() - 3 ),
		  NC::CENTER );

    win->bkgdset( style.scrl );
    win->vline( 0, win->maxx() - 1, win->height(), ' ' );
    haveUtf8() ?
    win->add_wch( 0, win->maxx() - 1, WACS_DARROW )
    : win->addch( 0, win->maxx() - 1, ACS_DARROW );
}


void NCMenuButton::rebuildMenuTree()
{
    // NOP
}


NCursesEvent NCMenuButton::postMenu()
{
    // Add fixed heigth of 1;
    // dont't use win->height() because win might be invalid (bnc#931154)
    wpos at( ScreenPos() + wpos( 1, 0 ) );

    NCPopupMenu * dialog = new NCPopupMenu( at,
					    itemsBegin(),
					    itemsEnd() );
    YUI_CHECK_NEW( dialog );

    NCursesEvent event;
    dialog->post( &event );

    YDialog::deleteTopmostDialog();

    NCursesEvent newEvent = NCursesEvent::none;

    if ( event == NCursesEvent::button )
    {
	newEvent = NCursesEvent::menu;
	newEvent.selection = event.selection;
    }

    return newEvent;
}


void NCMenuButton::activateItem( YMenuItem * item )
{
    NCursesEvent event = NCursesEvent::menu;
    event.widget = this;
    event.selection = item;
    YNCursesUI::ui()->sendEvent( event );
}
