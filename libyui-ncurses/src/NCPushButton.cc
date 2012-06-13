/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
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
  |****************************************************************************
*/



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

   File:       NCPushButton.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include <yui/YDialog.h>
#include "NCDialog.h"
#include "NCurses.h"
#include "NCPushButton.h"


NCPushButton::NCPushButton( YWidget * parent, const string & nlabel )
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


void NCPushButton::setLabel( const string & nlabel )
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
