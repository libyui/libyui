/*
 * Copyright (c) [2009-2012] Novell, Inc.
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
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
