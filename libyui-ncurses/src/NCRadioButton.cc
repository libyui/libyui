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

   File:       NCRadioButton.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCRadioButton.h"
#include "NCRadioButtonGroup.h"
#include "YNCursesUI.h"


NCRadioButton::NCRadioButton( YWidget * parent,
			      const std::string & nlabel,
			      bool check )
	: YRadioButton( parent, nlabel )
	, NCWidget( parent )
	, checked( false )
{
    // yuiDebug() << std::endl;
    setLabel( nlabel );
    hotlabel = &label;
    setValue( check );
}


NCRadioButton::~NCRadioButton()
{
    // yuiDebug() << std::endl;
}


int NCRadioButton::preferredWidth()
{
    return wGetDefsze().W;
}


int NCRadioButton::preferredHeight()
{
    return wGetDefsze().H;
}


void NCRadioButton::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YRadioButton::setEnabled( do_bv );
}


void NCRadioButton::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


void NCRadioButton::setLabel( const std::string & nlabel )
{
    label  = NCstring( nlabel );
    label.stripHotkey();
    defsze = wsze( label.height(), label.width() + 4 );
    YRadioButton::setLabel( nlabel );
    Redraw();
}


void NCRadioButton::setValue( bool newval )
{
    if ( newval != checked )
    {
	checked = newval;

	if ( checked && buttonGroup() )
	{
	    buttonGroup()->uncheckOtherButtons( this );
	}

	Redraw();

	if (notify())
	{
	    NCursesEvent event = NCursesEvent::ValueChanged;
	    event.widget = this;
	    YNCursesUI::ui()->sendEvent(event);
	}
    }
}


void NCRadioButton::wRedraw()
{
    if ( !win )
	return;

    const NCstyle::StWidget & style( widgetStyle() );

    win->bkgdset( style.plain );

    win->printw( 0, 0, "( ) " );

    label.drawAt( *win, style, wpos( 0, 4 ) );

    win->bkgdset( style.data );

    win->printw( 0, 1, "%c", ( checked ? 'x' : ' ' ) );
}


NCursesEvent NCRadioButton::wHandleInput( wint_t key )
{
    NCursesEvent ret;
    bool oldChecked = checked;
    NCRadioButtonGroup * group;

    switch ( key )
    {
	case KEY_HOTKEY:
	case KEY_SPACE:
	case KEY_RETURN:
	    setValue( true );

	    if ( notify() && oldChecked != checked )
		ret = NCursesEvent::ValueChanged;

	    break;

	case KEY_UP:
	    group = dynamic_cast<NCRadioButtonGroup *>( buttonGroup() );

	    if ( group )
		group->focusPrevButton();

	    break;

	case KEY_DOWN:
	    group = dynamic_cast<NCRadioButtonGroup *>( buttonGroup() );

	    if ( group )
		group->focusNextButton();

	    break;
    }

    return ret;
}

