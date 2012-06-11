#
# Selection box example
#
require File.join(File.dirname(__FILE__),'_loadpath')

require 'yui'

def pizzaItems
    items = Array.new
    items << Yui::YItem.new( "Pizza Margherita" 	 )
    items << Yui::YItem.new( "Pizza Capricciosa"	 )
    items << Yui::YItem.new( "Pizza Funghi"		 )
    items << Yui::YItem.new( "Pizza Prosciutto"	 )
    items << Yui::YItem.new( "Pizza Quattro Stagioni" )
    items << Yui::YItem.new( "Calzone"		 )

    items
end


def pastaItems
    items = Array.new
    items << Yui::YItem.new( "Spaghetti Napoli" 	 )
    items << Yui::YItem.new( "Spaghetti Bolognese" 	 )
    items << Yui::YItem.new( "Fusili Emiliana" 	 )
    items << Yui::YItem.new( "Penne Calabrese" 	 )
    items << Yui::YItem.new( "Penne Al Forno" 	 )
    items << Yui::YItem.new( "Lasagne"	 	 )

    items
end


    Yui::YUILog::set_log_file_name "/tmp/libyui-examples.log"
    Yui::YUILog::enable_debug_logging
    
    factory = Yui::YUI::widget_factory
    dialog = factory.create_popup_dialog
    vbox = factory.create_vbox dialog

    # Specify larger size for the SelectionBox: It can scroll, so its size
    # depends on other widgets in the layout.
    minSize = factory.create_min_size( vbox, 40, 8 ) # minWidth, minHeight


    # The SelectionBox. This is what this example all about.
    # The other widgets are just here to allow some experimenting.

    selBox = factory.create_selection_box( minSize, "&Menu" )
    pizzaItems.each { |i| selBox.add_item( i ) }


    # Buttons to allow changing the SelectionBox items

    buttonBox      = factory.create_hbox( vbox )
    pastaButton    = factory.create_push_button( buttonBox, "&Pasta" )
    pizzaButton    = factory.create_push_button( buttonBox, "Pi&zza" )
    clearButton    = factory.create_push_button( buttonBox, "C&lear" )
    deselectButton = factory.create_push_button( buttonBox, "&Deselect" )

    factory.create_vspacing( vbox, 0.5 )


    # Check boxes to allow toogling "notify" and "immediate" for the SelectionBox

    leftAlignment = factory.create_left( vbox )
    notifyCheckBox = factory.create_check_box( leftAlignment, "&Notify Mode" )
    notifyCheckBox.set_notify

    leftAlignment = factory.create_left( vbox )
    immediateCheckBox = factory.create_check_box( leftAlignment, "&Immediate Mode" )
    immediateCheckBox.set_notify

    factory.create_vspacing( vbox, 0.3 )


    # OutputField + button to allow monitoring the SelectionBox value
    # (and when it changes, i.e. when events are sent)
    
    hbox = factory.create_hbox( vbox )
    valueField  = factory.create_output_field( hbox, "???" )
    valueField.set_stretchable( Yui::YD_HORIZ, true ) # allow stretching over entire dialog width

    valueButton = factory.create_push_button( hbox, "&Value" )

    factory.create_vspacing( vbox, 0.7 )


    # "Close" button

    rightAlignment = factory.create_right( vbox )
    closeButton    = factory.create_push_button( rightAlignment, "&Close" )


    #
    # Event loop
    #

    loop do
	event = dialog.wait_for_event

	if ( event )
	    break if ( event.event_type() == Yui::YEvent::CancelEvent ) # window manager "close window" button

	    valueField.set_value( "???" )

	    if ( event.widget == closeButton )
		break
	    elsif ( event.widget == pastaButton )
		selBox.delete_all_items
		pastaItems.each { |i| selBox.add_item( i ) }
	    elsif ( event.widget == pizzaButton )
		selBox.delete_all_items
		pizzaItems.each { |i| selBox.add_item( i ) }
	    elsif ( event.widget == clearButton )
		selBox.delete_all_items
	    elsif ( event.widget == deselectButton )
		selBox.deselect_all_items
	    elsif ( event.widget == notifyCheckBox )
		notify = notifyCheckBox.is_checked

		if ( ! notify )
		    # immediateMode implicitly includes notify, so set
		    # immediateMode off if the user wants to set notify off

		    selBox.set_immediate_mode( false )
		    immediateCheckBox.set_checked( false )
		end

		selBox.set_notify( notify )
	    elsif ( event.widget == immediateCheckBox )
		immediate = immediateCheckBox.is_checked
		selBox.set_immediate_mode( immediate )

		# immediateMode implicitly includes notify;
		# reflect this in the notify check box

		if ( immediate )
		    notifyCheckBox.set_checked( true )
		end
	    elsif ( event.widget == selBox || event.widget == valueButton )
		item = selBox.selected_item

		if ( item )
		    valueField.set_value( item.label )
		else
		    valueField.set_value( "<none>" )
	        end
	    end
        end
    end


    #
    # Clean up
    #

    dialog.destroy

