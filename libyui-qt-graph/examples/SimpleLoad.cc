/*
 * Copyright (C) 2009-2012 Novell, Inc
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) version 3.0 of the License. This library
 * is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details. You should have received a copy of the GNU
 * Lesser General Public License along with this library; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

// g++ SimpleLoad.cc -o SimpleLoad -Wall -O2 -lyui

// Trivial example loading a graph from a file.


#include <iostream>

#include "yui/YUI.h"
#include "yui/YWidgetFactory.h"
#include "yui/YOptionalWidgetFactory.h"
#include "yui/YDialog.h"
#include "yui/YLayoutBox.h"
#include "yui/YGraph.h"
#include "yui/YPushButton.h"
#include "yui/YEvent.h"


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
