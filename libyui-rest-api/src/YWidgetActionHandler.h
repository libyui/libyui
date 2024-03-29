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
#include <boost/algorithm/string.hpp>

#define TreePathDelimiter "|"
#define ShortcutChar "&"

#include <yui/YCheckBoxFrame.h>
#include <yui/YComboBox.h>
#include <yui/YDateField.h>
#include <yui/YInputField.h>
#include <yui/YItem.h>
#include <yui/YMenuItem.h>
#include <yui/YMultiSelectionBox.h>
#include <yui/YRadioButton.h>
#include <yui/YSelectionBox.h>
#include <yui/YTimeField.h>
#include <yui/YWidget.h>

#include "YHttpHandler.h"
#include "YWidgetFinder.h"

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

    // Normalize a label before comparing it for equality:
    // - remove shortcut characters (&)
    // - remove BiDi control characters (added to certain table cells to render pathnames correctly)
    static std::string normalize_label(const std::string & label);

    // Apply normalize_label to both arguments and compare the result with ==.
    static bool normalized_labels_equal(const std::string & a, const std::string & b)
    {
        std::string na = normalize_label(a);
        std::string nb = normalize_label(b);
        return na == nb;
    }
};

#endif //YWidgetActionHandler_h
