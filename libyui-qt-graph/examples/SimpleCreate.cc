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

// g++ SimpleCreate.cc -o SimpleCreate -Wall -O2 -lyui -lgvc -lgraph -lcdt

// Trivial example creating a graph using graphviz functions.


#include <string.h>
#include <graphviz/gvc.h>

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
    YDialog* dialog = YUI::widgetFactory()->createPopupDialog();
    YLayoutBox* vbox = YUI::widgetFactory()->createVBox(dialog);

    GVC_t* gvc = gvContext();

    graph_t* graph = agopen(strdup("test"), AGDIGRAPH);

    node_t* node1 = agnode(graph, strdup("1"));
    node_t* node2 = agnode(graph, strdup("2"));

    agnodeattr(graph, strdup("fillcolor"), strdup("gray"));
    agset(node1, strdup("fillcolor"), strdup("red"));
    agset(node2, strdup("fillcolor"), strdup("green"));

    edge_t* edge1 = agedge(graph, node1, node2);
    edge_t* edge2 = agedge(graph, node2, node1);

    agedgeattr(graph, strdup("color"), strdup("black"));
    agset(edge1, strdup("color"), strdup("red"));
    agset(edge2, strdup("color"), strdup("green"));

    gvLayout(gvc, graph, "dot");

    YUI::optionalWidgetFactory()->createGraph(vbox, graph);
    YPushButton* button = YUI::widgetFactory()->createPushButton(vbox, "&Close");

    gvFreeLayout(gvc, graph);
    agclose(graph);
    gvFreeContext(gvc);

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
