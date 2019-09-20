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

#include "YComboBox.h"
#include "YDialog.h"
#include "YCheckBox.h"
#include "YInputField.h"
#include "YPushButton.h"
#include "YRadioButton.h"
#include "YTable.h"
#include "YTree.h"
#include "YTreeItem.h"
#include "YMultiLineEdit.h"
#include "YIntField.h"

#include <vector>
#include <sstream>
#include <cstdlib>
#include <string>
#include <boost/algorithm/string.hpp>

#include "YHttpWidgetsActionHandler.h"

void YHttpWidgetsActionHandler::body(struct MHD_Connection* connection,
    const char* url, const char* method, const char* upload_data,
    size_t* upload_data_size, std::ostream& body, bool *redraw)
{
    if (YDialog::topmostDialog(false))  {
        WidgetArray widgets;

        const char* label = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "label");
        const char* id = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "id");
        const char* type = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "type");

        if ( label || id || type ) {
            widgets = YWidgetFinder::find(label, id, type);
        } else {
            widgets = YWidgetFinder::all();
        }

        if (widgets.empty()) {
            body << "{ \"error\" : \"Widget not found\" }" << std::endl;
            _error_code = MHD_HTTP_NOT_FOUND;
        }
        else if (const char* action = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "action"))
        {
            if( widgets.size() != 1 ) {
                body << "{ \"error\" : \"Multiple widgets found to act on, try using multicriteria serach (label+id+type)\" }" << std::endl;
                _error_code = MHD_HTTP_NOT_FOUND;
            }
            _error_code = do_action(widgets[0], action, connection, body);

            // the action possibly changed something in the UI, signalize redraw needed
            if (redraw && _error_code == MHD_HTTP_OK)
                *redraw = true;
        }
        else {
            body << "{ \"error\" : \"Missing action parameter\" }" << std::endl;
            _error_code = MHD_HTTP_NOT_FOUND;
        }
    }
    else {
        body << "{ \"error\" : \"No dialog is open\" }" << std::endl;
        _error_code = MHD_HTTP_NOT_FOUND;
    }
}

std::string YHttpWidgetsActionHandler::contentEncoding()
{
    return "application/json";
}

int YHttpWidgetsActionHandler::do_action(YWidget *widget, const std::string &action, struct MHD_Connection *connection, std::ostream& body) {
    yuiMilestone() << "Starting action: " << action << std::endl;

    // TODO improve this, maybe use better names for the actions...

    // press a button
    if (action == "press") {
        return action_handler<YPushButton>(widget, [] (YPushButton *button) {
            yuiMilestone() << "Pressing button \"" << button->label() << '"' << std::endl;
            button->setKeyboardFocus();
            button->activate();
        } );
    }
    // check a checkbox
    else if (action == "check") {
        return action_handler<YCheckBox>(widget, [] (YCheckBox *checkbox) {
            if (checkbox->isChecked()) return;
            yuiMilestone() << "Checking \"" << checkbox->label() << '"' << std::endl;
            checkbox->setKeyboardFocus();
            checkbox->setChecked(true);
        } );
    }
    // uncheck a checkbox
    else if (action == "uncheck") {
        return action_handler<YCheckBox>(widget, [] (YCheckBox *checkbox) {
            if (!checkbox->isChecked()) return;
            yuiMilestone() << "Unchecking \"" << checkbox->label() << '"' << std::endl;
            checkbox->setKeyboardFocus();
            checkbox->setChecked(false);
        } );
    }
    // toggle a checkbox (reverse the state)
    else if (action == "toggle") {
        return action_handler<YCheckBox>(widget, [] (YCheckBox *checkbox) {
            yuiMilestone() << "Toggling \"" << checkbox->label() << '"' << std::endl;
            checkbox->setKeyboardFocus();
            checkbox->setChecked(!checkbox->isChecked());
        } );
    }
    // enter input field text
    else if (action == "enter_text") {
        std::string value;
        if (const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "value"))
            value = val;

        if (dynamic_cast<YInputField*>(widget)) {
            return action_handler<YInputField>(widget, [&] (YInputField *input) {
                yuiMilestone() << "Setting value for InputField \"" << input->label() << '"' << std::endl;
                input->setKeyboardFocus();
                input->setValue(value);
            } );
        }
        else if (dynamic_cast<YIntField*>(widget)) {
            return action_handler<YIntField>(widget, [&] (YIntField *input) {
                yuiMilestone() << "Setting value for YIntField \"" << input->label() << '"' << std::endl;
                input->setKeyboardFocus();
                input->setValue(atoi(value.c_str()));
            } );
        }
        else if (dynamic_cast<YMultiLineEdit*>(widget)) {
            return action_handler<YMultiLineEdit>(widget, [&] (YMultiLineEdit *input) {
                yuiMilestone() << "Setting value for YMultiLineEdit \"" << input->label() << '"' << std::endl;
                input->setKeyboardFocus();
                input->setValue(value);
            } );
        }
        else {
            body << "Action is not supported for the selected widget: " << widget->widgetClass() << std::endl;
            return MHD_HTTP_NOT_FOUND;
        }
    }
    else if (action == "switch_radio") {
        return action_handler<YRadioButton>(widget, [&] (YRadioButton *rb) {
            yuiMilestone() << "Activating RadioButton \"" << rb->label() << '"' << std::endl;
            rb->setKeyboardFocus();
            rb->setValue(true);
        } );
    }
    else if (action == "select") {
        std::string value;
        if (const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "value"))
            value = val;
        if (dynamic_cast<YComboBox*>(widget)) {
            return action_handler<YComboBox>(widget, [&] (YComboBox *cb) {
                yuiMilestone() << "Activating ComboBox \"" << cb->label() << '"' << std::endl;
                cb->setKeyboardFocus();
                cb->setValue(value);
            } );
        }
        else if(dynamic_cast<YTable*>(widget)) {
            int column_id = 0;
            if (const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "column"))
                column_id = atoi(val);
            return action_handler<YTable>(widget, [&] (YTable *tb) {
                YItem * item = tb->findItem(value, column_id);
                if (item) {
                        yuiMilestone() << "Activating Table \"" << tb->label() << '"' << std::endl;
                        tb->setKeyboardFocus();
                        tb->selectItem(item);
                }
                else {
                    body << '"' << value << "\" item cannot be found in the table" << std::endl;
                    throw YUIException("Item cannot be found in the table");
                }
            } );
        }
        else if(dynamic_cast<YTree*>(widget)) {
            return action_handler<YTree>(widget, [&] (YTree *tree) {
                // Vector of string to store path to the tree item
                std::vector<std::string> path;
                boost::split( path, value, boost::is_any_of( TreePathDelimiter ) );
                YItem * item = tree->findItem( path );
                if (item) {
                    yuiMilestone() << "Activating Tree Item \"" << item->label() << '"' << std::endl;
                    tree->setKeyboardFocus();
                    tree->selectItem(item);
                    tree->activate();
                }
                else {
                    body << '"' << value << "\" item cannot be found in the tree" << std::endl;
                    throw YUIException("Item cannot be found in the tree");
                }
            } );
        }
        else {
            body << "Action is not supported for the selected widget" << widget->widgetClass() << std::endl;
            return MHD_HTTP_NOT_FOUND;
        }
    }
    // TODO: more actions
    // else if (action == "enter") {
    // }
    else {
        body << "{ \"error\" : \"Unknown action\" }" << std::endl;
        return MHD_HTTP_NOT_FOUND;
    }

    return MHD_HTTP_OK;
}
