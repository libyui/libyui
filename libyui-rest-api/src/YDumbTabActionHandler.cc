/*
  Copyright (C) 2021 SUSE LLC

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

#include <boost/algorithm/string.hpp>

#define YUILogComponent "rest-api"
#include <yui/YUILog.h>

#include <yui/YDumbTab.h>
#include <yui/YItem.h>

#include "YDumbTabActionHandler.h"


std::function<void (YDumbTab*)> YDumbTabActionHandler::get_handler( YDumbTab * widget,
                                                                    const std::string &item_label )
{
    return [&] (YDumbTab *tab) {
        YItem * item = findItem( item_label, tab->itemsBegin(), tab->itemsEnd() );
        if ( item )
        {
            yuiMilestone() << "Activating Tree Item \"" << item->label() << '"' << std::endl;
            tab->setKeyboardFocus();
            tab->selectItem( item );
            activate_widget( tab );
        }
        else
        {
            throw YUIException("Item: '" + item_label + "' cannot be found among tabs");
        }
    };
}

YItem * 
YDumbTabActionHandler::findItem( const std::string &item_label, 
                                 YItemConstIterator	begin,
                                 YItemConstIterator	end ) const
{
    std::string item_label_sanitized = normalize_label(item_label);
    for ( YItemConstIterator it = begin; it != end; ++it )
    {
        YItem * item = *it;

        if ( normalize_label(item->label()) == item_label_sanitized )
        {
            return item;
        }
    }

    return nullptr;
}
