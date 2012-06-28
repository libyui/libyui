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
