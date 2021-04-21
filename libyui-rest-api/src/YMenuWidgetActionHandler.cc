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

#include <yui/YMenuItem.h>
#include "YMenuWidgetActionHandler.h"

YMenuItem* 
YMenuWidgetActionHandler::findItem( std::vector<std::string>::iterator path_begin,
                                    std::vector<std::string>::iterator path_end,
                                    YItemConstIterator                 begin,
                                    YItemConstIterator                 end ) const
{
    for ( YItemConstIterator it = begin; it != end; ++it )
    {
        YMenuItem * item = dynamic_cast<YMenuItem *>(*it);

        if ( !item )
            return nullptr;

        if ( boost::erase_all_copy( item->label(), ShortcutChar ) == *path_begin )
        {
            if ( std::next( path_begin ) == path_end )
            {
                // Only return items which can trigger an action.
                // Intermediate items only open a submenu, so continue looking.
                if ( item->hasChildren() )
                    continue;

                return item;
            }
            YMenuItem * result = findItem( ++path_begin, path_end,
                                           item->childrenBegin(), item->childrenEnd() );
            if ( result )
                return result;
        }
    }
    return nullptr;
}
