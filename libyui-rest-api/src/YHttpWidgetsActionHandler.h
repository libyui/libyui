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

#include "YHttpHandler.h"
#include "YWidgetFinder.h"
#include "YWidget.h"
#include "YItem.h"

#include <iostream>
#include <functional>
#include <microhttpd.h>

#define TreePathDelimiter "|"

#include "YComboBox.h"
#include "YDateField.h"
#include "YHttpHandler.h"
#include "YInputField.h"
#include "YJsonSerializer.h"
#include "YItem.h"
#include "YMultiSelectionBox.h"
#include "YSelectionBox.h"
#include "YTimeField.h"
#include "YWidgetFinder.h"
#include "YWidget.h"


class YHttpWidgetsActionHandler : public YHttpHandler
{

public:

    YHttpWidgetsActionHandler() {}
    virtual ~YHttpWidgetsActionHandler() {}

protected:

    virtual void process_request(struct MHD_Connection* connection,
        const char* url, const char* method, const char* upload_data,
        size_t* upload_data_size, std::ostream& body, int& error_code,
        std::string& content_type, bool *redraw);

    virtual int do_action( YWidget *widget, const std::string &action, struct MHD_Connection *connection, std::ostream& body );

    template<typename T>
    int action_handler( YWidget *widget, std::ostream& body, std::function<void (T*)> handler_func ) {
        if (auto w = dynamic_cast<T*>(widget)) {
            try
            {
                // allow changing only the enabled widgets, disabled ones
                // cannot be changed by user from the UI, do not be more powerfull
                if (handler_func && widget->isEnabled()) handler_func(w);
            }
            // some widgets may throw an exception when setting invalid values
            catch (const YUIException &e)
            {
                body << "{ error: \"" << typeid(*widget).name() << " " << e.what() << "\" }" << std::endl;
                return MHD_HTTP_UNPROCESSABLE_ENTITY;
            }
        }
        else {
            // TODO: demangle the C++ names here ?
            // https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
            return MHD_HTTP_NOT_FOUND;
        }

        return MHD_HTTP_OK;
    }

    // TODO: move this somewhere else...

    virtual int do_action(YWidget *widget, const std::string &action, struct MHD_Connection *connection, std::ostream& body);

private:

    int _error_code;

    template<typename T>
    int get_item_selector_handler(T *widget, const std::string &value, std::ostream& body, const int state = -1) {
        return action_handler<T>( widget, body, [&] (T *selector) {
            // auto selector = dynamic_cast<T*>(widget);

            YItem * item = selector->findItem( value );
            if ( item )
            {
                // yuiMilestone() << "Activating item selector with item \"" << value << '"' << std::endl;
                selector->setKeyboardFocus();
                // Toggle in case state selector undefined
                bool select = state < 0  ? !item->selected() :
                              state == 0 ? false :
                                           true;
                if( state < 0 )
                {
                    select = !item->selected();
                }
                else
                {
                    select = state == 0 ? false : true;
                }
                item->setSelected( select );
                selector->selectItem( item, select );
                selector->activateItem( item );
            }
            else
            {
                throw YUIException("Item: '" + value + "' cannot be found in the item selector widget");
            }
        } );
    }

};

#endif // YHttpWidgetsActionHandler_h
