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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YQPackageSelectorPluign::createPkgSpecial
//	METHOD TYPE : YWidget
//
//	DESCRIPTION : creates special widgets used for the package selection
//		      dialog (which do not have a corresponding widget in qt-ui)
//
YWidget * YQPackageSelectorPluginStub::createPkgSpecial( YWidget *parent, const string &subwidget )
{
    return impl->createPkgSpecial( parent, subwidget );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YQPackageSelectorPLugin::runPkgSelection
//	METHOD TYPE : YEvent *
//
//	DESCRIPTION : Implementation of UI builtin RunPkgSelection() which
//		      has to be called after OpenDialog( `PackageSelector() ).
//
YEvent * YQPackageSelectorPluginStub::runPkgSelection(  YDialog * dialog,
						    YWidget * selector )
{
    return impl->runPkgSelection( dialog, selector );
}

