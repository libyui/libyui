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


  File:       NCPkgMenuFilter.cc
  Author:     Gabriele Mohr <gs@suse.de>

*/


#define YUILogComponent "ncurses-pkg"
#include <yui/YUILog.h>

#include "NCPkgPatchSearch.h"
#include "NCPkgTable.h"
#include "NCPackageSelector.h"
#include "NCPkgMenuFilter.h"


using std::endl;

/*
  Textdomain "ncurses-pkg"
*/

NCPkgMenuFilter::NCPkgMenuFilter (YWidget *parent, std::string label, NCPackageSelector *pkger)
    : NCMenuButton( parent, label)
    , pkg (pkger)
{
    createLayout();
}

NCPkgMenuFilter::~NCPkgMenuFilter()
{

}

void NCPkgMenuFilter::createLayout()
{
    // menu items of the filter menu for patches - keep them short
    // and use unique hotkeys from begin: to end:
    // begin:
    needed = new YMenuItem( _( "&Needed Patches" ) );
    // _( "Re&levant Patches" )
    unneeded = new YMenuItem( _( "&Installed Patches" ) );
    // _( "&Satisfied Patches" ) );
    recommended = new YMenuItem( _( "&Recommended" ) );
    security = new YMenuItem( _( "&Security" ) );
    optional = new YMenuItem( _( "&Optional" ) );
    allPatches = new YMenuItem( _( "&All Patches" ) );
    // end:
    search = new YMenuItem( _( "S&earch" ) );

    items.push_back( needed );
    items.push_back( unneeded );
    items.push_back( recommended );
    items.push_back( security );
    items.push_back( optional );
    items.push_back( allPatches );
    items.push_back( search );

    addItems( items );
}


bool NCPkgMenuFilter::handleEvent ( const NCursesEvent & event)
{
    if ( !event.selection)
    {
	yuiError() << "Menu selection failed" << endl;
	return false;
    }

    NCPkgTable *pkgList = pkg->PackageList();

    if ( !pkgList )
    {
	yuiError() << "No package list available" << endl;
	return false;
    }

    yuiMilestone() << "Handle event NCPkgMenuFilter" << endl;

    // Call the appropriate method from NCPackageSelector for
    // the selected menu entry.

    if ( event.selection == needed )
	pkg->fillPatchList( F_Needed );		// show needed patches
    else if ( event.selection == unneeded )
	pkg->fillPatchList( F_Unneeded );	// show unneeded patches
    else if ( event.selection == allPatches )
	pkg->fillPatchList( F_All );			// show all patches
    else if ( event.selection == recommended )
	pkg->fillPatchList( F_Recommended );		// patch kind recommended
    else if ( event.selection == security )
	pkg->fillPatchList( F_Security );		// patch kind security
    else if ( event.selection == optional )
	pkg->fillPatchList( F_Optional );		// patch kind optional
    else if ( event.selection == search )
    {
	searchPopup = new NCPkgPatchSearch( wpos( 1, 1 ), pkg );

	if ( searchPopup )
	{
	    NCursesEvent retEvent = searchPopup->showSearchPopup();

	    if ( retEvent == NCursesEvent::button )
	    {
		yuiMilestone() << "Searching for: " <<  retEvent.result << endl;
		pkgList->showInformation();
	    }
	    else
	    {
		yuiMilestone() << "Search is canceled"  << endl;
	    }
	    searchPopup->destroy();  // (or call YDialog::deleteTopmostDialog())
	    searchPopup = 0;
	}
    }

    pkgList->setKeyboardFocus();

    pkgList->showInformation();

    return true;
}
