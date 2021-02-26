#!/usr/bin/env python
# vim: set et ts=4 sw=4:
#coding:utf-8
#############################################################################
#
# mga-dialogs.py  -  Show mga msg dialog and about dialog.
#
# License: GPLv3
# Author:  Angelo Naselli <anaselli@linux.it>
#############################################################################

###########
# imports #
###########
import sys
sys.path.insert(0,'../../../build/swig/python')
import os

import yui

log = yui.YUILog.instance()
log.setLogFileName("/tmp/debug.log")
log.enableDebugLogging( True )
appl = yui.YUI.application()
appl.setApplicationTitle("Show dialogs example")

#################
# class mainGui #
#################
class Info(object):
    def __init__(self,title,richtext,text):
        self.title=title
        self.richtext=richtext
        self.text=text

class mainGui():
    """
    Main class
    """

    def __init__(self):
        self.factory = yui.YUI.widgetFactory()
        self.dialog = self.factory.createPopupDialog()
        mainvbox = self.factory.createVBox(self.dialog)
        frame    = self.factory.createFrame(mainvbox,"Test frame")
        HBox     = self.factory.createHBox(frame)
        self.aboutbutton = self.factory.createPushButton(HBox,"&About")
        self.closebutton = self.factory.createPushButton(self.factory.createRight(HBox), "&Close")


    def ask_YesOrNo(self, info):
        yui.YUI.widgetFactory
        mgafactory = yui.YMGAWidgetFactory.getYMGAWidgetFactory(yui.YExternalWidgets.externalWidgetFactory("mga"))
        dlg = mgafactory.createDialogBox(yui.YMGAMessageBox.B_TWO)
        dlg.setTitle(info.title)
        dlg.setText(info.text, info.richtext)
        dlg.setButtonLabel("Yes", yui.YMGAMessageBox.B_ONE)
        dlg.setButtonLabel("No",  yui.YMGAMessageBox.B_TWO)
        dlg.setMinSize(50, 5);
        return dlg.show() == yui.YMGAMessageBox.B_ONE

    def aboutDialog(self):
        yui.YUI.widgetFactory;
        mgafactory = yui.YMGAWidgetFactory.getYMGAWidgetFactory(yui.YExternalWidgets.externalWidgetFactory("mga"))
        dlg = mgafactory.createAboutDialog("About dialog title example", "1.0.0", "GPLv3",
                                        "Angelo Naselli", "This beautiful test example shows how it is easy to play with libyui bindings", "")
        dlg.show();


    def handleevent(self):
        """
        Event-handler for the 'widgets' demo
        """
        while True:
            event = self.dialog.waitForEvent()
            if event.eventType() == yui.YEvent.CancelEvent:
                self.dialog.destroy()
                break
            if event.widget() == self.closebutton:
                info = Info("Quit confirmation", 1, "Are you sure you want to quit?")
                if self.ask_YesOrNo(info):
                    self.dialog.destroy()
                    break
            if event.widget() == self.aboutbutton:
                self.aboutDialog()

if __name__ == "__main__":
    main_gui = mainGui()
    main_gui.handleevent()
