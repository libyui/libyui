# coding=UTF-8
#
# Replacepoint, allows you to replace part of the widget tree
#
# Example by Jan-Simon Möller <dl9pf@gmx.de>
#

import sys
sys.path.insert(0,'../../../build/swig/python')
import yui

factory = yui.YUI.widgetFactory()
dialog = factory.createMainDialog()
replacepoint = factory.createReplacePoint(dialog)
hbox = factory.createHBox(replacepoint)
b1 = factory.createPushButton(hbox, "1")
b2 = factory.createPushButton(hbox, "2")
b3 = factory.createPushButton(hbox, "3")
event = dialog.waitForEvent()           # push button
replacepoint.deleteChildren()
vbox = factory.createVBox(replacepoint)
b1 = factory.createPushButton(vbox, "1")
b2 = factory.createPushButton(vbox, "2")
b3 = factory.createPushButton(vbox, "3")
dialog.recalcLayout()
replacepoint.showChild()
event = dialog.waitForEvent()           # tada!
dialog.destroy()
