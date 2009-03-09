/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|						   (c) SuSE Linux GmbH |
\----------------------------------------------------------------------/

  File:		YQGraph.cc

  Author:	Arvin Schnell <aschnell@suse.de>

  Textdomain	"qt-graph"

/-*/


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


YQGraph::YQGraph(YWidget* parent, graph_t* graph)
    : QY2Graph(graph, (QWidget*) parent->widgetRep()),
      YGraph(parent, graph)
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
    connect(this, SIGNAL(backgroundContextMenuEvent(const QPoint&)),
	    this, SLOT(backgroundContextMenu(const QPoint&)));

    connect(this, SIGNAL(nodeContextMenuEvent(const QPoint&, const QString&)),
	    this, SLOT(nodeContextMenu(const QPoint&, const QString&)));

    connect(this, SIGNAL(nodeDoubleClickEvent(const QString&)),
	    this, SLOT(nodeDoubleClick(const QString&)));
}


void
YQGraph::renderGraph(const string& filename, const string& layoutAlgorithm)
{
    QY2Graph::renderGraph(filename, layoutAlgorithm);
}


void
YQGraph::renderGraph(graph_t* graph)
{
    QY2Graph::renderGraph(graph);
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
YQGraph::backgroundContextMenu(const QPoint& pos)
{
    if (contextMenu())
    {
	lastActivatedNode.clear();
	YQUI::yqApp()->setContextMenuPos(viewport()->mapToGlobal(pos));
	YQUI::ui()->sendEvent(new YWidgetEvent(this, YEvent::ContextMenuActivated));
    }
}

void
YQGraph::nodeContextMenu(const QPoint& pos, const QString& name)
{
    if (contextMenu())
    {
	lastActivatedNode = name.toStdString();
	YQUI::yqApp()->setContextMenuPos(viewport()->mapToGlobal(pos));
	YQUI::ui()->sendEvent(new YWidgetEvent(this, YEvent::ContextMenuActivated));
    }
}


void
YQGraph::nodeDoubleClick(const QString& name)
{
    lastActivatedNode = name.toStdString();
    YQUI::ui()->sendEvent(new YWidgetEvent(this, YEvent::Activated));
}


#include "YQGraph.moc"
