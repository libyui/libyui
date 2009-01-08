#
# Table example
# taken from https://bugzilla.novell.com/show_bug.cgi?id=449842#c10
#

# ensure we're using the latest build, if called from our build environment
import sys
sys.path.insert(0,'../../../build/swig/python')

import yui

# enable logging for debug
log = yui.YUILog.instance()
log.setLogFileName("debug.log")
log.enableDebugLogging( True )

factory = yui.YUI.widgetFactory()
dialog = factory.createMainDialog()

VBox = factory.createVBox(dialog)

yTableHeader = yui.YTableHeader()
yTableHeader.addColumn("aaaaa")
yTableHeader.addColumn("bbbbb")
yTableHeader.addColumn("ccccc")
yTableHeader.addColumn("ddddd")

myTableMinSize = factory.createMinSize(VBox, 50, 12)
myTable = factory.createTable(myTableMinSize, yTableHeader)

myOK = factory.createPushButton(VBox, "OK")

# note YItemCollection seems unavailable !
myItem = yui.YTableItem("X", "foo", "1.2.1", "qwertz")
myTable.addItem(myItem)
myItem2 = yui.YTableItem("", "bar", "1.3.1", "asdasdasd")
myTable.addItem(myItem2)

event1 = dialog.waitForEvent()
print 1
myTable.deleteAllItems()
print 2  #crashes now.
del myItem
del myItem2
print 3  # if not "del" before, crashes here.
myItem = yui.YTableItem("X3", "foo3", "1.2.13", "qwertz3")
print 4
myTable.addItem(myItem)
print 5
myItem2 = yui.YTableItem("4", "bar4", "1.3.14", "asdasdasd4")
print 6
myTable.addItem(myItem2)

# should already have crashed
event2 = dialog.waitForEvent()
dialog.destroy()

