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

   File:       NCCheckBox.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCCheckBox.h"
#include "YNCursesUI.h"


unsigned char NCCheckBox::statetag[3] = { '?', ' ', 'x' };


NCCheckBox::NCCheckBox( YWidget * parent,
			const std::string & nlabel,
			bool checked )
	: YCheckBox( parent, nlabel )
	, NCWidget( parent )
	, tristate( false )
	, checkstate( checked ? S_ON : S_OFF )
{
    // yuiDebug() << std::endl;
    setLabel( nlabel );
    hotlabel = &label;
}


NCCheckBox::~NCCheckBox()
{
    // yuiDebug() << std::endl;
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


void NCCheckBox::setLabel( const std::string & nlabel )
{
    label  = NCstring( nlabel );
    label.stripHotkey();
    defsze = wsze( label.height(), label.width() + 4 );
    YCheckBox::setLabel( nlabel );
    Redraw();
}


void NCCheckBox::setValue( YCheckBoxState state )
{
    YCheckBoxState old = value();

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

    // trigger the notify event if enabled
    if (old != state && notify())
    {
        NCursesEvent event = NCursesEvent::ValueChanged;
        event.widget = this;
        YNCursesUI::ui()->sendEvent(event);
    }
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
