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

  File:	      YQPackageSelectorPluginStub.h

  Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/

// -*- c++ -*-

#ifndef YQPackageSelectorPluginStub_h
#define YQPackageSelectorPluginStub_h

#include <YPackageSelectorPlugin.h>
#include <YDialog.h>
#include <YEvent.h>
#include <ycp/YCPString.h>

#include "YQPackageSelectorPluginIf.h"

/**
 * Simplified access to the ncurses UI's package selector plugin.
 **/

class YQPackageSelectorPluginIf;

class YQPackageSelectorPluginStub: public YPackageSelectorPlugin
{
public:


    /**
     * Constructor: Load the plugin library for the YQurses package selector.
     **/
    YQPackageSelectorPluginStub();

    /**
     * Destructor. Calls dlclose() which will unload the plugin library if it
     * is no longer used, i.e. if the reference count dlopen() uses reaches 0.
     **/
    virtual ~YQPackageSelectorPluginStub();

    /**
     * Create a package selector.
     * Implemented from YPackageSelectorPlugin.
     *
     * This might return 0 if the plugin lib could not be loaded or if the
     * appropriate symbol could not be located in the plugin lib.
     **/
    virtual YPackageSelector * createPackageSelector( YWidget *		parent,
						      long		modeFlags );

    // optional widget
    virtual YWidget * createPatternSelector( YWidget *		parent,
                                             long		modeFlags );

    // optional widget
    virtual YWidget * createSimplePatchSelector( YWidget *		parent,
                                                 long		modeFlags );


    /**
     * Fills the PackageSelector widget (runs the package selection).
     */
    virtual YEvent * runPkgSelection( YDialog *currentDialog,
				      YWidget * packageSelector );

    /**
     * Create a special widget
     */
    virtual YWidget * createPkgSpecial( YWidget *parent,
					const string &subwidget );


    YQPackageSelectorPluginIf *impl ;
};


#endif // YQPackageSelectorPluginStub_h
