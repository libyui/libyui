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

#include "YCheckBox.h"
#include "YComboBox.h"
#include "YDialog.h"
#include "YDumbTab.h"
#include "YInputField.h"
#include "YIntField.h"
#include "YItemSelector.h"
#include "YMenuButton.h"
#include "YMultiLineEdit.h"
#include "YPushButton.h"
#include "YRadioButton.h"
#include "YRichText.h"
#include "YTable.h"
#include "YTree.h"
#include "YTreeItem.h"
#include "YSelectionBox.h"
#include "YMultiSelectionBox.h"
#include "YWidgetID.h"

#include <codecvt>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <string>
#include <boost/algorithm/string.hpp>

#include "YHttpWidgetsActionHandler.h"

void YHttpWidgetsActionHandler::process_request(struct MHD_Connection* connection,
    const char* url, const char* method, const char* upload_data,
    size_t* upload_data_size, std::ostream& body, int& error_code,
    std::string& content_type, bool *redraw)
{
    if ( YDialog::topmostDialog(false) )
    {
        WidgetArray widgets;

        const char* label = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "label");
        const char* id = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "id");
        const char* type = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "type");

        if ( label || id || type )
        {
            widgets = YWidgetFinder::find(label, id, type);
        }
        else
        {
            widgets = YWidgetFinder::all();
        }

        if ( widgets.empty() )
        {
            body << "{ \"error\" : \"Widget not found\" }" << std::endl;
            error_code = MHD_HTTP_NOT_FOUND;
        }

        if ( const char* action = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "action") )
        {
            if( widgets.size() != 1 )
            {
                body << "{ \"error\" : \"Multiple widgets found to act on, try using multicriteria search (label+id+type)\" }" << std::endl;
                error_code = MHD_HTTP_NOT_FOUND;
            }
            else
                error_code = do_action(widgets[0], action, connection, body);

            // the action possibly changed something in the UI, signalize redraw needed
            if ( redraw && error_code == MHD_HTTP_OK )
                *redraw = true;
        }
        else
        {
            body << "{ \"error\" : \"Missing action parameter\" }" << std::endl;
            error_code = MHD_HTTP_NOT_FOUND;
        }
    }
    else {
        body << "{ \"error\" : \"No dialog is open\" }" << std::endl;
        error_code = MHD_HTTP_NOT_FOUND;
    }

    content_type = "application/json";
}

