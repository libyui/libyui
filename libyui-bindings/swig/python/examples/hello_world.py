#
# Trivial libyui example
#

import sys
sys.path.append('../../../build/bindings/python')
import yui

factory = yui.YUI.widgetFactory();
dialog = factory.createPopupDialog();
vbox = factory.createVBox( dialog )
factory.createLabel( vbox, "Hello, World!" )
factory.createPushButton( vbox, "&OK" )
event = dialog.waitForEvent()
dialog.destroy()
