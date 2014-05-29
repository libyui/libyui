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
 * File:	YQGraphPlugin.h
 * Author:	Arvin Schnell <aschnell@suse.de>
 */


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
    virtual YQGraph * createGraph ( YWidget * parent, const std::string & filename,
				    const std::string & layoutAlgorithm );

};


#endif // YQGraphPlugin_h
