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
#include "YDialog.h"
#include "YDumbTab.h"
#include "YIntField.h"
#include "YItemSelector.h"
#include "YMenuBar.h"
#include "YMenuButton.h"
#include "YMultiLineEdit.h"
#include "YProperty.h"
#include "YPushButton.h"
#include "YRadioButton.h"
#include "YRichText.h"
#include "YTableActionHandler.h"
#include "YTree.h"
#include "YTreeItem.h"
#include "YWidgetID.h"

#include <codecvt>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <string>

#include "YHttpWidgetsActionHandler.h"

#define YUILogComponent "rest-api"
#include "YUILog.h"

void YHttpWidgetsActionHandler::process_request(struct MHD_Connection* connection,
    const char* url, const char* method, const char* upload_data,
    size_t* upload_data_size, std::ostream& body, int& error_code,
    std::string& content_type, bool *redraw)
{
    if ( YDialog::topmostDialog(false) )
    {
        WidgetArray widgets;

        content_type = "application/json";

        const char* label = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "label");
        const char* id = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "id");
        const char* type = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "type");

        if ( label || id || type )
        {
            widgets = YWidgetFinder::find(label, id, type);
        }
        else
        {
            body << "{ \"error\" : \"No search criteria provided\" }" << std::endl;
            error_code = MHD_HTTP_NOT_FOUND;
            return;
        }

        if ( widgets.empty() )
        {
            body << "{ \"error\" : \"Widget not found\" }" << std::endl;
            error_code = MHD_HTTP_NOT_FOUND;
            return;
        }

        if ( const char* action = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "action") )
        {
            if( widgets.size() != 1 )
            {
                body << "{ \"error\" : \"Multiple widgets found to act on, try using multicriteria search (label+id+type)\" }" << std::endl;
                error_code = MHD_HTTP_NOT_FOUND;
                return;
            }

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
}

