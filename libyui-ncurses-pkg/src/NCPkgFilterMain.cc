/*
  Copyright (c) [2002-2011] Novell, Inc.
  Copyright (c) 2021 SUSE LLC

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


  File:       NCPkgFilterMain.cc
  Author:     Hedgehog Painter <kmachalkova@suse.cz>

*/


#define YUILogComponent "ncurses-pkg"
#include <yui/YUILog.h>

#include "NCPkgFilterMain.h"
#include "NCPkgFilterService.h"


using std::endl;

/*
  Textdomain "ncurses-pkg"
*/

NCPkgFilterMain::NCPkgFilterMain (YWidget *parent, std::string label, NCPackageSelector *pkger )
	:NCComboBox(parent, label, false)
	, pkg (pkger)
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

    repositories = new YItem( _( "Repositories" ) );
    items.push_back( repositories );

    if (NCPkgServiceTable::any_service())
    {
      services = new YItem( _( "Services" ) );
      items.push_back( services );
    }

    search = new YItem( _( "Search" ) );
    search->setSelected();
    items.push_back( search );

    inst_summary = new YItem( _( "Installation Summary" ) );
    items.push_back( inst_summary );

    pkg_class = new YItem( _( "Package Classification" ) );
    items.push_back( pkg_class );

    addItems( items );

}

bool NCPkgFilterMain::handleEvent()
{

    YItem *currentItem = selectedItem();

    if ( currentItem == patterns )
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
