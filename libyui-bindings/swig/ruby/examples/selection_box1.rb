#
# Trivial libyui example
#

require File.join(File.dirname(__FILE__),'_loadpath')

require 'test/unit'

class LoadTest < Test::Unit::TestCase
  def test_selection_box1
    require 'yui'

    Yui::YUILog::set_log_file_name "/tmp/libyui-examples.log"
    Yui::YUILog::enable_debug_logging
    
    dialog = Yui::YUI::widget_factory.create_popup_dialog
    vbox = Yui::YUI::widget_factory.create_vbox dialog
    selBox = Yui::YUI::widget_factory.create_selection_box vbox, "&Menu"
    
#    items = [] #Yui::YItemCollection.new
    selBox.add_item( Yui::YItem.new( "Pizza Margherita" ) )
    selBox.add_item( Yui::YItem.new( "Pizza Capricciosa" ) )
    selBox.add_item( Yui::YItem.new( "Pizza Funghi" ) )
    selBox.add_item( Yui::YItem.new( "Pizza Prosciutto" ) )
    selBox.add_item( Yui::YItem.new( "Pizza Quattro Stagioni" ) )
    selBox.add_item( Yui::YItem.new( "Calzone" ) )
    #selBox.add_items items # This is more efficient than repeatedly calling selBox.add_item
    
    hbox = Yui::YUI::widget_factory.create_hbox vbox
    valueField  = Yui::YUI::widget_factory.create_output_field hbox, "<SelectionBox value unknown>"
    valueField.set_stretchable Yui::YD_HORIZ, true # // allow stretching over entire dialog width
    
    valueButton = Yui::YUI::widget_factory.create_push_button hbox, "&Value"
    
    Yui::YUI::widget_factory.create_vspacing vbox, 0.3

    rightAlignment = Yui::YUI::widget_factory.create_right vbox
    closeButton    = Yui::YUI::widget_factory.create_push_button rightAlignment, "&Close"
    
    #
    # Event loop
    #

    loop do
      event = dialog.wait_for_event
      next unless event

      break if event.event_type == Yui::YEvent::CancelEvent # window manager "close window" button

      valueField.set_value "???"
      break if event.widget == closeButton

      if ( event.widget == valueButton ||
	  event.widget == selBox )		# selBox will only send events with setNotify()
	item = selBox.selected_item
	if item
	  valueField.set_value item.label
	else
	  valueField.set_value "<none>"
	end
      end
    end
  end
end



