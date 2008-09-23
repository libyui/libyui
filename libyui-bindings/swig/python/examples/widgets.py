#coding:utf-8
#############################################################################
#
# widgets.py  -  Demonstration of widgets available in python-libyui
#
# License: GPLv2
# Author:  Jan-Simon Möller, dl9pf@gmx.de
#############################################################################

# ensure we're using the latest build, if called from our build environment
import sys
sys.path.insert(0,'../../../build/swig/python')

###########
# imports #
###########
import yui
import locale


####################################
# LOCALE (important for TERMINAL!) #
####################################
# set the locale to de/utf-8
locale.setlocale(locale.LC_ALL, "")
log = yui.YUILog.instance()
log.setLogFileName("debug.log")
log.enableDebugLogging( True )
appl = yui.YUI.application()
appl.setLanguage( "de", "UTF-8" )
#appl.setConsoleFont(magic, font, screenMap, unicodeMap, language)
# see /usr/share/YaST2/data/consolefonts.ycp
appl.setConsoleFont("(K", "lat9w-16.psfu", "trivial", "", "en_US.UTF-8")


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
        self.mainvbox = self.factory.createVBox(self.mainhbox)
        self.mainvbox.setWeight(0,20)
        self.selbox = self.factory.createSelectionBox(self.mainvbox, "Widgets")
        self.selbox.setNotify()
        self.closebutton = self.factory.createPushButton(self.mainvbox, "&Close")
        self.boxright = self.factory.createVBox(self.mainhbox)
        self.boxright.setWeight(0,80)
        self.framedisplay = self.factory.createFrame(self.boxright, "View")
        self.framedisplay.setWeight(1,33)
        self.framedisplay.setStretchable(0,True)
	self.framedisplayminsize = self.factory.createMinSize(self.framedisplay, 5, 5)
        self.display = self.factory.createReplacePoint(self.framedisplayminsize) # here we change the widget
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
            if event.widget() == self.closebutton:
                self.dialog.destroy()
                break
            if event.widget() == self.selbox:
                self.dialog.startMultipleChanges()
                self.updatedisplay()
                self.updatedescription()
                self.updatecode()
                self.dialog.doneMultipleChanges()

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
                                if event.widget() == myComboBox: <br>
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
                             if event.widget() = myInputField: <br>
                                 value = myInputField.value()
                             """]
    avwidgets["CheckBox"]  =['createCheckBox(self.display, "Checkbox")',
                             '.setChecked(True)',
                             None,
                             """This Widget is a Checkbox""",
                             """Code:<br>
                             myCheckBox = fatory.createCheckbox(parentWidget, "Name") <br>
                             myCheckbox.setEnabled(True) <br>
                             Event: <br>
                             if event.widget() == myCheckbox: <br>
                               if myCheckbox.isChecked(): <br>
                                 print "Box is checked"
                            """]
    avwidgets["Frame"]    =['createFrame(self.display, "Frame")',
                            ".setStretchable(0,True)",
                            ".setStretchable(1,True)",
                            """This Widget is a Frame. It can hold other widgets (vbox,hbox,single widget).""",
                            """Code:<br>
                            myFrame = factory.createFrame(parentWidget, "Name") <br>
                            """]
    avwidgets["Label"]    =['createLabel(self.display, "Label")',
                            None,
                            None,
                            """This Widget is a Label""",
                            """Code: <br>
                            myLabel = factory.createLabel(parentWidget, "LabelText") <br>
                            """]
    avwidgets["LogView"]  =['createLogView(self.display, "LogView", 10, 10)',
                            '.appendLines("Logtext1  ")',
                            '.appendLines("Logtext2  ")',
                            """This Widget is a Log-window.""",
                            """Code:<br>
                            myLogView = factory.createLogView(parentWidget, "Name", nrLinesShown, nrLinesCached)<br>
                            myLogView.appendLines("Logtext1")
                            """] # can't use \n in Logtext1 ... need to check
    avwidgets["ProgressBar"]=['createProgressBar(self.display, "ProgressBar", 100)',
                              '.setValue(10)',
                              None,
                              """This Widget is a ProgressBar.""",
                              """Code:<br>
                              myProgressBar = factory.createProgressBar(parentWidget, "Name", maxpercentvalue) <br>
                              e.g.: <br>
                              myProgressBar = factory.createProgressBar(dialog, "Progress", 100") <br>
                              myProgressBar.setValue(33)
                              """]
    avwidgets["SelectionBox"]=['createSelectionBox(self.display, "Selection")',
                               '.addItem("SELBOX_item1")',
                               '.addItem("SELBOX_item2")',
                               """This Widget is a SelectionBox""",
                               """Code:<br>
                               mySelectionBox = factory.createSelectionBox(parentWidget, "Name") <br>
                               mySelectionBox.addItem("Item1") <br>
                               Event:<br>
                               if event.widget() = mySelectionBox: <br>
                                 selected = mySelectionBox.selectedItem()
                               """]
    MY_MAIN_GUI = WIDGETS(avwidgets)
    MY_MAIN_GUI.handleevent()

