/*************************************************************************************************************

 Copyright (C) 2000 - 2010 Novell, Inc.   All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation; either version 2 of the License, or (at your
 option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*************************************************************************************************************/



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

   File:       NCPopupMenu.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPopupMenu.h"

#include "NCTable.h"
#include <yui/YMenuButton.h>


NCPopupMenu::NCPopupMenu( const wpos at, YItemIterator begin, YItemIterator end )
	: NCPopupTable( at )
	, itemsMap()
{
    vector<string> row( 2 );
    createList( row );

    for ( YItemIterator it = begin; it != end; ++it )
    {
	YMenuItem * item = dynamic_cast<YMenuItem *>( *it );
	YUI_CHECK_PTR( item );

	row[0] = item->label();
	row[1] = item->hasChildren() ? "..." : "";

	YTableItem *tableItem = new YTableItem( row[0], row[1] );
	yuiDebug() << "Add to map: TableItem: " << tableItem << " Menu item: " << item << std::endl;

	addItem( tableItem );
	itemsMap[tableItem] = item;
    }

    stripHotkeys();
}


NCPopupMenu::~NCPopupMenu()
{
    itemsMap.clear();
}


NCursesEvent NCPopupMenu::wHandleInput( wint_t ch )
{
    NCursesEvent ret;

    switch ( ch )
    {
	case KEY_RIGHT:
	    {
		yuiDebug() << "CurrentItem: " << getCurrentItem() << std::endl;
		YTableItem * tableItem = dynamic_cast<YTableItem *> ( getCurrentItemPointer() );

		if ( tableItem )
		{
		    YMenuItem * item = itemsMap[ tableItem ];

		    if ( item && item->hasChildren() )
			ret = NCursesEvent::button;
		}

		break;
	    }

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
    yuiDebug() << "Index: " << selection << std::endl;
    YTableItem * tableItem = dynamic_cast<YTableItem *>( getCurrentItemPointer() );

    YMenuItem * item = itemsMap[ tableItem ];

    if ( !item )
	return false;

    yuiMilestone() << "Menu item: " << item->label() << std::endl;

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

	    if ( !again )
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

