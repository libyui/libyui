/*
  Copyright (C) 2020 SUSE LLC

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

#include "YTable.h"
#include "YTableItem.h"

#include <boost/algorithm/string.hpp>
#include <vector>

#include "YTableActionHandler.h"

#define YUILogComponent "rest-api"
#include "YUILog.h"

std::function<void (YTable*)> YTableActionHandler::get_handler( YTable * widget,
                                                                const std::string &value,
                                                                const int &column_id,
                                                                const int &row_id )
{
    // Select a row in the table by the row number
    if ( row_id != -1 )
    {
        return [&] (YTable *tb) {
            if( row_id >= tb->itemsCount() || row_id < 0 )
                throw YUIException( "Table: '" + tb->label() + "' does NOT contain row #" + std::to_string( row_id ) );

            if ( YItem * item = tb->itemAt(row_id) )
            {
                    yuiMilestone() << "Activating Table \"" << tb->label() << '"' << std::endl;
                    tb->setKeyboardFocus();
                    tb->selectItem( item );
            }
            else
            {
                throw YUIException( "Row: '" + std::to_string( row_id ) + "' cannot be found in the table" );
            }
        };
    }

    // Do the search of the value in the given column
    return [&] (YTable *tb) {
        std::vector<std::string> path;
        boost::split( path, value, boost::is_any_of( TablePathDelimiter ) );
        auto * item = YTableActionHandler::table_findItem( path.begin(),
                                                           path.end(),
                                                           tb->itemsBegin(),
                                                           tb->itemsEnd(),
                                                           column_id );
        if ( item )
        {
                yuiMilestone() << "Activating Table \"" << tb->label() << "\" Item: \"" << item->label( column_id ) << "\"" << std::endl;
                tb->setKeyboardFocus();
                tb->selectItem( item );
        }
        else
        {
            throw YUIException( "Item: '" + value + "' cannot be found in the table" );
        }
    };
}


YTableItem *
YTableActionHandler::table_findItem( std::vector<std::string>::const_iterator path_begin,
                                     std::vector<std::string>::const_iterator path_end,
                                     YItemConstIterator                       begin,
                                     YItemConstIterator                       end,
                                     const int                               &column_id )
{
    for ( YItemConstIterator it = begin; it != end; ++it )
    {
        YTableItem * item = dynamic_cast<YTableItem *>( *it );

        if ( ! item )
            return nullptr;

        if ( item->label(column_id) == *path_begin )
        {
            if ( std::next( path_begin ) == path_end )
                return item;

            // Recursively search child items
            YTableItem * result = YTableActionHandler::table_findItem( ++path_begin,
                                                                       path_end,
                                                                       item->childrenBegin(),
                                                                       item->childrenEnd(),
                                                                       column_id );

            if ( result )
                return result;
        }
    }

    return nullptr;
}
