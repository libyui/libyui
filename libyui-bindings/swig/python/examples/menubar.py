# vim: set et ts=4 sw=4:
#coding:utf-8
#############################################################################
#
# menubar.py  -  Show a dialog with YMenuBar implementation
#
# License: GPLv2+
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
log.setLogFileName("/tmp/menubar.log")
log.enableDebugLogging( True )
appl = yui.YUI.application()
appl.setApplicationTitle("Show YMenuBar example")

#################
# class mainGui #
#################

class mainGui():
    """
    Main class
    """

    def __init__(self):
        self.factory = yui.YUI.widgetFactory()
        self.dialog = self.factory.createPopupDialog()
        minSize = self.factory.createMinSize(self.dialog, 50, 20 )
        mainvbox = self.factory.createVBox(minSize)
        self.menubar = self.factory.createMenuBar(mainvbox)

        #Items must be "disowned"
        fileMenu = self.menubar.addMenu("&File")
        fileMenu.this.own(False)
        tmi = yui.YMenuItem(fileMenu, "&New")
        tmi.this.own(False)
        item = yui.YMenuItem( tmi, "New &1" , "document-new")
        item.this.own(False)
        item = yui.YMenuItem( tmi, "New &2" , "contact-new")
        item.this.own(False)
        item = fileMenu.addSeparator()
        item.this.own(False)
        item = yui.YMenuItem(fileMenu, "&Open", "document-open.png")
        item.this.own(False)
        item = fileMenu.addSeparator()
        item.this.own(False)
        item = yui.YMenuItem(fileMenu, "&Save", "document-save.png")
        item.this.own(False)
        item = yui.YMenuItem(fileMenu, "&Save as", "document-save-as")
        item.this.own(False)
        item = fileMenu.addSeparator()
        item.this.own(False)
        self.quitMenu = yui.YMenuItem(fileMenu, "&Quit", "application-exit")
        self.quitMenu.this.own(False)

        editmenu = self.menubar.addMenu("&Edit")
        self.editMenu = {
            'menu'      : editmenu,
            'undo'      : editmenu.addItem("&Undo", "edit-undo.png"),
            'redo'      : editmenu.addItem("&Redo", "edit-redo.png"),
            'sep0'      : editmenu.addSeparator(),
            'cut'       : editmenu.addItem("Cu&t", "edit-cut.png"),
            'copy'      : editmenu.addItem("&Copy", "edit-copy.png"),
            'paste'     : yui.YMenuItem(editmenu, "&Paste", "edit-paste.png"),
        }
        #Items must be "disowned"
        for k in self.editMenu.keys():
            self.editMenu[k].this.own(False)

        self.menubar.resolveShortcutConflicts()
        self.menubar.rebuildMenuTree()

        HBox     = self.factory.createHBox(mainvbox)
        self.closebutton = self.factory.createPushButton(self.factory.createRight(HBox), "&Close")


    def handleevent(self):
        """
        Event-handler for the 'widgets' demo
        """
        while True:
            event = self.dialog.waitForEvent()
            eventType = event.eventType()
            if eventType == yui.YEvent.CancelEvent:
                break
            elif (eventType == yui.YEvent.MenuEvent) :
                item = event.item()
                if (item) :
                    if  item == self.quitMenu :
                        break
            elif (eventType == yui.YEvent.WidgetEvent) :
                # widget selected
                widget  = event.widget()
                if (widget == self.closebutton) :
                    break

        self.dialog.destroy()

if __name__ == "__main__":
    main_gui = mainGui()
    main_gui.handleevent()

    yui.YDialog.deleteAllDialogs()
    # next line seems to be a workaround to prevent the qt-app from crashing
    # see https://github.com/libyui/libyui-qt/issues/41
    yui.YUILoader.deleteUI()

