
/* ****************************************************************************
  |
  | Copyright (c) 2000 - 2010 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |*************************************************************************** */

/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|						     (c) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:		YQPackageSelectorPluginStub.h

  Authors:	Katharina Machalkova <kmachalkova@suse.cz>
		Stephan Kulow <coolo@suse.de>

/-*/


#ifndef YQPackageSelectorPluginStub_h
#define YQPackageSelectorPluginStub_h

#include <YPackageSelectorPlugin.h>
#include <YDialog.h>
#include <YEvent.h>

#include "YQPackageSelectorPluginIf.h"


/**
 * Simplified access to the package selector plugin.
 **/
class YQPackageSelectorPluginIf;


class YQPackageSelectorPluginStub: public YPackageSelectorPlugin
{
public:
    /**
     * Constructor: Load the plugin library for the package selector.
     **/
    YQPackageSelectorPluginStub();

    /**
     * Destructor.
     **/
    virtual ~YQPackageSelectorPluginStub();

    /**
     * Create a package selector.
     * Implemented from YPackageSelectorPlugin.
     *
     * This might return 0 if the plugin lib could not be loaded or if the
     * appropriate symbol could not be located in the plugin lib.
     **/
    virtual YPackageSelector * createPackageSelector( YWidget *	parent,
						      long	modeFlags );

    /**
     * Create a pattern selector (optional widget).
     **/
    virtual YWidget * createPatternSelector( YWidget *	parent,
                                             long	modeFlags );

    /**
     * Create a simple patch selector (optional widget).
     **/
    virtual YWidget * createSimplePatchSelector( YWidget * parent,
                                                 long	   modeFlags );


    YQPackageSelectorPluginIf * impl;
};


#endif // YQPackageSelectorPluginStub_h
