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

#ifndef YHttpServer_h
#define YHttpServer_h

#include <vector>
#include <string>

#include "YHttpServerSockets.h"
#include "YHttpMount.h"
#include "YHttpWidgetsActionHandler.h"

// environment variables
#define YUITest_HTTP_REMOTE "YUI_HTTP_REMOTE"
#define YUITest_HTTP_PORT   "YUI_HTTP_PORT"
#define YUI_AUTH_USER       "YUI_AUTH_USER"
#define YUI_AUTH_PASSWD     "YUI_AUTH_PASSWD"
#define YUI_REUSE_PORT      "YUI_REUSE_PORT"

#define YUI_API_VERSION     "v1"

struct MHD_Daemon;

class YHttpServer
{

public:

    static bool enabled()
    {
        static bool enabled = port_num() != 0;
        return enabled;
    }

    /**
    * Access the global YHttpServer.
    **/
    static YHttpServer * yserver() { return _yserver; }

    static int port_num();

    /**
     * Constructor to override widgets action handler. Is used in case there
     * are UI specific actions for the widget.
     **/
    YHttpServer( YHttpWidgetsActionHandler * widgets_action_handler );

    YHttpServer() : YHttpServer( new YHttpWidgetsActionHandler() ) {};

    ~YHttpServer();

    /**
     * Start the HTTP server
     */
    void start();

    /**
     * Process the data by the HTTP server
     * @return true if the UI content has been changed and it should be refreshed
     */
    bool process_data();

    /**
     * Return the list of the FDs used by the HTTP server,
     * these should be watched by the UI in the main event loop
     * (in addition to the user input)
     */
    YHttpServerSockets sockets();

    void mount(std::string path, const std::string &method, YHttpHandler *handler, bool has_api_version = true);

    int handle(struct MHD_Connection* connection,
        const char* url, const char* method, const char* upload_data,
        size_t* upload_data_size);

    // must be public to be accessible from a plain C callback :-/
    std::string user() const {return auth_user;}
    std::string passwd() const {return auth_passwd;}

private:

    // dual stack support (for both IPv4 and IPv6)
    struct MHD_Daemon *server_v4, *server_v6;
    std::vector<YHttpMount> _mounts;
    bool redraw;
    static YHttpServer * _yserver;
    static YHttpWidgetsActionHandler * _widget_action_handler;
    // HTTP Basic Auth credentials
    std::string auth_user;
    std::string auth_passwd;

    static YHttpWidgetsActionHandler * get_widget_action_handler() { return _widget_action_handler; }

protected:
};


#endif // YHttpServer_h
