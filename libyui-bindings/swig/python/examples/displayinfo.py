#
# map disp = UI::GetDisplayInfo();
#  is a YCP-only function defined in ycp-ui-bindings/src/YCP_UI.cc
#

import sys
sys.path.insert(0,'../../../build/swig/python')
import yui

appl = yui.YUI.application()
print "Width ", appl.displayWidth()
print "Height ", appl.displayHeight()
print "Depth ", appl.displayDepth()
print "Colors ", appl.displayColors()
print "DefaultWidth ", appl.defaultWidth()
print "DefaultHeight  ", appl.defaultHeight()
print "TextMode  ", appl.isTextMode()
print "HasImageSupport ", appl.hasImageSupport()
print "HasIconSupport ", appl.hasIconSupport()
print "HasAnimationSupport ", appl.hasAnimationSupport()
print "HasFullUtf8Support ", appl.hasFullUtf8Support()
print "RichTextSupportsTable ", appl.richTextSupportsTable()
print "LeftHandedMouse ", appl.leftHandedMouse()
log = yui.YUILog.instance()
print "y2debug ", log.debugLoggingEnabled()

optional = yui.YUI.optionalWidgetFactory()

print "Has wizard ", optional.hasWizard()

#factory = yui.YUI.widgetFactory()
#dialog = factory.createPopupDialog()

#vbox = factory.createVBox( dialog )
#factory.createLabel( vbox, "Hello, World!" )
#factory.createPushButton( vbox, "&OK" )
#event = dialog.waitForEvent()
#dialog.destroy()
