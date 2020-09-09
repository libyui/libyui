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

   File:       NCPopupMenu.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCPopupMenu_h
#define NCPopupMenu_h

#include "NCPopupTable.h"
#include "CyclicContainer.h"


class NCPopupMenu : public NCPopupTable
{
private:

    struct Item;

    NCPopupMenu & operator=( const NCPopupMenu & );
    NCPopupMenu( const NCPopupMenu & );

    Item * selectedItem();

    void selectNextItem();
    void selectPreviousItem();
    void updateSelectedItem();

    CyclicContainer<Item>::Iterator findItem( YTableItem * tableItem );

    void selectItem( CyclicContainer<Item>::Iterator item );

    /** Container of menu items
     * It allows cyclic navigation between the items.
     * Note that this container holds pointers to items, but it does not own the pointers. The pointers
     * are owned by the NCPopupMenu object.
     **/
    CyclicContainer<Item> _items;

protected:

    virtual NCursesEvent wHandleInput( wint_t ch );
    virtual NCursesEvent wHandleHotkey( wint_t key );

    virtual bool postAgain();

public:

    NCPopupMenu( const wpos & at,
		 YItemIterator begin,
		 YItemIterator end );

    virtual ~NCPopupMenu();

};

#endif // NCPopupMenu_h
