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

#include <json/json.h>
#include <microhttpd.h>
#include <sstream>

#include "YJsonSerializer.h"
#include "YHttpHandler.h"

#define YUILogComponent "rest-api"
#include "YUILog.h"

MHD_RESULT YHttpHandler::handle(struct MHD_Connection* connection,
        const char* url, const char* method, const char* upload_data,
        size_t* upload_data_size, bool *redraw)
{
    std::ostringstream body_s;
    std::string content_type;
    int error_code;

    process_request(connection, url, method, upload_data, upload_data_size,
      body_s, error_code, content_type, redraw);

    std::string body_str = body_s.str();
    struct MHD_Response *response = MHD_create_response_from_buffer (body_str.length(),
		      (void *) body_str.c_str(), MHD_RESPMEM_MUST_COPY);

    if (!content_type.empty())
        MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, content_type.c_str());

    yuiMilestone() << "Sending response: code: " << error_code << ", body size: " << body_str.length()
      << ", content type: " << content_type << std::endl;

    MHD_RESULT ret = MHD_queue_response(connection, error_code, response);
    MHD_destroy_response (response);
    return ret;
}

int YHttpHandler::handle_error(std::ostream& body, std::string error, int error_code)
{
    Json::Value response;
    response["error"] = error;
    YJsonSerializer::save(response, body);
    return error_code;
}
