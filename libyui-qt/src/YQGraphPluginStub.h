/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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

  File:		YQGraphPluginStub.h

  Author:	Arvin Schnell <aschnell@suse.de>

/-*/

#ifndef YQGraphPluginStub_h
#define YQGraphPluginStub_h

#include <YGraphPlugin.h>
#include <YDialog.h>
#include <YEvent.h>

#include "YQGraphPluginIf.h"


/**
 * Simplified access to the package selector plugin.
 **/
class YQGraphPluginIf;


class YQGraphPluginStub : public YGraphPlugin
{
public:
    /**
     * Constructor: Load the plugin library for the package selector.
     **/
    YQGraphPluginStub();

    /**
     * Destructor.
     **/
    virtual ~YQGraphPluginStub();

    /**
     * Create a package selector.
     * Implemented from YGraphPlugin.
     *
     * This might return 0 if the plugin lib could not be loaded or if the
     * appropriate symbol could not be located in the plugin lib.
     **/
    virtual YGraph * createGraph( YWidget * parent, const string & filename,
				  const string & layoutAlgorithm );

    virtual YGraph * createGraph( YWidget * parent, graph_t * graph );

    YQGraphPluginIf * impl;
};


#endif // YQGraphPluginStub_h