int YHttpWidgetsActionHandler::do_action(YWidget *widget, const std::string &action, struct MHD_Connection *connection, std::ostream& body) {

    yuiMilestone() << "Starting action: " << action << std::endl;

    // TODO improve this, maybe use better names for the actions...

    // press a button
    if ( action == "press" )
    {
        yuiMilestone() << "Received action: press" << std::endl;
        if (dynamic_cast<YPushButton*>(widget))
        {
            return action_handler<YPushButton>( widget, body, [] (YPushButton *button)
            {
                yuiMilestone() << "Pressing button \"" << button->label() << '"' << std::endl;
                button->setKeyboardFocus();
                button->activate();
            } );
        }
        else if ( dynamic_cast<YRichText*>(widget) )
        {
            std::string value;
            if ( const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "value") )
                value = val;
            return action_handler<YRichText>(widget, body, [&] (YRichText *rt) {
                yuiMilestone() << "Activating hyperlink on richtext: \"" << value << '"' << std::endl;
                rt->setKeyboardFocus();
                rt->activateLink(value);
            } );
        }
        else if( dynamic_cast<YMenuButton*>(widget) )
        {
            std::string value;
            if ( const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "value") )
                value = val;
            return action_handler<YMenuButton>( widget, body, [&] (YMenuButton *mb) {
                // Vector of string to store path to the tree item
                std::vector<std::string> path;
                boost::split( path, value, boost::is_any_of( TreePathDelimiter ) );
                YMenuItem * item = mb->findItem( path );
                if ( item )
                {
                    yuiMilestone() << "Activating Item by path :" << value << " in \"" << mb->label() << "\" MenuButton" << std::endl;
                    mb->setKeyboardFocus();
                    mb->activateItem( item );
                }
                else
                {
                    throw YUIException("Item with path: '" + value + "' cannot be found in the MenuButton widget");
                }
            } );
        }

        body << "{ \"error\" : Action 'press' is not supported for the selected widget: \"" << widget->widgetClass() << "\" }" << std::endl;
        return MHD_HTTP_NOT_FOUND;
    }
    // check a checkbox
    else if ( action == "check" )
    {
        if ( dynamic_cast<YCheckBox*>(widget) )
        {
            return action_handler<YCheckBox>( widget, body, [] (YCheckBox *checkbox) {
                if (checkbox->isChecked()) return;
                yuiMilestone() << "Checking \"" << checkbox->label() << '"' << std::endl;
                checkbox->setKeyboardFocus();
                checkbox->setChecked(true);
            } );
        }
        else
        {
            std::string value;
            if ( const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "value") )
                value = val;

            if( YItemSelector* selector = dynamic_cast<YItemSelector*>(widget) )
            {
                return get_item_selector_handler( selector, value, body, 1 );
            }

            if( YMultiSelectionBox* selector = dynamic_cast<YMultiSelectionBox*>(widget) )
            {
                return get_item_selector_handler( selector, value, body, 1 );
            }
        }

        body << "Action 'check' is not supported for the selected widget \"" << widget->widgetClass() << "\" }" << std::endl;
        return MHD_HTTP_NOT_FOUND;
    }
    // uncheck a checkbox
    else if ( action == "uncheck" )
    {
        if ( dynamic_cast<YCheckBox*>(widget) )
        {
            return action_handler<YCheckBox>( widget, body, [] (YCheckBox *checkbox) {
                if (!checkbox->isChecked()) return;
                yuiMilestone() << "Unchecking \"" << checkbox->label() << '"' << std::endl;
                checkbox->setKeyboardFocus();
                checkbox->setChecked(false);
            } );
        }
        else
        {
            std::string value;
            if ( const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "value") )
                value = val;

            if( YItemSelector* selector = dynamic_cast<YItemSelector*>(widget) )
            {
                return get_item_selector_handler( selector, value, body, 0 );
            }

            if( YMultiSelectionBox* selector = dynamic_cast<YMultiSelectionBox*>(widget) )
            {
                return get_item_selector_handler( selector, value, body, 0 );
            }
        }

        body << "{ \"error\" : \"Action 'uncheck' is not supported for the selected widget \"" << widget->widgetClass() << "\" }" << std::endl;
        return MHD_HTTP_NOT_FOUND;
    }
    // toggle a checkbox (reverse the state)
    else if ( action == "toggle" )
    {
        if ( dynamic_cast<YCheckBox*>(widget) )
        {
            return action_handler<YCheckBox>( widget, body, [] (YCheckBox *checkbox) {
                yuiMilestone() << "Toggling \"" << checkbox->label() << '"' << std::endl;
                checkbox->setKeyboardFocus();
                checkbox->setChecked(!checkbox->isChecked());
            } );
        }
        else
        {
            std::string value;
            if ( const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "value") )
                value = val;

            if( YItemSelector* selector = dynamic_cast<YItemSelector*>(widget) )
            {
                return get_item_selector_handler( selector, value, body );
            }

            if( YMultiSelectionBox* selector = dynamic_cast<YMultiSelectionBox*>(widget) )
            {
                return get_item_selector_handler( selector, value, body );
            }
        }

        body << "{ \"error\" : \"Action 'toggle' is not supported for the selected widget \"" << widget->widgetClass() << "\" }" << std::endl;
        return MHD_HTTP_NOT_FOUND;
    }
    // enter input field text
    else if ( action == "enter_text" )
    {
        std::string value;
        if ( const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "value") )
            value = val;

        if ( dynamic_cast<YInputField*>(widget) )
        {
            return action_handler<YInputField>( widget, body, [&] (YInputField *input) {
                yuiMilestone() << "Setting value for InputField \"" << input->label() << '"' << std::endl;
                input->setKeyboardFocus();
                input->setValue(value);
            } );
        }
        else if ( dynamic_cast<YIntField*>(widget) )
        {
            return action_handler<YIntField>( widget, body, [&] (YIntField *input) {
                yuiMilestone() << "Setting value for YIntField \"" << input->label() << '"' << std::endl;
                input->setKeyboardFocus();
                input->setValue(atoi(value.c_str()));
            } );
        }
        else if ( dynamic_cast<YMultiLineEdit*>(widget) )
        {
            return action_handler<YMultiLineEdit>( widget, body, [&] (YMultiLineEdit *input) {
                yuiMilestone() << "Setting value for YMultiLineEdit \"" << input->label() << '"' << std::endl;
                input->setKeyboardFocus();
                input->setValue(value);
            } );
        }

        body << "{ \"error\" : \"Action 'enter_text' is not supported for the selected widget \"" << widget->widgetClass() << "\" }" << std::endl;
        return MHD_HTTP_NOT_FOUND;
    }
    else if ( action == "select" )
    {
        std::string value;
        if (const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "value"))
            value = val;

        if ( dynamic_cast<YComboBox*>(widget) )
        {
            return action_handler<YComboBox>( widget, body, [&] (YComboBox *cb) {
                yuiMilestone() << "Activating ComboBox \"" << cb->label() << '"' << std::endl;
                cb->setKeyboardFocus();
                // cb->setValue(value);
                YItem * item = cb->findItem(value);
                if ( item )
                {
                    yuiMilestone() << "Activating Combobox \"" << cb->label() << '"' << std::endl;
                    cb->selectItem(item);
                    cb->activate();
                }
                else
                {
                    throw YUIException("Item: '" + value + "' cannot be found in the combobox");
                }
            } );
        }
        else if( dynamic_cast<YTable*>(widget) )
        {
            int column_id = 0;
            if ( const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "column") )
                column_id = atoi(val);
            return action_handler<YTable>( widget, body, [&] (YTable *tb) {
                YItem * item = tb->findItem(value, column_id);
                if ( item )
                {
                        yuiMilestone() << "Activating Table \"" << tb->label() << '"' << std::endl;
                        tb->setKeyboardFocus();
                        tb->selectItem(item);
                }
                else
                {
                    throw YUIException("Item: '" + value + "' cannot be found in the table");
                }
            } );
        }
        else if( dynamic_cast<YTree*>(widget) )
        {
            return action_handler<YTree>( widget, body, [&] (YTree *tree) {
                // Vector of string to store path to the tree item
                std::vector<std::string> path;
                boost::split( path, value, boost::is_any_of( TreePathDelimiter ) );
                YItem * item = tree->findItem( path );
                if (item)
                {
                    yuiMilestone() << "Activating Tree Item \"" << item->label() << '"' << std::endl;
                    tree->setKeyboardFocus();
                    tree->selectItem(item);
                    tree->activate();
                }
                else
                {
                    throw YUIException("Item: '" + value + "' cannot be found in the tree");
                }
            } );
        }
        else if ( dynamic_cast<YDumbTab*>(widget) )
        {
            return action_handler<YDumbTab>( widget, body, [&] (YDumbTab *tab) {
                YItem * item = tab->findItem( value );
                if ( item )
                {
                    yuiMilestone() << "Activating Tree Item \"" << item->label() << '"' << std::endl;
                    tab->setKeyboardFocus();
                    tab->selectItem(item);
                    tab->activate();
                }
                else
                {
                    throw YUIException("Item: '" + value + "' cannot be found among tabs");
                }
            } );
        }
        else if( dynamic_cast<YRadioButton*>(widget) )
        {
            return action_handler<YRadioButton>( widget, body, [&] (YRadioButton *rb) {
                yuiMilestone() << "Activating RadioButton \"" << rb->label() << '"' << std::endl;
                rb->setKeyboardFocus();
                rb->setValue(true);
            } );
        }
        else if( dynamic_cast<YSelectionBox*>(widget) )
        {
            return action_handler<YSelectionBox>( widget, body, [&] (YSelectionBox *sb) {
                YItem * item = sb->findItem( value );
                if ( item )
                {
                    yuiMilestone() << "Activating selection box \"" << sb->label() << '"' << std::endl;
                    sb->setKeyboardFocus();
                    sb->selectItem(item);
                    sb->activate();
                }
                else
                {
                    throw YUIException("Item: '" + value + "' cannot be found in the selection box");
                }
            } );
        }
        else if( dynamic_cast<YMultiSelectionBox*>(widget) )
        {
            return get_item_selector_handler( dynamic_cast<YMultiSelectionBox*>(widget), value, body, 1 );
        }
        else if( dynamic_cast<YItemSelector*>(widget) )
        {
            return get_item_selector_handler( dynamic_cast<YItemSelector*>(widget), value, body, 1 );
        }

        body << "{ \"error\" : \"Action 'select' is not supported for the selected widget \"" << widget->widgetClass() << "\" }" << std::endl;
        return MHD_HTTP_NOT_FOUND;
    }
    else
    {
        body << "{ \"error\" : \"Unknown action\" }" << std::endl;
        return MHD_HTTP_NOT_FOUND;
    }

    return MHD_HTTP_OK;
}
