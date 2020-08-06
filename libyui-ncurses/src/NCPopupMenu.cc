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
    YTableItem * table_item;
    YMenuItem * menu_item;
};


NCPopupMenu::NCPopupMenu( const wpos & at, YItemIterator begin, YItemIterator end )
	: NCPopupTable( at )
	, _items()
{
    std::vector<std::string> row( 2 );
    createList( row );

    for ( YItemIterator it = begin; it != end; ++it )
    {
	YMenuItem * item = dynamic_cast<YMenuItem *>( *it );
	YUI_CHECK_PTR( item );

	row[0] = item->label();
	row[1] = item->hasChildren() ? "..." : "";

	// if (item->isSeparator())
	//     row[0] = "---";

	YTableItem *tableItem = new YTableItem( row[0], row[1] );
	// yuiDebug() << "Add to std::map: TableItem: " << tableItem << " Menu item: " << item << std::endl;

	NCTableLine::STATE state = item->isEnabled() ? NCTableLine::S_NORMAL : NCTableLine::S_DISABELED;

	addItem( tableItem, state );

	_items.push_back(new Item({tableItem, item}));
    }

    stripHotkeys();
}


NCPopupMenu::~NCPopupMenu()
{
    for (auto item : _items)
	delete item;
}


NCursesEvent NCPopupMenu::wHandleInput( wint_t ch )
{
    NCursesEvent ret;

    switch ( ch )
    {
	case KEY_RIGHT:
	    {
		// yuiDebug() << "CurrentItem: " << getCurrentItem() << std::endl;
		YTableItem * tableItem = dynamic_cast<YTableItem *> ( getCurrentItemPointer() );

		if ( tableItem )
		{
		    auto current = find_item(tableItem);

		    if (current != _items.end())
		    {
			YMenuItem * item = (*current)->menu_item;

			if ( item && item->hasChildren() && item->isEnabled() )
			    ret = NCursesEvent::button;
		    }
		}

		break;
	    }

	case KEY_DOWN:
	{
	    ret = select_next_item();
	    break;
	}
	case KEY_UP:
	    ret = select_previous_item();
	    break;

	case KEY_LEFT:
	    ret = NCursesEvent::cancel;
	    ret.detail = NCursesEvent::CONTINUE;
	    break;

	default:
	    ret = NCPopup::wHandleInput( ch );
	    break;
    }

    return ret;
}


bool NCPopupMenu::postAgain()
{
    // dont mess up postevent.detail here
    bool again = false;
    int  selection = ( postevent == NCursesEvent::button ) ? getCurrentItem()
		     : -1;
    // yuiDebug() << "Index: " << selection << std::endl;
    YTableItem * tableItem = dynamic_cast<YTableItem *>( getCurrentItemPointer() );

    auto current = find_item(tableItem);

    if ( current == _items.end() )
	return false;

    YMenuItem * item = (*current)->menu_item;

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


NCursesEvent NCPopupMenu::select_next_item()
{
    NCursesEvent event;

    YTableItem * tableItem = dynamic_cast<YTableItem *> ( getCurrentItemPointer() );

    if ( !tableItem)
	return event;

    auto current = find_item(tableItem);

    if ( current == _items.end() )
	return event;

    auto begin = std::next(current, 1);

    auto next_enabled = find_if(begin, _items.end(), [](const Item * item) {
	return item->menu_item->isEnabled() && !item->menu_item->isSeparator();
    });

    if (next_enabled == _items.end())
	return event;

    int steps_to_next = std::distance(current, next_enabled);

    for (int i = 0; i < steps_to_next; i++)
	event = NCPopup::wHandleInput( KEY_DOWN );

    return event;
}


NCursesEvent NCPopupMenu::select_previous_item()
{
    NCursesEvent event;

    YTableItem * tableItem = dynamic_cast<YTableItem *> ( getCurrentItemPointer() );

    if ( !tableItem )
	return event;

    auto current = find_item(tableItem);

    if ( current == _items.end() || current == _items.begin() )
	return event;

    std::reverse_iterator<std::vector<Item *>::iterator> rbegin(current);

    auto previous_enabled = find_if(rbegin, _items.rend(), [](const Item * item) {
	return item->menu_item->isEnabled() && !item->menu_item->isSeparator();
    });

    if (previous_enabled == _items.rend())
	return event;

    auto previous = find_item((*previous_enabled)->table_item);

    int steps_to_previous = std::distance(previous, current);

    for (int i = 0; i < steps_to_previous; i++)
	event = NCPopup::wHandleInput( KEY_UP );

    return event;
}


std::vector<NCPopupMenu::Item *>::iterator
NCPopupMenu::find_item(YTableItem* table_item)
{
    auto it = find_if(_items.begin(), _items.end(), [table_item](const Item * item) {
	return item->table_item == table_item;
    });

    return it;
}
