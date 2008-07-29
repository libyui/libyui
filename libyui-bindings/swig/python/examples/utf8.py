#coding:utf-8
#
# Trivial libyui example
#

text = "Hello,Wörld!"

import sys
sys.path.insert(0,'../../../build/bindings/python')
import yui

log = yui.YUILog.instance()
log.setLogFileName("debug.log")
log.enableDebugLogging( True )

appl = yui.YUI.application()
appl.setLanguage( "de", "UTF-8" )
#appl.setConsoleFont(magic, font, screenMap, unicodeMap, language)
# see /usr/share/YaST2/data/consolefonts.ycp
appl.setConsoleFont("(K", "lat9w-16.psfu", "trivial", "", "en_US.UTF-8")
factory = yui.YUI.widgetFactory()

print "Lang: ", appl.language()
dialog = factory.createPopupDialog()

vbox = factory.createVBox( dialog )
factory.createLabel( vbox, text )
factory.createPushButton( vbox, "&OK" )
event = dialog.waitForEvent()
dialog.destroy()
# BEISPIEL 1 geht im graphischen gut, aber nicht auf der konsole ... 
