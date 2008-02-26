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

// -*- c++ -*-

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


    YQPackageSelectorPluginIf * impl ;
};


#endif // YQPackageSelectorPluginStub_h