int YHttpWidgetsActionHandler::do_action(YWidget *widget, const std::string &action, struct MHD_Connection *connection, std::ostream& body)
{

    // TODO improve this, maybe use better names for the actions...

    // press a button
    if ( action == "press" )
    {
        yuiMilestone() << "Received action: press" << std::endl;
        if ( dynamic_cast<YPushButton*>(widget) )
        {
            return action_handler<YPushButton>( widget, body, [&] (YPushButton *button) {
                yuiMilestone() << "Pressing button \"" << button->label() << '"' << std::endl;
                button->setKeyboardFocus();
                activate_widget( button );
            } );
        }
        std::string error ( "Action 'press' is not supported for the selected widget: \"" );
        error.append( widget->widgetClass() ).append( "\"" );
        return handle_error( body, error, MHD_HTTP_NOT_FOUND );
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
                checkbox->setChecked( true );
            } );
        }
        else if ( dynamic_cast<YCheckBoxFrame*>(widget) )
        {
            return action_handler<YCheckBoxFrame>( widget,
                                                   body,
                                                   [&] (YCheckBoxFrame *cbframe) {
                yuiMilestone() << "Checking \"" << cbframe->label() << '"' << std::endl;
                cbframe->setKeyboardFocus();
                cbframe->setValue( true );
                activate_widget( cbframe );
            },
                                                    true );
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

        std::string error ( "Action 'check' is not supported for the selected widget: \"" );
        error.append( widget->widgetClass() ).append( "\"" );
        return handle_error( body, error, MHD_HTTP_NOT_FOUND );
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
        else if ( dynamic_cast<YCheckBoxFrame*>(widget) )
        {
            return action_handler<YCheckBoxFrame>( widget, body, [&] (YCheckBoxFrame *cbframe) {
                yuiMilestone() << "Unchecking \"" << cbframe->label() << '"' << std::endl;
                cbframe->setKeyboardFocus();
                cbframe->setValue( false );
                activate_widget( cbframe );
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

        std::string error ( "Action 'uncheck' is not supported for the selected widget: \"" );
        error.append( widget->widgetClass() ).append( "\"" );
        return handle_error( body, error, MHD_HTTP_NOT_FOUND );
    }
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
        else if ( dynamic_cast<YCheckBoxFrame*>(widget) )
        {
            return action_handler<YCheckBoxFrame>( widget,
                                                   body,
                                                   [&] (YCheckBoxFrame *cbframe) {
                yuiMilestone() << "Toggling \"" << cbframe->label() << '"' << std::endl;
                cbframe->setKeyboardFocus();
                activate_widget( cbframe );
                cbframe->setValue( !cbframe->value() );
                activate_widget( cbframe );
            },
                                                   true ); //Allowing acting on disabled, as do not know state in advance
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

        std::string error ( "Action 'toggle' is not supported for the selected widget: \"" );
        error.append( widget->widgetClass() ).append( "\"" );
        return handle_error( body, error, MHD_HTTP_NOT_FOUND );
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
                activate_widget( input );
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
        else if ( dynamic_cast<YDateField*>(widget) )
        {
            return action_handler<YDateField>( widget, body, [&] (YDateField *input) {
                yuiMilestone() << "Setting value for YDateField \"" << input->label() << '"' << std::endl;
                input->setKeyboardFocus();
                input->setValue( value );
                activate_widget( input );
            } );
        }
        else if ( dynamic_cast<YTimeField*>(widget) )
        {
            return action_handler<YTimeField>( widget, body, [&] (YTimeField *input) {
                yuiMilestone() << "Setting value for YTimeField \"" << input->label() << '"' << std::endl;
                input->setKeyboardFocus();
                input->setValue( value );
                activate_widget( input );
            } );
        }
        else if ( dynamic_cast<YComboBox*>(widget) )
        {
            YComboBox* cb = dynamic_cast<YComboBox*>(widget);
            if( !cb->editable() )
            {
                std::string error ("Combobox " + cb->label() + "' is not editable ");
                return handle_error( body, error, MHD_HTTP_UNPROCESSABLE_ENTITY );
            }

            return action_handler<YComboBox>( widget, body, [&] (YComboBox *cb) {
                yuiMilestone() << "Setting value for YComboBox \"" << cb->label() << '"' << std::endl;
                cb->setKeyboardFocus();
                cb->setValue( value );
                activate_widget( cb );
            } );
        }

        std::string error ( "Action 'enter_text' is not supported for the selected widget: \"" );
        error.append( widget->widgetClass() ).append( "\"" );
        return handle_error( body, error, MHD_HTTP_NOT_FOUND );
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
                YItem * item = cb->findItem(value);
                if ( item )
                {
                    yuiMilestone() << "Activating Combobox \"" << cb->label() << '"' << std::endl;
                    cb->selectItem( item );
                    activate_widget( cb );
                }
                else
                {
                    throw YUIException("Item: '" + value + "' cannot be found in the combobox");
                }
            } );
        }
        else if( auto tbl = dynamic_cast<YTable*>(widget) )
        {
            int row_id = -1;
            if ( const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "row") )
                row_id = atoi(val);

            int column_id = 0;
            if ( const char* val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "column") )
                column_id = atoi(val);

            return action_handler<YTable>( widget, body, YTableActionHandler::get_handler(tbl, value, column_id, row_id) );
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
                    tree->selectItem( item );
                    activate_widget( tree );
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
                    tab->selectItem( item );
                    activate_widget( tab );
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
                    sb->selectItem( item );
                    activate_widget( sb );
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
        else if ( dynamic_cast<YRichText*>(widget) )
        {
            return action_handler<YRichText>(widget, body, [&] (YRichText *rt) {
                yuiMilestone() << "Activating hyperlink on richtext: \"" << value << '"' << std::endl;
                rt->setKeyboardFocus();
                rt->activateLink(value);
            } );
        }
        else if( dynamic_cast<YMenuButton*>(widget) )
        {
            return get_menu_selector_handler( dynamic_cast<YMenuButton*>(widget), value, body );
        }
        else if( dynamic_cast<YMenuBar*>(widget) )
        {
            return get_menu_selector_handler( dynamic_cast<YMenuBar*>(widget), value, body );
        }

        std::string error ( "Action 'select' is not supported for the selected widget: \"" );
        error.append( widget->widgetClass() ).append( "\"" );
        return handle_error( body, error, MHD_HTTP_NOT_FOUND );
    }
    else
    {
        body << "{ \"error\" : \"Unknown action\" }" << std::endl;
        return MHD_HTTP_NOT_FOUND;
    }

    return MHD_HTTP_OK;
}

void YHttpWidgetsActionHandler::activate_widget( YCheckBoxFrame * widget ) {};
void YHttpWidgetsActionHandler::activate_widget( YComboBox * widget ) {};
void YHttpWidgetsActionHandler::activate_widget( YDateField * widget ) {};
void YHttpWidgetsActionHandler::activate_widget( YInputField * widget ) {};
void YHttpWidgetsActionHandler::activate_widget( YTimeField * widget ) {};
void YHttpWidgetsActionHandler::activate_widget ( YSelectionBox * widget ) {};

void YHttpWidgetsActionHandler::activate_widget ( YMultiSelectionBox * widget, YItem * item ) {};
