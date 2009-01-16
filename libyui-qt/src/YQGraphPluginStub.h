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
