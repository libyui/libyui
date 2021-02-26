/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:		NCPackageSelectorPluginStub.cc

  Author:	Hedgehog Painter <kmachalkova@suse.cz>


/-*/

#include "NCPackageSelectorPluginStub.h"

#define YUILogComponent "ncurses-ui"
#include <yui/YUILog.h>
#include "NCWidget.h"
#include "NCLabel.h"
#include "NCDialog.h"
#include "NCPackageSelectorPluginIf.h"
#include "YNCursesUI.h" // NCtoY2Event

#define PLUGIN_BASE_NAME "ncurses-pkg"


NCPackageSelectorPluginStub::NCPackageSelectorPluginStub()
    : YPackageSelectorPlugin( PLUGIN_BASE_NAME )
{
    if ( success() )
    {
	yuiMilestone() << "Loaded " << PLUGIN_BASE_NAME
	<< " plugin successfully from " << pluginLibFullPath()
	<< std::endl;
    }


    impl = ( NCPackageSelectorPluginIf* ) locateSymbol( "PSP" );

    if ( !impl )
	YUI_THROW( YUIPluginException( PLUGIN_BASE_NAME ) );

}


NCPackageSelectorPluginStub::~NCPackageSelectorPluginStub()
{
    // NOP
}


YPackageSelector * NCPackageSelectorPluginStub::createPackageSelector( YWidget * parent,
								       long modeFlags )
{
    return impl->createPackageSelector( parent, modeFlags );
}


YWidget * NCPackageSelectorPluginStub::createPkgSpecial( YWidget *parent, const std::string &subwidget )
{
    return impl->createPkgSpecial( parent, subwidget );
}


YEvent * NCPackageSelectorPluginStub::runPkgSelection( YDialog * dialog,
						       YWidget * selector )
{
    return impl->runPkgSelection( dialog, selector );
}
