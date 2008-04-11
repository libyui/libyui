#
# Trivial libyui example
#

$:.unshift "../../../build/swig/ruby"

require 'test/unit'

class LoadTest < Test::Unit::TestCase
  def test_hello_world
    require 'yui'
    dialog = Yui::YUI::widget_factory.create_popup_dialog
    vbox = Yui::YUI::widget_factory.create_vbox dialog
    Yui::YUI::widget_factory.create_label vbox, "Hello, World!"
    Yui::YUI::widget_factory.create_push_button vbox, "&OK"
    event = dialog.wait_for_event
    dialog.destroy
  end
end
