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
 * File:	YQGraph.h
 * Author:	Arvin Schnell <aschnell@suse.de>
 */


#ifndef YQGraph_h
#define YQGraph_h

#include <YGraph.h>

#include "QY2Graph.h"


class YQGraph : public QY2Graph, public YGraph
{
    Q_OBJECT

public:

    YQGraph(YWidget* parent, const std::string& filename, const std::string& layoutAlgorithm);

    YQGraph(YWidget* parent, /* graph_t */ void* graph);

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

    void renderGraph(const std::string& filename, const std::string& layoutAlgorithm);

    void renderGraph(/* graph_t */ void* graph);

    std::string activatedNode() const { return lastActivatedNode; }

private slots:

    void backgroundContextMenu(QContextMenuEvent* event);
    void nodeContextMenu(QContextMenuEvent* event, const QString& name);
    void nodeDoubleClick(QMouseEvent* event, const QString& name);

private:

    void init();

    std::string lastActivatedNode;

};


#endif // YQGraph_h
