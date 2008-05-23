#
# More advanced libyui example
#

import sys
sys.path.append('../../../build/bindings/python')
import yui

factory = yui.YUI.widgetFactory();

yui.YUILog.setLogFileName( "/tmp/libyui-examples.log" )
yui.YUILog.enableDebugLogging()

dialog = factory.createPopupDialog()
vbox = factory.createVBox( dialog )
selBox = factory.createSelectionBox( vbox, "&Menu" )

items = [] #Yui.YItemCollection.new
i1 = yui.YItem( "Pizza Margherita" )
selBox.addItem( i1 )
i2 = yui.YItem( "Pizza Capricciosa" )
selBox.addItem( i2 )
i3 = yui.YItem( "Pizza Funghi" )
selBox.addItem( i3 )
i4 = yui.YItem( "Pizza Prosciutto" )
selBox.addItem( i4 )
i5 = yui.YItem( "Pizza Quattro Stagioni" )
selBox.addItem( i5 )
i6 = yui.YItem( "Calzone" )
selBox.addItem( i6 )
#selBox.addItems items # This is more efficient than repeatedly calling selBox.addItem

hbox = factory.createHBox( vbox )
valueField  = factory.createOutputField( hbox, "<SelectionBox value unknown>" )
valueField.setStretchable( yui.YD_HORIZ, True ) # // allow stretching over entire dialog width

valueButton = factory.createPushButton( hbox, "&Value" ) 
factory.createVSpacing( vbox, 0.3 )

rightAlignment = factory.createRight( vbox )
closeButton    = factory.createPushButton( rightAlignment, "&Close" )

#
# Event loop
#

while True:
  print "Begin of loop"
  event = dialog.waitForEvent()
  print "Event "
  print event
  print "---"
  if not event:
    print "Empty"
    next
  print "Type"
  print event.eventType()
  print "---"
  print "CancelType"
  print yui.YEvent.CancelEvent
  print "---"
  if event.eventType() == yui.YEvent.CancelEvent: # window manager "close window" button
    print "Cancel"
    break
  print "Not a cancel"
  valueField.setValue( "???" )
  if event.widget() == closeButton:
    print "Close"
    break
  print "Not a close"

  if ( event.widget() == valueButton or event.widget() == selBox ):		# selBox will only send events with setNotify()
    item = selBox.selectedItem()
    print "selBox or valueButton"
    if item:
      valueField.setValue( item.label )
    else:
      valueField.setValue( "<none>" )

    print "valueField set"

print "Loop end"
