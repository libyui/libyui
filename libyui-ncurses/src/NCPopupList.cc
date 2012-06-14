/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
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
			  const string & label,
			  const list<string> & deflist,
			  int index )
	: NCPopupTable( at )
{
    createEntries( deflist, index );
}


NCPopupList::~NCPopupList()
{

}


void NCPopupList::createEntries( const list<string> & deflist, int index )
{
    vector<string> row( 1 );
    createList( row );

    for ( list<string>::const_iterator entry = deflist.begin();
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
