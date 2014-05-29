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
 * File:	YQGraphPlugin.cc
 * Author:	Arvin Schnell <aschnell@suse.de>
 */


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
		       << std::endl;
    }
    else
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );
}


YQGraphPlugin::~YQGraphPlugin()
{
    // NOP
}


YQGraph *
YQGraphPlugin::createGraph( YWidget * parent, const std::string & filename,
			    const std::string & layoutAlgorithm )
{
    if ( error() )
	return 0;

    YQGraph * graph = new YQGraph( parent, filename, layoutAlgorithm );

    YUI_CHECK_NEW( graph );

    return graph;
}
