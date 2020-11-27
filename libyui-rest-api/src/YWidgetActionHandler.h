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

#ifndef YWidgetActionHandler_h
#define YWidgetActionHandler_h

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

class YWidgetActionHandler
{

public:

    YWidgetActionHandler() {}
    virtual ~YWidgetActionHandler() {}

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

    virtual void activate_widget( YMultiSelectionBox * widget, YItem * item ) {};

    template<typename T>
    std::function<void (T*)> get_item_selector_handler( T *widget, const std::string &value, const int state = -1 ) {
        return [&] (T *selector) {
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
        };
    }

    template<typename T>
    std::function<void (T*)> get_menu_selector_handler( T *widget, const std::string &value ) {
        return [&] (T *menu_selector) {
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
        };
    }
};

#endif //YWidgetActionHandler_h
