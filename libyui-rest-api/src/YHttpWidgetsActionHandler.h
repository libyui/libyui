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

#define TreePathDelimiter "|"

#include "YCheckBoxFrame.h"
#include "YComboBox.h"
#include "YDateField.h"
#include "YHttpHandler.h"
#include "YInputField.h"
#include "YItem.h"
#include "YMultiSelectionBox.h"
#include "YMenuItem.h"
#include "YRadioButton.h"
#include "YSelectionBox.h"
#include "YTimeField.h"
#include "YWidgetFinder.h"
#include "YWidget.h"

#include <boost/algorithm/string.hpp>

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

    int do_action( YWidget *widget, const std::string &action, struct MHD_Connection *connection, std::ostream& body );

    // TODO: move this somewhere else...

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


    /**
     * Define default widget activation and override only widgets which
     * either don't have method availaible in libyui or if they require
     * exceptional handling.
     **/
    template<typename T>
    void activate_widget( T * widget ) {
        widget->activate();
    }

    /**
     * Declare methods where we need to override widget activation for nc or qt
     * We keep empty methods here, that it still works in case of missing
     * override in the children classes.
     **/
    virtual void activate_widget( YCheckBoxFrame * widget ) {};
    virtual void activate_widget( YComboBox * widget ) {};
    virtual void activate_widget( YDateField * widget ) {};
    virtual void activate_widget( YInputField * widget ) {};
    virtual void activate_widget( YRadioButton * widget ) {};
    virtual void activate_widget( YTimeField * widget ) {};
    virtual void activate_widget( YSelectionBox * widget ) {};

    /**
     * Same as activate_widget, but for some widgets we also need to specify
     * item for the selection, so provide method to do exactly that.
     */
    template<typename T, typename I >
    void activate_widget( T * selector, I *item ) {
        selector->activateItem( item );
    }

    virtual void activate_widget ( YMultiSelectionBox * widget, YItem * item ) {};

    template<typename T>
    int get_item_selector_handler( T *widget, const std::string &value, std::ostream& body, const int state = -1 ) {
        return action_handler<T>( widget, body, [&] (T *selector) {
            YItem * item = selector->findItem( value );
            if ( item )
            {
                selector->setKeyboardFocus();
                // Toggle in case state selector undefined
                bool select = state < 0  ? !item->selected() : (state != 0);
                if( state < 0 )
                {
                    select = !item->selected();
                }
                else
                {
                    select = (state != 0);
                }
                item->setSelected( select );
                selector->selectItem( item, select );
                activate_widget( selector, item );
            }
            else
            {
                throw YUIException("Item: '" + value + "' cannot be found in the item selector widget");
            }
        } );
    }

    template<typename T>
    int get_menu_selector_handler( T *widget, const std::string &value, std::ostream& body ) {
        return action_handler<T>( widget, body, [&] (T *menu_selector) {
            // Vector of string to store path to the tree item
            std::vector<std::string> path;
            boost::split( path, value, boost::is_any_of( TreePathDelimiter ) );
            YMenuItem * item = menu_selector->findItem( path );
            if ( item )
            {
                menu_selector->setKeyboardFocus();
                activate_widget( menu_selector, item );
            }
            else
            {
                throw YUIException("Item with path: '" + value + "' cannot be found in the menu selector widget");
            }
        } );
    }
};

#endif // YHttpWidgetsActionHandler_h
