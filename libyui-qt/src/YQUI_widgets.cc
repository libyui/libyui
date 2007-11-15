/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      	YUIQt_widgets.cc

  Author:    	Stefan Hundhammer <sh@suse.de>

/-*/

#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YQUI.h"

#include "YQDialog.h"
#include "YQPackageSelectorPlugin.h"



YQPackageSelectorPlugin * YQUI::packageSelectorPlugin()
{
    static YQPackageSelectorPlugin * plugin = 0;

    if ( ! plugin )
    {
	plugin = new YQPackageSelectorPlugin();

	// This is a deliberate memory leak: If an application requires a
	// PackageSelector, it is a package selection application by
	// definition. In this case, the qt_pkg plugin is intentionally kept
	// open to avoid repeated start-up cost of the plugin and libzypp.
    }

    return plugin;
}




// EOF
