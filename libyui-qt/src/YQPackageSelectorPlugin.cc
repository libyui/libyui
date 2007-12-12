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
#include "pkg/YQPackageSelector.h"
#include "pkg/YQPatternSelector.h"
#include "pkg/YQSimplePatchSelector.h"
#include "YUIException.h"

#define PLUGIN_BASE_NAME "qt_pkg"


YQPackageSelectorPlugin::YQPackageSelectorPlugin()
    : YPackageSelectorPlugin( PLUGIN_BASE_NAME )
{
    if ( success() )
    {
	y2milestone( "Loaded %s plugin successfully from %s",
		     PLUGIN_BASE_NAME, pluginLibFullPath().c_str() );
    }
    else
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );
}


YQPackageSelectorPlugin::~YQPackageSelectorPlugin()
{
    // NOP
}


YQPackageSelector *
YQPackageSelectorPlugin::createPackageSelector( YWidget * parent, long modeFlags )
{
    if ( error() )
	return 0;
    
    YQPackageSelector * packageSelector = 0;
    
    try
    {
	packageSelector = new YQPackageSelector( parent, modeFlags );
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

    YUI_CHECK_NEW( packageSelector );
    
    return packageSelector;
}


YQPatternSelector *
YQPackageSelectorPlugin::createPatternSelector( YWidget * parent, long modeFlags )
{
    if ( error() )
	return 0;
    
    YQPatternSelector * patternSelector = 0;
    
    try
    {
	patternSelector = new YQPatternSelector( parent, modeFlags );
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

    YUI_CHECK_NEW( patternSelector );
    
    return patternSelector;
}


YQSimplePatchSelector *
YQPackageSelectorPlugin::createSimplePatchSelector( YWidget * parent, long modeFlags )
{
    if ( error() )
	return 0;
    
    YQSimplePatchSelector * simplePatchSelector = 0;
    
    try
    {
	simplePatchSelector = new YQSimplePatchSelector( parent, modeFlags );
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

    YUI_CHECK_NEW( simplePatchSelector );
    
    return simplePatchSelector;
}

