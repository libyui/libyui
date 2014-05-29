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
