#
# Combo box example
#
require File.join(File.dirname(__FILE__),'_loadpath')

require 'yui'

    editable = ARGV.size > 0

    Yui::YUILog::set_log_file_name "/tmp/libyui-examples.log"
    Yui::YUILog::enable_debug_logging
    
    factory = Yui::YUI::widget_factory
    dialog = factory.create_popup_dialog
    vbox = factory.create_vbox dialog

    comboBox = factory.create_combo_box( vbox, "&Menu", editable )

    comboBox.add_item( Yui::YItem.new( "Pizza Margherita" 	 ) )
    comboBox.add_item( Yui::YItem.new( "Pizza Capricciosa"	 ) )
    comboBox.add_item( Yui::YItem.new( "Pizza Funghi"		 ) )
    comboBox.add_item( Yui::YItem.new( "Pizza Prosciutto"	 ) )
    comboBox.add_item( Yui::YItem.new( "Pizza Quattro Stagioni" ) )
    comboBox.add_item( Yui::YItem.new( "Calzone"		 ) )


    hbox = factory.create_hbox( vbox )
    valueField  = factory.create_output_field( hbox, "<ComboBox value unknown>" )
    valueField.set_stretchable( Yui::YD_HORIZ, true ) # allow stretching over entire dialog width

    valueButton = factory.create_push_button( hbox, "&Value" )

    factory.create_vspacing( vbox, 0.3 )

    rightAlignment = factory.create_right( vbox )
    closeButton    = factory.create_push_button( rightAlignment, "&Close" )


    #
    # Event loop
    #

    loop do
	event = dialog.wait_for_event

	if ( event )
	    break if ( event.event_type == Yui::YEvent::CancelEvent ) # window manager "close window" button

	    valueField.set_value( "???" )

	    if ( event.widget == closeButton )
		break # leave event loop

	    if ( event.widget == valueButton || event.widget == comboBox )	# comboBox will only send events with setNotify()
		# Get the current value of the ComboBox and display it in valueField.
		#
		# In normal applications, it should be clear whether or not a
		# ComboBox is editable, so the following if()...else should
		# not be necessary, only either the "if" branch or the "else" branch.

		if ( comboBox.editable )
		    # An editable ComboBox can have a selectedItem(), but the user can
		    # also enter any other text. YComboBox::value() will return that text or,
		    # if the user chose an item from the list, that item's
		    # label.
		    #
		    # YComboBox::set_value() OTOH will automatically select a list item
		    # if an item with that label exists.
		    #
		    # Semantically, an editable ComboBox is an InputField with some
		    # frequently used values in a drop-down list. This means that it
		    # makes a lot more sense to use strings and YComboBox::value() /
		    # YComboBox::set_value() consistently for such a ComboBox, not
		    # YComboBox::selected_item() / YComboBox::select_item().

		    valueField.set_value( comboBox.value )
		else # not editable
		    # For non-editable ComboBox widgets, using items is preferred:
		    # YComboBox::selected_item() / YComboBox::select_item().
		    #
		    # While it is possible to use YComboBox::value() and
		    # YComboBox::set_value(), this is not the idea behind it.
		    
		    item = comboBox.selected_item

		    if ( item )
			valueField.set_value( item.label )
		    else
			valueField.set_value( "<none>" )
		    end
		end
	    end
	end
    end


    #
    # Clean up
    #

    dialog.destroy
end
