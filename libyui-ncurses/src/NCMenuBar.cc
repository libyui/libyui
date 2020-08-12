/*
  Copyright (C) 2020 SUSE LLC
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

   File:       NCMenuBar.cc

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define	 YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCMenuBar.h"
#include "YNCursesUI.h"
#include "NCPopupMenu.h"


// Margin to the left of the first toplevel item
#define LEFT_MARGIN     0

// Spacing between toplevel items
#define SPACING         2


struct NCMenuBar::Menu {
    wpos position;
    YMenuItem* item;

    bool isSelectable() const
    {
	if ( ! item )
	    return false;

	return item->isEnabled();
    }
};


NCMenuBar::NCMenuBar( YWidget* parent ) :
    YMenuBar( parent ), NCWidget( parent ), _menus()
{
    defsze = wsze( 1, 10 );
}


NCMenuBar::~NCMenuBar()
{
    clear();
}


void
NCMenuBar::clear()
{
    for ( Menu* menu : _menus )
	delete menu;

    _menus.clear();

    defsze = wsze( 1, 10 );
}


void
NCMenuBar::rebuildMenuTree()
{
    clear();

    int width = LEFT_MARGIN;

    for ( YItemIterator it = itemsBegin(); it != itemsEnd(); ++it )
    {
	YMenuItem * item = dynamic_cast<YMenuItem *>(*it);
	YUI_CHECK_PTR( item );

	if ( ! item->isMenu() )
	    YUI_THROW( YUIException( "NCMenuBar: Only menus allowed on toplevel. ") );

	Menu * menu = new Menu();
	menu->item = item;
	menu->position = wpos( 0, width );

	_menus.add( menu );
	item->setUiItem( menu );

	NClabel label( NCstring( menu->item->label() ) );
	label.stripHotkey();

	width += label.width() + SPACING;
    }

    selectNextMenu();

    defsze = wsze( 1, width );
}


void
NCMenuBar::wRedraw()
{
    if ( !win )
	return;

    for ( const Menu * menu : _menus )
    {
	const NCstyle::StWidget & style = menuStyle( menu );

	win->bkgdset( style.plain );

	NClabel label( NCstring( menu->item->label() ) );
	label.stripHotkey();

	label.drawAt( *win, style,  menu->position, wsze( -1, label.width() + SPACING ), NC::LEFT );
    }

    if ( defsze.W < win->width() ) {
	const NCstyle::StWidget & bkg_style( widgetStyle( true ) );

	win->move( 0, defsze.W );
	win->bkgdset( bkg_style.plain );
	win->clrtoeol();
    }
}


NCursesEvent
NCMenuBar::postMenu()
{
    wpos at(ScreenPos() + wpos( 1, selectedMenu()->position.C) );

    NCPopupMenu * dialog = new NCPopupMenu(
	at,
	selectedMenu()->item->childrenBegin(),
	selectedMenu()->item->childrenEnd()
    );

    YUI_CHECK_NEW( dialog );

    NCursesEvent event;
    int selectedIndex = dialog->post( &event );

    YDialog::deleteTopmostDialog();

    return handlePostMenu( event, selectedIndex );
}


void
NCMenuBar::setItemEnabled( YMenuItem * item, bool enabled )
{
    YMenuWidget::setItemEnabled( item, enabled );
    rebuildMenuTree();
    wRedraw();
}


void
NCMenuBar::activateItem( YMenuItem * item )
{
    if ( item->isMenu() || item->isSeparator() )
	return;

    NCursesEvent event = NCursesEvent::menu;

    event.widget = this;
    event.selection = item;

    YNCursesUI::ui()->sendEvent( event );
}


NCursesEvent
NCMenuBar::wHandleInput( wint_t key )
{
    NCursesEvent event = NCursesEvent::none;

    switch ( key )
    {
	case KEY_LEFT:
	    selectPreviousMenu();
	    wRedraw();
	    break;

	case KEY_RIGHT:
	    selectNextMenu();
	    wRedraw();
	    break;

	case KEY_DOWN:
	    event = postMenu();
	    break;

	default:
	    event = NCWidget::wHandleInput(key);
	    break;
    }

    return event;
}


int
NCMenuBar::preferredWidth()
{
    return defsze.W;
}


int
NCMenuBar::preferredHeight()
{
    return 1;
}


void
NCMenuBar::setSize( int newWidth, int newHeight )
{
    wRelocate( wpos( 0 ), wsze( newHeight, newWidth ) );
}


void
NCMenuBar::setEnabled( bool enabled )
{
    NCWidget::setEnabled( enabled );
    YMenuBar::setEnabled( enabled );
}


bool
NCMenuBar::setKeyboardFocus()
{
    if ( !grabFocus() )
	return YWidget::setKeyboardFocus();

    return true;
}


bool
NCMenuBar::HasHotkey( int key )
{
    // TODO
    return false;
}


NCursesEvent
NCMenuBar::wHandleHotkey( wint_t key )
{
    // TODO
    NCursesEvent event = NCursesEvent::none;

    return event;
}


NCursesEvent
NCMenuBar::handlePostMenu( const NCursesEvent & event, int selectedIndex )
{
    NCursesEvent new_event = NCursesEvent::none;

    if ( event == NCursesEvent::button && selectedIndex >= 0 )
    {
	YMenuItem * item = findMenuItem( selectedIndex );

	if ( item && item->isEnabled() )
	{
	    new_event = NCursesEvent::menu;
	    new_event.selection = item;
	}
    }
    else if ( event == NCursesEvent::key )
    {
	if ( event.keySymbol == "CursorLeft" )
	{
	    wHandleInput( KEY_LEFT );
	    new_event = wHandleInput( KEY_DOWN );
	}
	else if ( event.keySymbol == "CursorRight" )
	{
	    wHandleInput( KEY_RIGHT );
	    new_event = wHandleInput( KEY_DOWN );
	}
    }

    return new_event;
}


NCMenuBar::Menu *
NCMenuBar::selectedMenu()
{
    return *_menus.current();
}


void
NCMenuBar::selectNextMenu()
{
    _menus.setCurrent( _menus.next() );
}


void
NCMenuBar::selectPreviousMenu()
{
    _menus.setCurrent( _menus.previous() );
}


const NCstyle::StWidget &
NCMenuBar::menuStyle( const Menu * menu )
{
    if ( !menu->item->isEnabled() )
	return wStyle().getWidget( NC::WSdisabled );

    bool non_active = ( menu != selectedMenu() );
    return widgetStyle( non_active );
}
