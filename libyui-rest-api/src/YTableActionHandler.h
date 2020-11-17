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

#ifndef YTableActionHandler_h
#define YTableActionHandler_h

#include <functional>

#include "YTable.h"
#include "YTableItem.h"

#define TablePathDelimiter "|"

class YTableActionHandler
{
public:

    YTableActionHandler() {}
    virtual ~YTableActionHandler() {}

    static std::function<void (YTable*)> get_handler( YTable * widget,
                                                      const std::string &value,
                                                      const int &column_id = 0, //Use first column by default
                                                      const int &row_id = -1 );

protected:
    static YTableItem * table_findItem( std::vector<std::string>::const_iterator path_begin,
                                        std::vector<std::string>::const_iterator path_end,
                                        YItemConstIterator                       begin,
                                        YItemConstIterator                       end,
                                        const int                               &column_id );
};

#endif // YTableActionHandler_h
