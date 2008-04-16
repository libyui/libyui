/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPkgMenuFilter.cc

   Author:     Gabriele Mohr <gs@suse.de>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgMenuFilter.h"
#include "NCPkgPatchSearch.h"

/*
  Textdomain "ncurses-pkg"
*/

NCPkgMenuFilter::NCPkgMenuFilter (YWidget *parent, string label, NCPackageSelector *pkger)
	: NCMenuButton( parent, label) 
        ,pkg (pkger)
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
    installable = new YMenuItem( _( "&Installable Patches" ) );
    installed = new YMenuItem( _( "Installed &Patches" ) );
    allPatches = new YMenuItem( _( "&All Patches" ) );
    recommended = new YMenuItem( _( "&Recommended" ) );
    security = new YMenuItem( _( "&Security" ) );
    optional = new YMenuItem( _( "&Optional" ) );
    // end:
    search = new YMenuItem( _( "S&earch" ) );

    items.push_back( installable );
    items.push_back( installed );
    items.push_back( allPatches );
    items.push_back( recommended );
    items.push_back( security );
    items.push_back( optional );
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

    if ( event.selection == installable )
	pkg->fillPatchList( "installable" );		// show installed patches
    else if ( event.selection == installed )
	pkg->fillPatchList( "installed" );		// show installed patches
    else if ( event.selection == allPatches )
	pkg->fillPatchList( "all" );			// show all patches
    else if ( event.selection == recommended )
	pkg->fillPatchList( "recommended" );		// patch kind recommended
    else if ( event.selection == security )
	pkg->fillPatchList( "security" );		// patch kind security
    else if ( event.selection == optional )
	pkg->fillPatchList( "optional" );		// patch kind optional
    else if ( event.selection == search )
    {
	searchPopup = new NCPkgPatchSearch( wpos( 1, 1 ), pkg );

	if ( searchPopup )
	{
	    NCursesEvent retEvent = searchPopup->showSearchPopup();

	    if ( retEvent == NCursesEvent::button )
	    {
		yuiMilestone() << "Searching for: " <<  retEvent.result << endl;
		pkgList->showInformation( );
	    }
	    else
	    {
		yuiMilestone() << "Search is canceled"  << endl;
	    }
	    delete searchPopup;
	}
    }
	
    pkgList->setKeyboardFocus();
	

    return true;
}
