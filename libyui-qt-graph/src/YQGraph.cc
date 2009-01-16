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
    : QGraph((QWidget*) parent->widgetRep(), filename, layoutAlgorithm),
      YGraph(parent, filename, layoutAlgorithm)
{
    setWidgetRep(this);
}


YQGraph::YQGraph(YWidget* parent, graph_t* graph)
    : QGraph((QWidget*) parent->widgetRep(), graph),
      YGraph(parent, graph)
{
    setWidgetRep(this);
}


YQGraph::~YQGraph()
{
}


void
YQGraph::renderGraph(const string& filename, const string& layoutAlgorithm)
{
    QGraph::renderGraph(filename, layoutAlgorithm);
}


void
YQGraph::renderGraph(graph_t* graph)
{
    QGraph::renderGraph(graph);
}


int
YQGraph::preferredWidth()
{
    return std::min(640, sizeHint().width());
}


int
YQGraph::preferredHeight()
{
    return std::min(480, sizeHint().height());
}


void
YQGraph::setSize(int newWidth, int newHeight)
{
    resize(newWidth, newHeight);
}


#include "YQGraph.moc"
