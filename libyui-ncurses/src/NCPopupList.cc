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

   File:       NCPopupList.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPopupList.h"
#include <yui/YTableItem.h>


NCPopupList::NCPopupList( const wpos at,
			  const std::string & label,
			  const std::list<std::string> & deflist,
			  int index )
	: NCPopupTable( at )
{
    createEntries( deflist, index );
}


NCPopupList::~NCPopupList()
{

}


void NCPopupList::createEntries( const std::list<std::string> & deflist, int index )
{
    std::vector<std::string> row( 1 );
    createList( row );

    for ( std::list<std::string>::const_iterator entry = deflist.begin();
	  entry != deflist.end(); ++entry )
    {
	YItem *item = new YTableItem(( *entry ) );
	addItem( item );
    }

    setCurrentItem( index );
}


bool NCPopupList::postAgain()
{
    return NCPopupTable::postAgain();
}
