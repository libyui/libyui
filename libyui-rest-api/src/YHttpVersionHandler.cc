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

#include "YHttpVersionHandler.h"
#include "YJsonSerializer.h"

#include "YHttpServer.h"
#include <microhttpd.h>
#include <string>
#include <json/json.h>

void YHttpVersionHandler::process_request(struct MHD_Connection* connection,
    const char* url, const char* method, const char* upload_data,
    size_t* upload_data_size, std::ostream& body, int& error_code,
    std::string& content_encoding, bool *redraw)
{
    Json::Value info;
    info["api_version"] = YUI_API_VERSION;
    YJsonSerializer::save(info, body);

    content_encoding = "application/json";
    error_code = MHD_HTTP_OK;
}
