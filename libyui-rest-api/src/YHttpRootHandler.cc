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

#include "YHttpRootHandler.h"
#include "YHttpServer.h"
#include "YJsonSerializer.h"

#include <microhttpd.h>
#include <string>
#include <cstring>
#include <boost/algorithm/string/replace.hpp>
#include <json/json.h>

#define YUILogComponent "rest-api"
#include "YUILog.h"

bool accepts_html(struct MHD_Connection *connection);

const std::string YHttpRootHandler::documentation_url = "https://github.com/libyui/libyui-rest-api/blob/master/API_v1.md";

void YHttpRootHandler::process_request(struct MHD_Connection* connection,
    const char* url, const char* method, const char* upload_data,
    size_t* upload_data_size, std::ostream& body, int& error_code,
    std::string& content_encoding, bool *redraw)
{
    if (accepts_html(connection))
    {
        body <<
"<html>"
"  <head><title>LibYUI Embedded Webserver</title></head>"
"  <body>"
"    <h2>The Libyui REST API Documentation</h2>"
"    <p>"
"      <a href='https://github.com/libyui/libyui-rest-api/blob/master/README.md'>"
"      The Generic REST API documentation</a>"
"    </p>"
"    <p>"
"      <a href='" << documentation_url << "'>"
"      The REST API v1 specification</a>"
"    </p>"
"    </body>"
"</html>";
        content_encoding = "text/html";
    }
    else
    {
        Json::Value info;
        info["documentation_url"] = documentation_url;
        YJsonSerializer::save(info, body);
        content_encoding = "application/json";
    }

    error_code = MHD_HTTP_OK;
}

// does the client accept an HTML response?
bool accepts_html(struct MHD_Connection *connection)
{
    const char *accept = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Accept");
    yuiDebug() << "Accept header: " << accept << std::endl;

    if (!accept) return false;
    return strstr(accept, "text/html");
}
