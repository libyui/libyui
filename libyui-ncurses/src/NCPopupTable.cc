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

   File:       NCPopupTable.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPopupTable.h"

#include "NCTable.h"
#include <yui/YMenuButton.h>


NCPopupTable::NCPopupTable( const wpos & at )
	: NCPopup( at, false )
	, sellist( 0 )
{
}


NCPopupTable::~NCPopupTable()
{
}


void NCPopupTable::createList( std::vector<std::string> & row )
{
    if ( sellist )
	return ;

    YTableHeader * tableHeader = new YTableHeader();

    // sellist =  new NCTable( this, row.size() );
    sellist = new NCTable( this, tableHeader );

    YUI_CHECK_NEW( sellist );

    sellist->setBigList( true );
    sellist->SetSepChar( ' ' );
    sellist->SetSepWidth( 0 );
    sellist->SetHotCol( 0 );
    sellist->setNotify( true );
}


void NCPopupTable::addItem( YItem *yitem )
{
    if ( !yitem )
	return;

    sellist->addItem( yitem );

    // Calling sellist->addItem() resets the hotcol because
    // NCTableStyle's constructor sets hotcol to -1.
    // Set hot coll again:
    sellist->SetHotCol( 0 );
}


void NCPopupTable::setCurrentItem( int index )
{
    if ( !sellist )
	return;

    sellist->setCurrentItem( index );
}


int NCPopupTable::getCurrentItem() const
{
    if ( !sellist )
	return -1;

    return sellist->getCurrentItem();
}


YItem * NCPopupTable::getCurrentItemPointer( ) const
{
    if ( !sellist )
	return 0;

    return sellist->getCurrentItemPointer( );
}


NCursesEvent NCPopupTable::wHandleHotkey( wint_t key )
{
    if ( key >= 0 && sellist->setItemByKey( key ) )
	return wHandleInput( KEY_RETURN );

    return NCursesEvent::none;
}


bool NCPopupTable::postAgain()
{
    if ( sellist )
    {
	postevent.detail = ( postevent == NCursesEvent::button ) ?
	    sellist->getCurrentItem() : NCursesEvent::NODETAIL;
    }

    return false;
}


void NCPopupTable::stripHotkeys()
{
    if ( sellist )
    {
	sellist->stripHotkeys();
    }
}
