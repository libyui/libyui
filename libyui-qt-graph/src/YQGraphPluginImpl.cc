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

  File:		YQGraphPluginImpl.cc

  Author:	Arvin Schnell <aschnell@suse.de>

/-*/

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

YGraph * YQGraphPluginImpl::createGraph( YWidget * parent, const string & filename,
					 const string & layoutAlgorithm )
{
    /* if ( error() )
       return 0; */

    YQGraph * yqgraph = new YQGraph( parent, filename, layoutAlgorithm );

    YUI_CHECK_NEW( yqgraph );

    return yqgraph;
}


YGraph * YQGraphPluginImpl::createGraph( YWidget * parent, graph_t * graph )
{
    /* if ( error() )
       return 0; */

    YQGraph * yqgraph = new YQGraph( parent, graph );

    YUI_CHECK_NEW( yqgraph );

    return yqgraph;
}
