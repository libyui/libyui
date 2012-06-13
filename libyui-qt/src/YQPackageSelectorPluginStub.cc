/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////   __/\\\\\\_____________/\\\__________/\\\________/\\\___/\\\________/\\\___/\\\\\\\\\\\_           ////
 ////    _\////\\\____________\/\\\_________\///\\\____/\\\/___\/\\\_______\/\\\__\/////\\\///__          ////
 ////     ____\/\\\______/\\\__\/\\\___________\///\\\/\\\/_____\/\\\_______\/\\\______\/\\\_____         ////
 ////      ____\/\\\_____\///___\/\\\_____________\///\\\/_______\/\\\_______\/\\\______\/\\\_____        ////
 ////       ____\/\\\______/\\\__\/\\\\\\\\\_________\/\\\________\/\\\_______\/\\\______\/\\\_____       ////
 ////        ____\/\\\_____\/\\\__\/\\\////\\\________\/\\\________\/\\\_______\/\\\______\/\\\_____      ////
 ////         ____\/\\\_____\/\\\__\/\\\__\/\\\________\/\\\________\//\\\______/\\\_______\/\\\_____     ////
 ////          __/\\\\\\\\\__\/\\\__\/\\\\\\\\\_________\/\\\_________\///\\\\\\\\\/_____/\\\\\\\\\\\_    ////
 ////           _\/////////___\///___\/////////__________\///____________\/////////______\///////////__   ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                 widget abstraction library providing Qt, GTK and ncurses frontends                  ////
 ////                                                                                                     ////
 ////                                   3 UIs for the price of one code                                   ////
 ////                                                                                                     ////
 ////                                        ***  Qt4 plugin  ***                                         ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                              (C) SUSE Linux GmbH    ////
 ////                                                                                                     ////
 ////                                                              libYUI-AsciiArt (C) 2012 Björn Esser   ////
 ////                                                                                                     ////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-/

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
                       << std::endl;
    }


    impl = (YQPackageSelectorPluginIf*) locateSymbol("PSP");

    if ( ! impl )
    {
        yuiError() << "Plugin " << PLUGIN_BASE_NAME << " does not provide PSP symbol" << std::endl;
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

