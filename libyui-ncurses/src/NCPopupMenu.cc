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

#include <algorithm>
#include <iterator>

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPopupMenu.h"
#include "NCTable.h"


struct NCPopupMenu::Item
{
    YTableItem * tableItem;
    YMenuItem * menuItem;
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

	// TODO
	// if (menuItem->isSeparator())
	//     row[0] = "---";

	YTableItem *tableItem = new YTableItem( row[0], row[1] );
	// yuiDebug() << "Add to std::map: TableItem: " << tableItem << " Menu item: " << item << std::endl;

	NCTableLine::STATE state = menuItem->isEnabled() ? NCTableLine::S_NORMAL : NCTableLine::S_DISABELED;

	addItem( tableItem, state );

	Item * item = new Item();
	item->tableItem = tableItem;
	item->menuItem = menuItem;

	_items.push_back( item );
    }

    selectItem( findNextEnabledItem( _items.begin() ) );

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
	    ItemIterator current = currentItem();

	    if (current != _items.end())
	    {
		YMenuItem * item = (*current)->menuItem;

		if ( item->hasChildren() )
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
	    selectItem( nextItem() );
	    break;

	case KEY_UP:
	    selectItem( previousItem() );
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

    ItemIterator current = currentItem();

    if ( current == _items.end() )
	return false;

    YMenuItem * item = (*current)->menuItem;

    yuiDebug() << "Menu item: " << item->label() << std::endl;

    if ( selection != -1 )
    {
	if ( item->hasChildren() )
	{
	    // post submenu
	    wpos at( ScreenPos() + wpos( selection, inparent.Sze.W - 1 ) );
	    NCPopupMenu * dialog = new NCPopupMenu( at,
						    item->childrenBegin(),
						    item->childrenEnd() );
	    YUI_CHECK_NEW( dialog );

	    again = ( dialog->post( &postevent ) == NCursesEvent::CONTINUE );

	    YDialog::deleteTopmostDialog();
	}
	else
	{
	    // store selection
	    //postevent.detail = menu.itemList()[selection]->getIndex();
	    postevent.detail = item->index();
	}
    }

    return again;
}


NCPopupMenu::ItemIterator
NCPopupMenu::findItem( YTableItem* tableItem )
{
    return find_if(_items.begin(), _items.end(), [tableItem](const Item * item) {
	return item->tableItem == tableItem;
    });
}


void NCPopupMenu::selectItem( ItemIterator item )
{
    if ( item != _items.end() )
    {
	int index = std::distance(_items.begin(), item);

	setCurrentItem(index);
    }
}


NCPopupMenu::ItemIterator NCPopupMenu::currentItem()
{
    ItemIterator current = _items.end();

    YTableItem * tableItem = dynamic_cast<YTableItem *> ( getCurrentItemPointer() );

    if ( tableItem )
	current = findItem(tableItem);

    return current;
}


NCPopupMenu::ItemIterator NCPopupMenu::nextItem()
{
    ItemIterator current = currentItem();

    if ( current == _items.end() )
	return findNextEnabledItem( _items.begin() );

    ItemIterator next = findNextEnabledItem( std::next( current, 1 ) );

    if ( next == _items.end() )
	return findNextEnabledItem( _items.begin() );

    return next;
}


NCPopupMenu::ItemIterator NCPopupMenu::previousItem()
{
    ItemIterator current = currentItem();

    ReverseItemIterator rbegin;

    if ( current == _items.end() )
	rbegin = _items.rbegin();
    else
	rbegin = ReverseItemIterator( current );

    ReverseItemIterator previous = findPreviousEnabledItem( rbegin );

    if ( previous == _items.rend() && rbegin != _items.rbegin() )
	previous = findPreviousEnabledItem( _items.rbegin() );

    if ( previous == _items.rend() )
	return _items.end();

    return find( _items.begin(), _items.end(), *previous );
}


NCPopupMenu::ItemIterator NCPopupMenu::findNextEnabledItem( ItemIterator begin )
{
    return find_if( begin, _items.end(), [](const Item * item) {
	return item->menuItem->isEnabled() && !item->menuItem->isSeparator();
    });
}


NCPopupMenu::ReverseItemIterator NCPopupMenu::findPreviousEnabledItem( ReverseItemIterator rbegin )
{
    return find_if( rbegin, _items.rend(), [](const Item * item) {
	return item->menuItem->isEnabled() && !item->menuItem->isSeparator();
    });
}
