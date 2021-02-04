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

#ifndef YHttpWidgetsActionHandler_h
#define YHttpWidgetsActionHandler_h

#include <iostream>
#include <functional>
#include <microhttpd.h>
#include <sstream>
#include <boost/algorithm/string.hpp>

#define TreePathDelimiter "|"

#include <yui/YCheckBoxFrame.h>
#include <yui/YComboBox.h>
#include <yui/YDateField.h>
#include <yui/YInputField.h>
#include <yui/YItem.h>
#include <yui/YMultiSelectionBox.h>
#include <yui/YMenuItem.h>
#include <yui/YRadioButton.h>
#include <yui/YSelectionBox.h>
#include <yui/YTimeField.h>
#include <yui/YWidget.h>

#include "YTableActionHandler.h"
#include "YWidgetActionHandler.h"
#include "YWidgetFinder.h"

#include "YHttpHandler.h"


class YHttpWidgetsActionHandler : public YHttpHandler
{

public:

    YHttpWidgetsActionHandler() {};
    virtual ~YHttpWidgetsActionHandler() {};

protected:

    virtual void process_request(struct MHD_Connection* connection,
        const char* url, const char* method, const char* upload_data,
        size_t* upload_data_size, std::ostream& body, int& error_code,
        std::string& content_type, bool *redraw);

    int do_action( YWidget *widget, const std::string &action, struct MHD_Connection *connection, std::ostream& body );

    /**
     * Define widgets handlers to override in case need to implement
     * UI specific actions, like activation.
     **/
    virtual YWidgetActionHandler* get_widget_handler();
    virtual YTableActionHandler* get_table_handler();

    /**
     * Processes action on the given widget.
     * @tparam T the type of the widget handler will act on
     * @param widget Widget to which action will be applied
     * @param body HTTP response body stream
     * @param handler_func Function which will be called with widget as an argument
     * @param allow_disabled Some widgets get to disabled state, but are actually enabled, like YCheckBoxFrame
     *     Not allowing by default, but allow explicit overrides for the exceptions.
     * @return HTTP status code
     */
    template<typename T>
    int action_handler( YWidget *widget, std::ostream& body, std::function<void (T*)> handler_func, const bool allow_disabled = false ) {
        if (auto w = dynamic_cast<T*>(widget)) {
            try
            {
                // allow changing only the enabled widgets by default, as disabled ones
                // cannot be changed by user from the UI in most of the cases
                if( !widget->isEnabled() && !allow_disabled )
                {
                    std::string error ("Cannot operate on disabled widget: ");
                    error.append( typeid(*widget).name() );
                    return handle_error( body, error, MHD_HTTP_UNPROCESSABLE_ENTITY );
                }
                if ( handler_func )
                    handler_func(w);
            }
            // some widgets may throw an exception when setting invalid values
            catch (const YUIException &e)
            {
                std::string error ("");
                error.append( typeid(*widget).name() ).append( " " ).append( e.what() );
                return handle_error( body, error, MHD_HTTP_UNPROCESSABLE_ENTITY );
            }
        }
        else {
            return MHD_HTTP_NOT_FOUND;
        }

        return MHD_HTTP_OK;
    }

    YTableActionHandler  * table_action_handler = nullptr;
    YWidgetActionHandler * widget_action_handler = nullptr;

};

#endif // YHttpWidgetsActionHandler_h
