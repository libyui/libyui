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
