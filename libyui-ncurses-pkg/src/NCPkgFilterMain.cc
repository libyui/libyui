/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
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
|***************************************************************************/


/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPkgFilterMain.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/

#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgFilterMain.h"

using std::endl;

/*
  Textdomain "ncurses-pkg"
*/

NCPkgFilterMain::NCPkgFilterMain (YWidget *parent, std::string label, NCPackageSelector *pkger )
	:NCComboBox(parent, label, false)
	,pkg (pkger)
{
    createLayout();
    setNotify( true );
}

NCPkgFilterMain::~NCPkgFilterMain()
{

}

void NCPkgFilterMain::createLayout()
{
    patterns = new YItem( _( "Patterns" ) );
    items.push_back( patterns );

    languages = new YItem( _( "Languages" ) );
    items.push_back( languages );

    rpmgroups = new YItem( _( "RPM Groups" ) );
    items.push_back( rpmgroups );

    repositories = new YItem( _( "Repositories" ) );
    items.push_back( repositories );

    services = new YItem( _( "Services" ) );
    items.push_back( services );

    search = new YItem( _( "Search" ) );
    search->setSelected();
    items.push_back( search );

    inst_summary = new YItem( _( "Installation Summary" ) );
    items.push_back( inst_summary );

    pkg_class = new YItem( _( "Package Classification" ) );
    items.push_back( pkg_class );

    addItems( items );

}

bool NCPkgFilterMain::handleEvent ( )
{

    YItem *currentItem = selectedItem();

    if ( currentItem == rpmgroups )
	pkg->replaceFilter ( NCPackageSelector::RPMGroups );
    else if ( currentItem == patterns )
	pkg->replaceFilter ( NCPackageSelector::Patterns );
    else if ( currentItem == languages )
	pkg->replaceFilter ( NCPackageSelector::Languages );
    else if ( currentItem == repositories )
	pkg->replaceFilter ( NCPackageSelector::Repositories );
    else if ( currentItem == services )
	pkg->replaceFilter ( NCPackageSelector::Services );
    else if ( currentItem == search )
	pkg->replaceFilter ( NCPackageSelector::Search );
    else if ( currentItem == inst_summary )
	pkg->replaceFilter ( NCPackageSelector::Summary );
    else if ( currentItem == pkg_class )
        pkg->replaceFilter ( NCPackageSelector::PkgClassification );
    else
	yuiError() << "zatim nic" << endl;
    return true;
}
