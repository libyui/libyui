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


#include <algorithm>    // std::max()
#include <iterator>

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
};


NCMenuBar::NCMenuBar( YWidget* parent ) :
    YMenuBar( parent ), NCWidget( parent ), _selectedMenu( nullptr )
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

    _selectedMenu = nullptr;
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

	_menus.push_back( menu );
	item->setUiItem( menu );

	NClabel label( NCstring( menu->item->label() ) );
	label.stripHotkey();

	width += label.width() + SPACING;
    }

    selectMenu( findNextEnabledMenu( _menus.begin() ) );

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
    wpos at(ScreenPos() + wpos( 1, _selectedMenu->position.C) );

    NCPopupMenu * dialog = new NCPopupMenu(
	at,
	_selectedMenu->item->childrenBegin(),
	_selectedMenu->item->childrenEnd()
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
	    selectMenu( previousMenu() );
	    wRedraw();
	    break;

	case KEY_RIGHT:
	    selectMenu( nextMenu() );
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


void
NCMenuBar::selectMenu( NCMenuBar::MenuIterator menu )
{
    if ( menu != _menus.end() )
	_selectedMenu = *menu;
}


NCMenuBar::MenuIterator
NCMenuBar::currentMenu()
{
    return std::find( _menus.begin(), _menus.end(), _selectedMenu );
}


NCMenuBar::MenuIterator
NCMenuBar::nextMenu()
{
    MenuIterator current = currentMenu();

    if ( current == _menus.end() )
	return findNextEnabledMenu( _menus.begin() );

    MenuIterator next = findNextEnabledMenu( std::next( current, 1 ) );

    if ( next == _menus.end() )
	return findNextEnabledMenu( _menus.begin() );

    return next;
}


NCMenuBar::MenuIterator
NCMenuBar::previousMenu()
{
    MenuIterator current = currentMenu();

    ReverseMenuIterator rbegin;

    if ( current == _menus.end() )
	rbegin = _menus.rbegin();
    else
	rbegin = ReverseMenuIterator( current );

    ReverseMenuIterator previous = findPreviousEnabledMenu( rbegin );

    if ( previous == _menus.rend() && rbegin != _menus.rbegin() )
	previous = findPreviousEnabledMenu( _menus.rbegin() );

    if ( previous == _menus.rend() )
	return _menus.end();

    return find( _menus.begin(), _menus.end(), *previous );
}


NCMenuBar::MenuIterator
NCMenuBar::findNextEnabledMenu( MenuIterator begin )
{
    return find_if( begin, _menus.end(), [](const Menu * menu ) {
	return menu->item->isEnabled();
    });
}


NCMenuBar::ReverseMenuIterator
NCMenuBar::findPreviousEnabledMenu( ReverseMenuIterator rbegin )
{
    return find_if( rbegin, _menus.rend(), [](const Menu * menu ) {
	return menu->item->isEnabled();
    });
}


const NCstyle::StWidget &
NCMenuBar::menuStyle( const Menu * menu ) const
{
    if ( !menu->item->isEnabled() )
	return wStyle().getWidget( NC::WSdisabled );

    bool non_active = ( menu != _selectedMenu );
    return widgetStyle( non_active );
}
