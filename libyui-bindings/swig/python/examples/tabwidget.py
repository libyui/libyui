
#!/usr/bin/python
#
# tabwidget.py
# Minimalistic example for tab widget
# (http://forgeftp.novell.com//yast/doc/SL11.0/tdg/DumbTab_widget.html)

import sys
sys.path.insert(0,'../../../build/swig/python')
import yui


#if ( ! UI::HasSpecialWidget(`DumbTab ) )
#  UI::OpenDialog(
#		   `VBox( 
#			 `Label("Error: This UI doesn't support the DumbTab widget!"),
#			 `PushButton(`opt(`default), "&OK")
#			 )
#		   );
#	UI::UserInput();
#	UI::CloseDialog();
#	
#	return;
#    }

    
factory = yui.YUI.widgetFactory()
optional = yui.YUI.optionalWidgetFactory()
dialog = factory.createPopupDialog()

vbox = factory.createVBox( dialog )
if optional.hasDumbTab():
  dumptab = optional.createDumbTab( vbox )
  dumptab.addItem( yui.YItem("Page 1") )
  dumptab.addItem( yui.YItem("Page 2") )
  dumptab.addItem( yui.YItem("Page 3") )
else:
  factory.createLabel(vbox, "Error: This UI doesn't support the DumbTab widget!")

factory.createPushButton( vbox, "&Close" )
event = dialog.waitForEvent()
dialog.destroy()

#		   `VBox(
#			 `DumbTab(
#				  [ "Page 1", "Page 2", "Page 3" ],
#				  `RichText(`id(`contents), "Contents" )
#				  ),
#			 `Right(`PushButton(`id(`close), "&Close" ) )
#			 )
#		   );
