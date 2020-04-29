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

#ifndef YUILogComponent
#define YUILogComponent   "ncurses-rest-api"
#endif //YUILogComponent

#include "YUILog.h"

#include "YNCHttpUI.h"
#include "YNCHttpWidgetsActionHandler.h"
#include <yui/YEvent.h>

int YNCHttpWidgetsActionHandler::do_action( YWidget *widget,
                                           const std::string &action,
                                           struct MHD_Connection *connection,
                                           std::ostream& body ) {

    // Use parent implementation if no special handling is required
    return YHttpWidgetsActionHandler::do_action( widget, action, connection, body );
}
