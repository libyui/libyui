
// g++ -I/usr/include/YaST2/yui -lyui SimpleLoad.cc -o SimpleLoad

// Trivial example loading a graph from a file.


#include <iostream>

#include "YUI.h"
#include "YWidgetFactory.h"
#include "YOptionalWidgetFactory.h"
#include "YDialog.h"
#include "YLayoutBox.h"
#include "YGraph.h"
#include "YPushButton.h"
#include "YEvent.h"


int
main(int argc, char** argv)
{
    if (argc != 3)
    {
	std::cerr << argv[0] << " filename layout" << std::endl;
	return 1;
    }

    YDialog* dialog = YUI::widgetFactory()->createPopupDialog();
    YLayoutBox* vbox = YUI::widgetFactory()->createVBox(dialog);

    YUI::optionalWidgetFactory()->createGraph(vbox, argv[1], argv[2]);
    YPushButton* button = YUI::widgetFactory()->createPushButton(vbox, "&Close");

    while (true)
    {
	YEvent* event = dialog->waitForEvent();

	if (event->eventType() == YEvent::CancelEvent)
	    break;

	if (event->widget() == button)
	    break;
    }

    dialog->destroy();
}
