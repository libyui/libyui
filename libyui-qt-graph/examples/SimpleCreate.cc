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

#ifdef WITH_CGRAPH
    graph_t* graph = agopen(strdup("test"), Agdirected, 0);

    node_t* node1 = agnode(graph, strdup("1"), 1);
    node_t* node2 = agnode(graph, strdup("2"), 1);

    agattr(graph, AGNODE, strdup("fillcolor"), strdup("gray"));
    agset(node1, strdup("fillcolor"), strdup("red"));
    agset(node2, strdup("fillcolor"), strdup("green"));

    edge_t* edge1 = agedge(graph, node1, node2, NULL, 1);
    edge_t* edge2 = agedge(graph, node2, node1, NULL, 1);

    agattr(graph, AGEDGE, strdup("color"), strdup("black"));
    agset(edge1, strdup("color"), strdup("red"));
    agset(edge2, strdup("color"), strdup("green"));
#else
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
#endif

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
