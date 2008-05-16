#
# Trivial libyui example
#

import sys
sys.path.append('../../../build/bindings/python')
import yui

dialog = yui.YWidgetFactory.createPopupDialog()
vbox = yui.YWidgetFactory_createVBox( dialog )
yui.YWidgetFactory_createLabel( vbox, "Hello, World!" )
yui.YWidgetFactory_createPushButton( vbox, "&OK" )
event = dialog.waitForEvent()
dialog.destroy()
