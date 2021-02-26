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

  File:		YQPackageSelectorPluginImpl.cc

  Author:	Stefan Hundhammer
	        \\\'>

/-*/

#include "YQPackageSelectorPluginImpl.h"
#include "YQPackageSelector.h"
#include "YQPatternSelector.h"
#include "YQSimplePatchSelector.h"

#define YUILogComponent "qt-pkg"
#include <YUILog.h>

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
extern "C" {
YQPackageSelectorPluginImpl PSP;
}

YPackageSelector * YQPackageSelectorPluginImpl::createPackageSelector( YWidget * parent,
                                                                       long modeFlags )
{
    /* if ( error() )
       return 0; */

    YQPackageSelector * packageSelector = 0;

    try
    {
        packageSelector = new YQPackageSelector( parent, modeFlags );
    }
    catch (const std::exception & e)
    {
        yuiError() << "Caught std::exception: " << e.what() << endl;
        yuiError() << "This is a libzypp problem. Do not file a bug against the UI!" << endl;
    }
    catch (...)
    {
        yuiError() << "Caught unspecified exception." << endl;
        yuiError() << "This is a libzypp problem. Do not file a bug against the UI!" << endl;
    }

    YUI_CHECK_NEW( packageSelector );

    return packageSelector;
}


YWidget *YQPackageSelectorPluginImpl::createPatternSelector( YWidget * parent, long modeFlags )
{
    /* if ( error() )
       return 0; */

    YQPatternSelector * patternSelector = 0;

    try
    {
        patternSelector = new YQPatternSelector( parent, modeFlags );
    }
    catch (const std::exception & e)
    {
        yuiError() << "Caught std::exception: " << e.what() << endl;
        yuiError() << "This is a libzypp problem. Do not file a bug against the UI!" << endl;
    }
    catch (...)
    {
        yuiError() << "Caught unspecified exception." << endl;
        yuiError() << "This is a libzypp problem. Do not file a bug against the UI!" << endl;
    }

    YUI_CHECK_NEW( patternSelector );

    return patternSelector;

}

YWidget *YQPackageSelectorPluginImpl::createSimplePatchSelector( YWidget * parent, long modeFlags )
{
    /* if ( error() )
       return 0; */

    YQSimplePatchSelector * simplePatchSelector = 0;

    try
    {
        simplePatchSelector = new YQSimplePatchSelector( parent, modeFlags );
    }
    catch (const std::exception & e)
    {
        yuiError() << "Caught std::exception: " << e.what() << endl;
        yuiError() << "This is a libzypp problem. Do not file a bug against the UI!" << endl;
    }
    catch (...)
    {
        yuiError() << "Caught unspecified exception." << endl;
        yuiError() << "This is a libzypp problem. Do not file a bug against the UI!" << endl;
    }

    YUI_CHECK_NEW( simplePatchSelector );

    return simplePatchSelector;

}
