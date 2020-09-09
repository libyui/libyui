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

   File:       NCPopupMenu.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPopupMenu.h"
#include "NCTable.h"


// Helper class that represents a menu item
struct NCPopupMenu::Item
{
    YTableItem * tableItem;
    YMenuItem * menuItem;


    // Whether the item can be selected
    bool isSelectable() const
    {
	if ( ! menuItem )
	    return false;

	return menuItem->isEnabled() && !menuItem->isSeparator();
    }

};


NCPopupMenu::NCPopupMenu( const wpos & at, YItemIterator begin, YItemIterator end )
	: NCPopupTable( at )
	, _items()
{
    std::vector<std::string> row( 2 );
    createList( row );

    for ( YItemIterator it = begin; it != end; ++it )
    {
	YMenuItem * menuItem = dynamic_cast<YMenuItem *>( *it );
	YUI_CHECK_PTR( menuItem );

	row[0] = menuItem->label();
	row[1] = menuItem->hasChildren() ? "..." : "";

	YTableItem *tableItem = new YTableItem( row[0], row[1] );
	// yuiDebug() << "Add to std::map: TableItem: " << tableItem << " Menu item: " << item << std::endl;

	NCTableLine::STATE state = menuItem->isEnabled() ? NCTableLine::S_NORMAL : NCTableLine::S_DISABELED;

	addItem( tableItem, state );

	Item * item = new Item();
	item->tableItem = tableItem;
	item->menuItem = menuItem;

	_items.add( item );
    }

    selectNextItem();

    stripHotkeys();
}


NCPopupMenu::~NCPopupMenu()
{
    for (Item * item : _items)
	delete item;
}


NCursesEvent NCPopupMenu::wHandleInput( wint_t ch )
{
    NCursesEvent event;

    switch ( ch )
    {
	case KEY_RIGHT:
	{
	    Item * item = selectedItem();

	    if ( item )
	    {
		if ( item->menuItem->hasChildren() )
		    event = NCursesEvent::button;
		else
		{
		    event = NCursesEvent::key;
		    event.keySymbol = "CursorRight";
		}
	    }

	    break;
	}

	case KEY_LEFT:
	    event = NCursesEvent::key;

	    event.keySymbol = "CursorLeft";
	    event.detail = NCursesEvent::CONTINUE;

	    break;

	case KEY_DOWN:
	    selectNextItem();
	    break;

	case KEY_UP:
	    selectPreviousItem();
	    break;

	case KEY_BACKSPACE:
	    event = NCursesEvent::key;
	    event.keySymbol = "BackSpace";
	    break;

	default:
	    event = NCPopup::wHandleInput( ch );
	    break;
    }

    return event;
}


bool NCPopupMenu::postAgain()
{
    // dont mess up postevent.detail here
    bool again = false;
    int  selection = ( postevent == NCursesEvent::button ) ? getCurrentItem() : -1;

    Item * item = selectedItem();

    if ( ! item )
	return false;

    yuiDebug() << "Menu item: " << item->menuItem->label() << std::endl;

    if ( selection != -1 )
    {
	if ( item->menuItem->hasChildren() )
	{
	    // post submenu
	    wpos at( ScreenPos() + wpos( selection, inparent.Sze.W - 1 ) );
	    NCPopupMenu * dialog = new NCPopupMenu( at,
						    item->menuItem->childrenBegin(),
						    item->menuItem->childrenEnd() );
	    YUI_CHECK_NEW( dialog );

	    again = ( dialog->post( &postevent ) == NCursesEvent::CONTINUE );

	    YDialog::deleteTopmostDialog();
	}
	else
	{
	    // store selection
	    postevent.selection = item->menuItem;
	}
    }

    return again;
}


NCPopupMenu::Item * NCPopupMenu::selectedItem()
{
    updateSelectedItem();

    return *_items.current();
}


void NCPopupMenu::selectNextItem()
{
    selectItem( _items.next() );
}


void NCPopupMenu::selectPreviousItem()
{
    selectItem( _items.previous() );
}


void NCPopupMenu::updateSelectedItem()
{
     YTableItem * tableItem = dynamic_cast<YTableItem *> ( getCurrentItemPointer() );

    if ( ! tableItem )
	return;

    CyclicContainer<Item>::Iterator newCurrent = findItem( tableItem);

    if ( newCurrent == _items.end() )
	return;

    if ( _items.current() != newCurrent )
	selectItem( newCurrent );
}


CyclicContainer<NCPopupMenu::Item>::Iterator NCPopupMenu::findItem( YTableItem * tableItem )
{
    return find_if( _items.begin(), _items.end(), [tableItem](Item * item) {
	return item->tableItem == tableItem;
    });
}


void NCPopupMenu::selectItem( CyclicContainer<Item>::Iterator item )
{
    _items.setCurrent( item );

    if ( item != _items.end() )
    {
	int index = std::distance(_items.begin(), item);

	setCurrentItem(index);
    }
}
