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
#include <boost/algorithm/string/replace.hpp>
#include <json/json.h>

const std::string YHttpRootHandler::documentation_url = "https://github.com/libyui/libyui-rest-api/#libyui-embedded-webserver";

void YHttpRootHandler::body(struct MHD_Connection* connection,
    const char* url, const char* method, const char* upload_data,
    size_t* upload_data_size, std::ostream& body, bool *redraw)
{
    Json::Value info;
    info["documentation_url"] = documentation_url;
    YJsonSerializer::save(info, body);
}

std::string YHttpRootHandler::contentEncoding()
{
    return "application/json";
}

int YHttpRootHandler::errorCode()
{
    return MHD_HTTP_OK;
}
