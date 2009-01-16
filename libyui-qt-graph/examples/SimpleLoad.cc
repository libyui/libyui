
// g++ -I/usr/include/YaST2/yui -lyui SimpleLoad.cc -o SimpleLoad

// Trivial example loading a graph from a file.


#include "YUI.h"
#include "YWidgetFactory.h"
#include "YOptionalWidgetFactory.h"
#include "YDialog.h"
#include "YLayoutBox.h"
#include "YEvent.h"
#include "YGraph.h"


int
main(int argc, char** argv)
{
    YDialog* dialog = YUI::widgetFactory()->createPopupDialog();
    YLayoutBox* vbox = YUI::widgetFactory()->createVBox(dialog);

    YUI::optionalWidgetFactory()->createGraph(vbox, "simple.dot", "dot");

    dialog->waitForEvent();
    dialog->destroy();
}
