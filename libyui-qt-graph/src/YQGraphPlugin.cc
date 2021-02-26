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

  File:		YQGraphPlugin.cc

  Author:	Arvin Schnell <aschnell@suse.de>

  Textdomain	"qt-graph"

/-*/

#include "YQGraphPlugin.h"
#define YUILogComponent "qt-ui"
#include "YUILog.h"
#include "YQGraph.h"
#include "YUIException.h"

#define PLUGIN_BASE_NAME "qt_graph"


YQGraphPlugin::YQGraphPlugin()
    : YGraphPlugin( PLUGIN_BASE_NAME )
{
    if ( success() )
    {
	yuiMilestone() << "Loaded " << PLUGIN_BASE_NAME
		       << " plugin successfully from " << pluginLibFullPath()
		       << endl;
    }
    else
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );
}


YQGraphPlugin::~YQGraphPlugin()
{
    // NOP
}


YQGraph *
YQGraphPlugin::createGraph( YWidget * parent, const string & filename,
			    const string & layoutAlgorithm )
{
    if ( error() )
	return 0;

    YQGraph * graph = new YQGraph( parent, filename, layoutAlgorithm );

    YUI_CHECK_NEW( graph );

    return graph;
}
