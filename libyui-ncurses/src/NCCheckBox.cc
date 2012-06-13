/****************************************************************************

  Copyright (c) 2000 - 2012 Novell, Inc.
  All Rights Reserved.

  This program is free software; you can redistribute it and/or
  modify it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, contact Novell, Inc.

  To contact Novell about this file by physical or electronic mail,
  you may find current contact information at www.novell.com

 ****************************************************************************/



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

   File:       NCCheckBox.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCCheckBox.h"


unsigned char NCCheckBox::statetag[3] = { '?', ' ', 'x' };


NCCheckBox::NCCheckBox( YWidget * parent,
			const string & nlabel,
			bool checked )
	: YCheckBox( parent, nlabel )
	, NCWidget( parent )
	, tristate( false )
	, checkstate( checked ? S_ON : S_OFF )
{
    yuiDebug() << std::endl;
    setLabel( nlabel );
    hotlabel = &label;
}


NCCheckBox::~NCCheckBox()
{
    yuiDebug() << std::endl;
}


int NCCheckBox::preferredWidth()
{
    return wGetDefsze().W;
}


int NCCheckBox::preferredHeight()
{
    return wGetDefsze().H;
}


void NCCheckBox::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YCheckBox::setEnabled( do_bv );
}


void NCCheckBox::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


void NCCheckBox::setLabel( const string & nlabel )
{
    label  = NCstring( nlabel );
    label.stripHotkey();
    defsze = wsze( label.height(), label.width() + 4 );
    YCheckBox::setLabel( nlabel );
    Redraw();
}


void NCCheckBox::setValue( YCheckBoxState state )
{
    switch ( state )
    {
	case YCheckBox_on:
	    checkstate = S_ON;
	    tristate = false;
	    break;

	case YCheckBox_off:
	    checkstate = S_OFF;
	    tristate = false;
	    break;

	case YCheckBox_dont_care:
	    tristate = true;
	    checkstate = S_DC;
	    break;
    }

    Redraw();
}


YCheckBoxState NCCheckBox::value()
{
    if ( checkstate == S_DC )
	return YCheckBox_dont_care;

    if ( checkstate == S_ON )
	return YCheckBox_on;
    else
	return YCheckBox_off;
}


void NCCheckBox::wRedraw()
{
    if ( !win )
	return;

    const NCstyle::StWidget & style( widgetStyle() );

    win->bkgdset( style.plain );

    win->printw( 0, 0, "[ ] " );

    label.drawAt( *win, style, wpos( 0, 4 ) );

    win->bkgdset( style.data );

    win->printw( 0, 1, "%c", statetag[checkstate] );
}


NCursesEvent NCCheckBox::wHandleInput( wint_t key )
{
    NCursesEvent ret;

    switch ( key )
    {
	case KEY_HOTKEY:
	case KEY_SPACE:
	case KEY_RETURN:

	    switch ( checkstate )
	    {
		case S_DC:
		    checkstate = S_ON;
		    break;

		case S_ON:
		    checkstate = S_OFF;
		    break;

		case S_OFF:
		    checkstate = tristate ? S_DC : S_ON;
		    break;
	    }

	    Redraw();

	    if ( notify() )
		ret = NCursesEvent::ValueChanged;

	    break;
    }

    return ret;
}
