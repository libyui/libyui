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

  File:		NCPackageSelectorPluginStub.cc

  Author:	Hedgehog Painter <kmachalkova@suse.cz>


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


    impl =  (YQPackageSelectorPluginIf*) locateSymbol("PSP");
    if ( !impl )
    {
        yuiError() << "Plugin " << PLUGIN_BASE_NAME << " does not provide PSP symbol" << endl;
        exit( 1 );
    }

}


YQPackageSelectorPluginStub::~YQPackageSelectorPluginStub()
{
    // NOP
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YQPackageSelectorPluginStub::createPackageSelector
//	METHOD TYPE : YWidget
//
//	DESCRIPTION : Create YQPackageSelectorStart which reads the layout
//                    term of the package selection dialog, creates the widget
//		      tree and creates the YQPackageSelector.
//
YPackageSelector * YQPackageSelectorPluginStub::createPackageSelector( YWidget * parent,
								   long modeFlags )
{
    return impl->createPackageSelector( parent, modeFlags );
}


YWidget * YQPackageSelectorPluginStub::createPatternSelector( YWidget * parent,
								   long modeFlags )
{
    return impl->createPatternSelector( parent, modeFlags );
}

YWidget * YQPackageSelectorPluginStub::createSimplePatchSelector( YWidget * parent,
                                                                           long modeFlags )
{
    return impl->createSimplePatchSelector( parent, modeFlags );
}

