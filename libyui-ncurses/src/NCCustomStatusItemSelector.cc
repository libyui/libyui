/*
  Copyright (C) 2019 SUSE LLC
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

   File:       NCCustomStatusItemSelector.cc

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define	 YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCCustomStatusItemSelector.h"

using std::string;


NCCustomStatusItemSelector::NCCustomStatusItemSelector( YWidget * parent,
                                                        const YItemCustomStatusVector & customStates )
    : NCItemSelectorBase( parent, customStates )
{
    yuiDebug() << endl;
}


NCCustomStatusItemSelector::~NCCustomStatusItemSelector()
{
    yuiDebug() << endl;
}


NCTableTag * NCCustomStatusItemSelector::createTagCell( YItem * item )
{
    // FIXME: TO DO
    // FIXME: TO DO
    // FIXME: TO DO

    NCTableTag * tag = new NCTableTag( item, item->selected(), enforceSingleSelection() );
    YUI_CHECK_NEW( tag );

    return tag;
}


NCTableTag * NCCustomStatusItemSelector::tagCell( int index ) const
{
    // FIXME: TO DO
    // FIXME: TO DO
    // FIXME: TO DO
    
    return NCItemSelectorBase::tagCell( index );
}


void NCCustomStatusItemSelector::cycleCurrentItemStatus()
{
    YItem *item = currentItem();

    if ( item )
    {
        yuiMilestone() << "Cycling status of item " << item->label() << endl;
    }
}


NCursesEvent NCCustomStatusItemSelector::wHandleInput( wint_t key )
{
    NCursesEvent ret;
    bool valueChanged = false;

    if ( ! handleInput( key ) )
    {
	YItem *curItem = currentItem();

	switch ( key )
	{
	    case KEY_SPACE:
	    case KEY_RETURN:

                if ( ! curItem )
                    curItem = scrollUpToPreviousItem();

                if ( curItem )
                {
                    cycleCurrentItemStatus();
                    valueChanged = true;
                }
                
		break;
	}
    }

    if ( notify() )
    {
        // FIXME TO DO: Send YMenuEvent
        
	if ( valueChanged )
	    ret = NCursesEvent::ValueChanged;
    }

    return ret;
}

