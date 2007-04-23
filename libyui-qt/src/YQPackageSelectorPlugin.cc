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

  File:		YQPackageSelectorPlugin.cc

  Author:	Stefan Hundhammer <sh@suse.de>

  Textdomain	"packages-qt"

/-*/

#include "YQPackageSelectorPlugin.h"
#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include "YQPackageSelector.h"
#include "YQPatternSelector.h"

#define PLUGIN_BASE_NAME "qt_pkg"


YQPackageSelectorPlugin::YQPackageSelectorPlugin()
    : YPackageSelectorPlugin( PLUGIN_BASE_NAME )
{
    if ( success() )
    {
	y2milestone( "Loaded %s plugin successfully from %s",
		     PLUGIN_BASE_NAME, pluginLibFullPath().c_str() );
    }
}


YQPackageSelectorPlugin::~YQPackageSelectorPlugin()
{
    // NOP
}


YWidget *
YQPackageSelectorPlugin::createPackageSelector( YWidget *	parent,
						YWidgetOpt &	opt )
{
    if ( error() )
	return 0;
    
    YWidget * packageSelector = 0;
    
    try
    {
	packageSelector = new YQPackageSelector( (QWidget *) ( parent->widgetRep() ), opt );
    }
    catch (const std::exception & e)
    {
	y2error( "Caught std::exception: %s", e.what() );
	y2error( "This is a libzypp problem. Do not file a bug against the UI!" );
    }
    catch (...)
    {
	y2error( "Caught unspecified exception." );
	y2error( "This is a libzypp problem. Do not file a bug against the UI!" );
    }
    
    return packageSelector;
}


YWidget *
YQPackageSelectorPlugin::createPatternSelector( YWidget *	parent,
						YWidgetOpt &	opt )
{
    if ( error() )
	return 0;
    
    YWidget * patternSelector = 0;
    
    try
    {
	patternSelector = new YQPatternSelector( (QWidget *) ( parent->widgetRep() ), opt );
    }
    catch (const std::exception & e)
    {
	y2error( "Caught std::exception: %s", e.what() );
	y2error( "This is a libzypp problem. Do not file a bug against the UI!" );
    }
    catch (...)
    {
	y2error( "Caught unspecified exception." );
	y2error( "This is a libzypp problem. Do not file a bug against the UI!" );
    }
    
    return patternSelector;
}

