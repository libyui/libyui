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

  File:		YQGraphPluginStub.cc

  Author:	Arvin Schnell <aschnell@suse.de>

/-*/

#include "YQGraphPluginStub.h"

#define YUILogComponent "qt-ui"
#include "YUILog.h"

#define PLUGIN_BASE_NAME "qt_graph"

YQGraphPluginStub::YQGraphPluginStub()
    : YGraphPlugin( PLUGIN_BASE_NAME )
{
    if ( success() )
    {
	yuiMilestone() << "Loaded " << PLUGIN_BASE_NAME
                       << " plugin successfully from " << pluginLibFullPath()
                       << endl;
    }

    impl = (YQGraphPluginIf*) locateSymbol("GP");
    
    if ( ! impl )
    {
        yuiError() << "Plugin " << PLUGIN_BASE_NAME << " does not provide GP symbol" << endl;
    }
}


YQGraphPluginStub::~YQGraphPluginStub()
{
    // NOP
}


YGraph *
YQGraphPluginStub::createGraph( YWidget * parent, const string & filename,
				const string & layoutAlgorithm )
{
    if ( ! impl )
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );
    
    return impl->createGraph( parent, filename, layoutAlgorithm );
}


YGraph *
YQGraphPluginStub::createGraph( YWidget * parent, graph_t * graph )
{
    if ( ! impl )
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );
    
    return impl->createGraph( parent, graph );
}
