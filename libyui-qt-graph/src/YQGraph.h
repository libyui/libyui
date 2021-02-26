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

  File:		YQGraph.h

  Author:	Arvin Schnell <aschnell@suse.de>

/-*/


#ifndef YQGraph_h
#define YQGraph_h

#include <YGraph.h>

#include "QY2Graph.h"


class YQGraph : public QY2Graph, public YGraph
{
    Q_OBJECT

public:

    YQGraph(YWidget* parent, const string& filename, const string& layoutAlgorithm);

    YQGraph(YWidget* parent, graph_t* graph);

    virtual ~YQGraph();

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize(int newWidth, int newHeight);

    void renderGraph(const string& filename, const string& layoutAlgorithm);

    void renderGraph(graph_t* graph);

    string activatedNode() const { return lastActivatedNode; }

private slots:

    void backgroundContextMenu(QContextMenuEvent* event);
    void nodeContextMenu(QContextMenuEvent* event, const QString& name);
    void nodeDoubleClick(QMouseEvent* event, const QString& name);

private:

    void init();

    string lastActivatedNode;

};


#endif // YQGraph_h
