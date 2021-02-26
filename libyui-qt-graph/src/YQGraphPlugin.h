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

  File:		YQGraphPlugin.h

  Author:	Arvin Schnell <aschnell@suse.de>

/-*/


#ifndef YQGraphPlugin_h
#define YQGraphPlugin_h

#include <YGraphPlugin.h>
#include "YQGraph.h"


/**
 * Simplified access to the Qt UI's package selector plugin.
 **/
class YQGraphPlugin : public YGraphPlugin
{
public:

    /**
     * Constructor: Load the plugin library for the Qt package selector.
     **/
    YQGraphPlugin();

    /**
     * Destructor. Calls dlclose() which will unload the plugin library if it
     * is no longer used, i.e. if the reference count dlopen() uses reaches 0.
     **/
    virtual ~YQGraphPlugin();

    /**
     * Create a package selector.
     * Implemented from YGraphPlugin.
     *
     * This might return 0 if the plugin lib could not be loaded or if the
     * appropriate symbol could not be located in the plugin lib.
     **/
    virtual YQGraph * createGraph ( YWidget * parent, const string & filename,
				    const string & layoutAlgorithm );

};


#endif // YQGraphPlugin_h
