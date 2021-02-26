#!/usr/bin/ruby
require File.join(File.dirname(__FILE__),'_loadpath')
require 'yui'

# reopen the class to add a convenience method
module Yui
  class YSelectionBox
    def <<(item_string)
      add_item Yui::YItem.new(item_string)
    end
  end
end

class SimpleFactory
  def initialize(parent_widget = nil)
    @parent_widget = parent_widget
  end

  # YSelectionBox should really yield self,
  # but it is dumb and yields a useless SimpleFactory instead,
  # so let's forward it
  def <<(*args)
    @parent_widget.<<(*args)
  end

  METHODS = ["popup_dialog", "selection_box", "hbox", "vbox", "output_field",
             "push_button", "vspacing", "left", "right"]

  def method_missing(name, *args, &block)
    super(name, *args, &block) unless METHODS.include? name.to_s

    create_args = []
    create_args << @parent_widget unless @parent_widget.nil?
    create_args += args
    widget = Yui::YUI::widget_factory.send "create_#{name}", *create_args
    unless block.nil?
      child_factory = self.class.new(widget)
      block.call child_factory
    end
    widget
  end
end

def example
  value_field = value_button = close_button = sel_box = nil

  ui = SimpleFactory.new
  dialog = ui.popup_dialog do |w|
    w.vbox do |w|
      sel_box = w.selection_box "&Menu" do |s|
        s << "Pizza Margherita"
        s << "Pizza Capricciosa"
        s << "Pizza Funghi"
        s << "Pizza Prosciutto"
        s << "Pizza Quattro Stagioni"
        s << "Calzone"
      end
      w.hbox do |w|
        value_field = w.output_field "<SelectionBox value unknown>"
        # allow stretching over entire dialog width
        value_field.set_stretchable Yui::YD_HORIZ, true
        value_button = w.push_button "&Value"
      end
      w.vspacing 0.3
      w.right do |w|
        close_button = w.push_button "&Close"
      end
    end
  end

  loop do
    event = dialog.wait_for_event
    next unless event

    # window manager "close window" button
    break if event.event_type == Yui::YEvent::CancelEvent

    value_field.set_value "???"
    break if event.widget == close_button

    if ( event.widget == value_button ||
         event.widget == sel_box )		# sel_box will only send events with setNotify()
      item = sel_box.selected_item
      if item
        value_field.set_value item.label
      else
        value_field.set_value "<none>"
      end
    end
  end
  dialog.destroy

end

example
