#
# Trivial libyui example
#

$:.unshift "../../../build/swig/ruby"

require 'test/unit'

class LoadTest < Test::Unit::TestCase
  def test_hello_world
    require 'yui'
    factory = Yui::YUI::widget_factory
    dialog = factory.create_popup_dialog
    vbox = factory.create_vbox dialog
    factory.create_label vbox, "Hello, Wörld!"
    factory.create_push_button vbox, "&OK"
    event = dialog.wait_for_event
    dialog.destroy
  end
end
