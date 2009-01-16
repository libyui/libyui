#
# Trivial libyui example
#

import sys
sys.path.insert(0,'../../../build/swig/python')
import yui

factory = yui.YUI.widgetFactory()
dialog = factory.createMainDialog()
hbox = factory.createHBox(dialog)
menu1 = factory.createMenuButton(hbox, "File")
print "menu1", menu1
item1 = yui.YMenuItem("open")
print "item1", item1
menu1.addItem(item1)
menu1.rebuildMenuTree()
event = dialog.waitForEvent()           # push button
dialog.destroy()
