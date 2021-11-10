/*
  Copyright (C) 2021 SUSE LLC

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

#define YUILogComponent "rest-api"
#include <yui/YUILog.h>

#include "YMultiSelectionBoxActionHandler.h"


std::function<void (YMultiSelectionBox*)> YMultiSelectionBoxActionHandler::check_handler( YMultiSelectionBox * widget,
                                                                const std::string &value,
                                                                CheckBoxStatus status )
{
    new_state = status;

    return [&] (YMultiSelectionBox *box) {
        YItem * item = box->findItem( value );
        yuiMilestone() << "Changing MultiSelectionBox \"" << box->label() << "\" Item: \"" << item << "\"" << std::endl;

        if ( item )
        {
            box->setKeyboardFocus();

            switch (new_state)
            {
                case ON:
                    box->selectItem( item, true );
                    break;
                case OFF:
                    box->selectItem( item, false );
                    break;
                case TOGGLE:
                    auto selected_items = box->selectedItems();
                    bool found = std::find(selected_items.begin(), selected_items.end(), item) != selected_items.end();
                    box->selectItem( item, !found ) ;
                    break;
            }
            // TODO FIXME: the ncurses UI does not send the :notify event ("ValueChanged")
        }
        else
        {
            throw YUIException("Item: '" + value + "' cannot be found in the widget");
        }
    };
}

std::function<void (YMultiSelectionBox*)> YMultiSelectionBoxActionHandler::select_handler( YMultiSelectionBox * widget,
                                                                const std::string &value)
{
    return [&] (YMultiSelectionBox *box) {
        YItem * item = box->findItem( value );
        yuiMilestone() << "Selecting item in MultiSelectionBox \"" << box->label() << "\" Item: \"" << item << "\"" << std::endl;

        if ( item )
        {
            box->setKeyboardFocus();
            box->setCurrentItem( item );
            // TODO FIXME: both ncurses and Qt UIs do not send the :notify event ("SelectionChanged")
        }
        else
        {
            throw YUIException("Item: '" + value + "' cannot be found in the widget");
        }
    };
}