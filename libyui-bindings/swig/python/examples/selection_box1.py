#
# More advanced libyui example
#

import sys
sys.path.insert(0, '../../../build/swig/python')
import yui

factory = yui.YUI.widgetFactory();

yui.YUILog.setLogFileName( "/tmp/libyui-examples.log" )
yui.YUILog.enableDebugLogging()

dialog = factory.createPopupDialog()
vbox = factory.createVBox( dialog )
selBox = factory.createSelectionBox( vbox, "&Menu" )

selBox.addItem( "Pizza Margherita" )
selBox.addItem( "Pizza Capricciosa" )
selBox.addItem( "Pizza Funghi" )
selBox.addItem( "Pizza Prosciutto" )
selBox.addItem( "Pizza Quattro Stagioni" )
selBox.addItem( "Calzone" )

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
  event = dialog.waitForEvent()
  if not event:
    print "Empty"
    next
  if event.eventType() == yui.YEvent.CancelEvent: # window manager "close window" button
    break
  valueField.setValue( "???" )
  if event.widget() == closeButton:
    break

  if ( event.widget() == valueButton or event.widget() == selBox ):		# selBox will only send events with setNotify()
    item = selBox.selectedItem()
    if item:
      valueField.setValue( item.label() )
    else:
      valueField.setValue( "<none>" )

dialog.destroy()
