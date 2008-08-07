#coding:utf-8
#############################################################################
#
# widgets.py  -  Demonstration of widgets available in python-libyui
#
# License: GPLv2
# Author:  Jan-Simon Möller, dl9pf@gmx.de
#############################################################################

###########
# imports #
###########
import yui
import locale


####################################
# LOCALE (important for TERMINAL!) #
####################################
# set the locale to $ENV
locale.setlocale(locale.LC_ALL, "")

#################
# class widgets #
#################
class WIDGETS(object):
    """
    Main class for the 'widgets' demo
    """
    
    def __init__(self, myavwidgets):
        """
        Init/Constructor for the 'widgets' demo
        """
        self.factory = yui.YUI.widgetFactory()
        self.dialog = self.factory.createMainDialog()
        self.avwidgets = myavwidgets.copy()
        # create the main gui
        # +---+-----------+
        # | s | display   |
        # | e +-----------+
        # | l |description|
        # | b +-----------+
        # | x |  code     |
        # +---+-----------+
        self.mainhbox = self.factory.createHBox(self.dialog)
        self.selbox = self.factory.createSelectionBox(self.mainhbox, "Widgets")
        self.selbox.setWeight(0,20)
        self.selbox.setNotify()
        self.boxright = self.factory.createVBox(self.mainhbox)
        self.boxright.setWeight(0,80)
        self.framedisplay = self.factory.createFrame(self.boxright, "View")
        self.framedisplay.setWeight(1,33)
        self.framedisplay.setStretchable(0,True)
        self.display = self.factory.createReplacePoint(self.framedisplay) # here we change the widget
        self.displaychild_ = {}
        self.framedescription = self.factory.createFrame(self.boxright, "Description")
        self.framedescription.setWeight(1,33)
        self.framedescription.setStretchable(0,True)
        self.description = self.factory.createReplacePoint(self.framedescription) # here we change the widget
        self.descriptionchild_ = {}
        self.framecode = self.factory.createFrame(self.boxright, "Code")
        self.framecode.setWeight(1,33)
        self.framecode.setStretchable(0,True)
        self.code = self.factory.createReplacePoint(self.framecode) # here we change the widget
        self.codechild_ = {}
        self.updateselbox() # import available widgets  into display
        self.updatedisplay()
        self.updatedescription()
        self.updatecode()
    
    def updateselbox(self):
        for i in self.avwidgets.keys():
            self.selbox.addItem(i)

    def updatedisplay(self):
        self.display.deleteChildren() # remove old widgets
        selected = self.selbox.selectedItem().label()
        #self.displaychild_[selected] = self.factory.createPushButton(self.display, "&OK") #self.avwidgets[selected][0]
        #print type(self.displaychild_[selected])
        widgettodisplay  = "self.displaychild_[selected] = self.factory."+self.avwidgets[selected][0]
        exec widgettodisplay 
        if self.avwidgets[selected][1]:
            widgettodisplay1 = "self.displaychild_[selected]"+self.avwidgets[selected][1]
            exec widgettodisplay1
        if self.avwidgets[selected][2]:
            widgettodisplay2 = "self.displaychild_[selected]"+self.avwidgets[selected][2]
            exec widgettodisplay2
        self.dialog.recalcLayout()
        self.display.showChild()
    
    def updatedescription(self):
        self.description.deleteChildren()
        selected = self.selbox.selectedItem().label()
        text = self.avwidgets[selected][3]
        self.descriptionchild_[selected] = self.factory.createRichText(self.description, str(text))
        #exec widgettodescribe
        self.dialog.recalcLayout()
        self.description.showChild()

    def updatecode(self):
        self.code.deleteChildren()
        selected = self.selbox.selectedItem().label()
        text = self.avwidgets[selected][4]
        print text
        self.codechild_[selected] = self.factory.createRichText(self.code, str(text))
        self.dialog.recalcLayout()
        self.code.showChild()
        
    
    def handleevent(self):
        """
        Event-handler for the 'widgets' demo
        """
        while True:
            event = self.dialog.waitForEvent()
            if event.eventType() == yui.YEvent.CancelEvent:
                self.dialog.destroy()
                break
            if event.widget() == self.selbox:
                self.updatedisplay()
                self.updatedescription()
                self.updatecode()

if __name__ == "__main__":
    avwidgets = {}
    avwidgets["PushButton"]=['createPushButton(self.display, "&OK")', 
                             None, 
                             None, 
                             """This Widget is a Button with a name and a configurable shortcut""",
                             """Code:<br>myPushButton = factory.createPushButton(parentWidget, Name) <br>e.g. myPushButton = f.createPushButton(myHBox, "&OK")"""] 
    avwidgets["ComboBox"]  =['createComboBox(self.display, "Combobox")',
                             '.addItem("Item1")' ,
                             '.addItem("Item2")' ,
                             """This Widget is a Combobox with 1 or more items.""",
                             """Code: <br>
                                myComboBox = factory.createComboBox(parentWidget, "Name") <br>
                                myComboBox.addItem("Item") <br>
                                
                                Event: <br>
                                if event.widget == myComboBox: <br>
                                   dosomething() """]
    avwidgets["InputField"]=['createInputField(self.display, "Inputfield")',
                             '.setValue("Input nonsense here")',
                             None,
                             """This Widget is an InputField for User-Input""",
                             """Code:<br>
                             myInputField = factory.createInputField(parentWidget, "Name") <br>
                             myInputField.setValue("Insert valid input here") <br>
                             myInputField.setValidChars("abcdefghijklmnopqrstuvwxyz") <br>
                             Event: <br>
                             if event.widget = myInputField: <br>
                                 value = myInputField.value()
                             """]
    MY_MAIN_GUI = WIDGETS(avwidgets)
    MY_MAIN_GUI.handleevent()  