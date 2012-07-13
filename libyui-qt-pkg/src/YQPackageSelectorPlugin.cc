/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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

  Textdomain	"qt-pkg"

/-*/

#include "YQPackageSelectorPlugin.h"
#define YUILogComponent "qt-ui"
#include "YUILog.h"
#include "pkg/YQPackageSelector.h"
#include "pkg/YQPatternSelector.h"
#include "pkg/YQSimplePatchSelector.h"
#include "YUIException.h"

#define PLUGIN_BASE_NAME "qt-pkg"


YQPackageSelectorPlugin::YQPackageSelectorPlugin()
    : YPackageSelectorPlugin( PLUGIN_BASE_NAME )
{
    if ( success() )
    {
	yuiMilestone() << "Loaded " << PLUGIN_BASE_NAME
		       << " plugin successfully from " << pluginLibFullPath()
		       << endl;
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

