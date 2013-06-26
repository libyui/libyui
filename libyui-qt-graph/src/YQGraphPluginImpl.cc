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
 * File:	YQGraphPluginImpl.cc
 * Author:	Arvin Schnell <aschnell@suse.de>
 */


#include "YQGraphPluginImpl.h"
#include "YQGraph.h"

#define YUILogComponent "qt-graph"
#include <YUILog.h>

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YQGraphPluginStub::createGraph
//	METHOD TYPE : YWidget
//
//	DESCRIPTION : Create YQGraphStart which reads the layout
//                    term of the package selection dialog, creates the widget
//		      tree and creates the YQGraph.
//
extern "C"
{
    YQGraphPluginImpl GP;
}

YGraph * YQGraphPluginImpl::createGraph( YWidget * parent, const std::string & filename,
					 const std::string & layoutAlgorithm )
{
    /* if ( error() )
       return 0; */

    YQGraph * yqgraph = new YQGraph( parent, filename, layoutAlgorithm );

    YUI_CHECK_NEW( yqgraph );

    return yqgraph;
}


YGraph * YQGraphPluginImpl::createGraph( YWidget * parent, /* graph_t */ void * graph )
{
    /* if ( error() )
       return 0; */

    YQGraph * yqgraph = new YQGraph( parent, graph );

    YUI_CHECK_NEW( yqgraph );

    return yqgraph;
}
