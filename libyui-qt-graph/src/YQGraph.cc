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

/*
 * File:	YQGraph.cc
 * Author:	Arvin Schnell <aschnell@suse.de>
 */


#define YUILogComponent "qt-graph"
#include "YUILog.h"

#include "YQGraph.h"

#include "YQDialog.h"
#include "YQApplication.h"
#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQi18n.h"


YQGraph::YQGraph(YWidget* parent, const string& filename, const string& layoutAlgorithm)
    : QY2Graph(filename, layoutAlgorithm, (QWidget*) parent->widgetRep()),
      YGraph(parent, filename, layoutAlgorithm)
{
    setWidgetRep(this);

    init();
}


YQGraph::YQGraph(YWidget* parent, /* graph_t */ void* graph)
    : QY2Graph((graph_t*)graph, (QWidget*) parent->widgetRep()),
      YGraph(parent, (graph_t*) graph)
{
    setWidgetRep(this);

    init();
}


YQGraph::~YQGraph()
{
}


void
YQGraph::init()
{
    connect(this, SIGNAL(backgroundContextMenuEvent(QContextMenuEvent*)),
	    this, SLOT(backgroundContextMenu(QContextMenuEvent*)));

    connect(this, SIGNAL(nodeContextMenuEvent(QContextMenuEvent*, const QString&)),
	    this, SLOT(nodeContextMenu(QContextMenuEvent*, const QString&)));

    connect(this, SIGNAL(nodeDoubleClickEvent(QMouseEvent*, const QString&)),
	    this, SLOT(nodeDoubleClick(QMouseEvent*, const QString&)));
}


void
YQGraph::renderGraph(const string& filename, const string& layoutAlgorithm)
{
    QY2Graph::renderGraph(filename, layoutAlgorithm);
}


void
YQGraph::renderGraph(/* graph_t */ void* graph)
{
    QY2Graph::renderGraph((graph_t*)graph);
}


int
YQGraph::preferredWidth()
{
    return std::min(160, sizeHint().width());
}


int
YQGraph::preferredHeight()
{
    return std::min(120, sizeHint().height());
}


void
YQGraph::setSize(int newWidth, int newHeight)
{
    resize(newWidth, newHeight);
}


void
YQGraph::backgroundContextMenu(QContextMenuEvent* event)
{
    if (notifyContextMenu())
    {
	lastActivatedNode.clear();
	YQUI::yqApp()->setContextMenuPos(event->globalPos());
	YQUI::ui()->sendEvent(new YWidgetEvent(this, YEvent::ContextMenuActivated));
    }
}


void
YQGraph::nodeContextMenu(QContextMenuEvent* event, const QString& name)
{
    if (notifyContextMenu())
    {
	lastActivatedNode = name.toStdString();
	YQUI::yqApp()->setContextMenuPos(event->globalPos());
	YQUI::ui()->sendEvent(new YWidgetEvent(this, YEvent::ContextMenuActivated));
    }
}


void
YQGraph::nodeDoubleClick(QMouseEvent* event, const QString& name)
{
    if (notify())
    {
	lastActivatedNode = name.toStdString();
	YQUI::ui()->sendEvent(new YWidgetEvent(this, YEvent::Activated));
    }
}


#include "YQGraph.moc"
