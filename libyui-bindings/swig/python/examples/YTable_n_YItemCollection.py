#
#   Copyright information
#
#   Author: Angelo Naselli <anaselli@linux.it>
#
#   License
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, see <http://www.gnu.org/licenses/>.

import sys
sys.path.insert(0,'../../../build/swig/python')
import yui

factory = yui.YUI.widgetFactory()
dialog = factory.createMainDialog()

VBox = factory.createVBox(dialog)

yTableHeader = yui.YTableHeader()
yTableHeader.addColumn("package")
yTableHeader.addColumn("version")
yTableHeader.addColumn("release")
yTableHeader.addColumn("arch")

myTableMinSize = factory.createMinSize(VBox, 50, 12)
myTable = factory.createTable(myTableMinSize, yTableHeader)

myOK = factory.createPushButton(VBox, "OK")

# NOTE YItemCollection have problems
# because YItem is not disown() so it must
# be done explicitally by using this.own property
ic = yui.YItemCollection()
for pkg in range(1,10):
    name = "name-{0}".format(pkg)
    item = yui.YTableItem(name , "1.0", "1", "i585")
    item.this.own(False) # comment this to have a crash
    ic.push_back(item)

myTable.addItems(ic)

event1 = dialog.waitForEvent()

dialog.destroy()
