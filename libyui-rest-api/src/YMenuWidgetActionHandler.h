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

#ifndef YMenuWidgetActionHandler_h
#define YMenuWidgetActionHandler_h

#include "YWidgetActionHandler.h"


class YMenuWidgetActionHandler : public YWidgetActionHandler
{
public:

    YMenuWidgetActionHandler() {};
    virtual ~YMenuWidgetActionHandler() {};

    template<typename T>
    std::function<void (T*)> get_handler( T *widget, const std::string &value ) {
        return [&] ( T *menu_selector ) {
            std::string value_sanitized = normalize_label( value );
            // Vector of string to store path to the tree item
            std::vector<std::string> path;
            boost::split( path, value_sanitized, boost::is_any_of( TreePathDelimiter ) );
            YMenuItem * item = findItem( path.begin(), path.end(), widget->itemsBegin(), widget->itemsEnd() );
            if ( item )
            {
                menu_selector->setKeyboardFocus();
                activate_widget( menu_selector, item );
            }
            else
            {
                throw YUIException( "Item with path: '" + value + "' cannot be found in the menu selector widget" );
            }
        };
    }

private:

  YMenuItem * findItem( std::vector<std::string>::iterator path_begin,
                        std::vector<std::string>::iterator path_end,
                        YItemConstIterator                 begin,
                        YItemConstIterator                 end ) const;
};

#endif // YMenuWidgetActionHandler_h
