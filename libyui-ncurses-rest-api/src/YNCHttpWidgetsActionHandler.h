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

#ifndef YNCHttpWidgetsActionHandler_h
#define YNCHttpWidgetsActionHandler_h

#include <yui/rest-api/YHttpWidgetsActionHandler.h>


class YNCHttpWidgetsActionHandler : public YHttpWidgetsActionHandler
{

public:

    YNCHttpWidgetsActionHandler() {}
    virtual ~YNCHttpWidgetsActionHandler() {}

protected:
    virtual YWidgetActionHandler* get_widget_handler();

};

#endif // YNCHttpWidgetsActionHandler_h
