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



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////   __/\\\\\\_____________/\\\__________/\\\________/\\\___/\\\________/\\\___/\\\\\\\\\\\_           ////
 ////    _\////\\\____________\/\\\_________\///\\\____/\\\/___\/\\\_______\/\\\__\/////\\\///__          ////
 ////     ____\/\\\______/\\\__\/\\\___________\///\\\/\\\/_____\/\\\_______\/\\\______\/\\\_____         ////
 ////      ____\/\\\_____\///___\/\\\_____________\///\\\/_______\/\\\_______\/\\\______\/\\\_____        ////
 ////       ____\/\\\______/\\\__\/\\\\\\\\\_________\/\\\________\/\\\_______\/\\\______\/\\\_____       ////
 ////        ____\/\\\_____\/\\\__\/\\\////\\\________\/\\\________\/\\\_______\/\\\______\/\\\_____      ////
 ////         ____\/\\\_____\/\\\__\/\\\__\/\\\________\/\\\________\//\\\______/\\\_______\/\\\_____     ////
 ////          __/\\\\\\\\\__\/\\\__\/\\\\\\\\\_________\/\\\_________\///\\\\\\\\\/_____/\\\\\\\\\\\_    ////
 ////           _\/////////___\///___\/////////__________\///____________\/////////______\///////////__   ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                 widget abstraction library providing Qt, GTK and ncurses frontends                  ////
 ////                                                                                                     ////
 ////                                   3 UIs for the price of one code                                   ////
 ////                                                                                                     ////
 ////                                      ***  NCurses plugin  ***                                       ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                              (C) SUSE Linux GmbH    ////
 ////                                                                                                     ////
 ////                                                              libYUI-AsciiArt (C) 2012 Björn Esser   ////
 ////                                                                                                     ////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-/

   File:       NCPopupTable.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPopupTable.h"

#include "NCTable.h"
#include <yui/YMenuButton.h>


NCPopupTable::NCPopupTable( const wpos at )
	: NCPopup( at, false )
	, sellist( 0 )
{
}


NCPopupTable::~NCPopupTable()
{
}


void NCPopupTable::createList( vector<string> & row )
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
