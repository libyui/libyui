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

  File:		YQPackageSelectorPluginStub.cc

  Authors:	Katharina Machalkova <kmachalkova@suse.cz>
		Stephan Kulow <coolo@suse.de>

/-*/


#include "YQPackageSelectorPluginStub.h"

#define YUILogComponent "qt-ui"
#include "YUILog.h"

#define PLUGIN_BASE_NAME "qt_pkg"

YQPackageSelectorPluginStub::YQPackageSelectorPluginStub()
    : YPackageSelectorPlugin( PLUGIN_BASE_NAME )
{
    if ( success() )
    {
	yuiMilestone() << "Loaded " << PLUGIN_BASE_NAME
                       << " plugin successfully from " << pluginLibFullPath()
                       << endl;
    }


    impl = (YQPackageSelectorPluginIf*) locateSymbol("PSP");
    
    if ( ! impl )
    {
        yuiError() << "Plugin " << PLUGIN_BASE_NAME << " does not provide PSP symbol" << endl;
    }
}


YQPackageSelectorPluginStub::~YQPackageSelectorPluginStub()
{
    // NOP
}


YPackageSelector *
YQPackageSelectorPluginStub::createPackageSelector( YWidget * parent, long modeFlags )
{
    if ( ! impl )
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );
    
    return impl->createPackageSelector( parent, modeFlags );
}


YWidget *
YQPackageSelectorPluginStub::createPatternSelector( YWidget * parent, long modeFlags )
{
    if ( ! impl )
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );
    
    return impl->createPatternSelector( parent, modeFlags );
}


YWidget *
YQPackageSelectorPluginStub::createSimplePatchSelector( YWidget * parent, long modeFlags )
{
    if ( ! impl )
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );
    
    return impl->createSimplePatchSelector( parent, modeFlags );
}

