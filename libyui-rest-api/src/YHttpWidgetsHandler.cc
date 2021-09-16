/*
  Copyright (C) 2017 SUSE LLC

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

#include <microhttpd.h>
#include <yui/YDialog.h>

#include "YWidgetFinder.h"
#include "YJsonSerializer.h"
#include "YHttpWidgetsHandler.h"


void YHttpWidgetsHandler::process_request(struct MHD_Connection* connection,
    const char* url, const char* method, const char* upload_data,
    size_t* upload_data_size, std::ostream& body, int& error_code,
    std::string& content_type, bool *redraw)
{
    content_type = "application/json";
    
    if (YDialog::topmostDialog(false))  {
        WidgetArray widgets;

        if ( MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, NULL, NULL ) == 0 ) {
            widgets = YWidgetFinder::all();
        }
        else {
            const char* label = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "label");
            const char* id = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "id");
            const char* type = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "type");
            const char* debug_label = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "debug_label");

            if ( label || id || type || debug_label )
            {
                widgets = YWidgetFinder::find(label, id, type, debug_label);
            }
            else {
                body << "{ \"error\" : \"No search criteria provided\" }" << std::endl;
                error_code = MHD_HTTP_NOT_FOUND;
                return;
            }
        }

        if (widgets.empty()) {
            body << "{ \"error\" : \"Widget not found\" }" << std::endl;
            error_code = MHD_HTTP_NOT_FOUND;
        }
        else {
            // non recursive dump
            YJsonSerializer::serialize(widgets, body, false);
            error_code = MHD_HTTP_OK;
        }
    }
    else {
        body << "{ \"error\" : \"No dialog is open\" }" << std::endl;
        error_code = MHD_HTTP_NOT_FOUND;
    }
}
