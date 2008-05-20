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

   File:       NCPkgMenuAction.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgMenuAction.h"
#include "NCPackageSelector.h"


/*
  Textdomain "ncurses-pkg"
*/

NCPkgMenuAction::NCPkgMenuAction (YWidget *parent, string label, NCPackageSelector *pkger)
	: NCMenuButton( parent, label)
	,pkg( pkger ) 
{
    createLayout();
}

NCPkgMenuAction::~NCPkgMenuAction()
{

}

void NCPkgMenuAction::createLayout()
{
    if ( !pkg->isYouMode() )
    {
	// Please note: add an appropriate number of whitespaces to get a well
	// formated menu (the [ ]s should be in one column) and use unique hotkeys until end:
	// begin: Actions menu, toggle the status of a package, e.g. change from installed to delete
	toggleItem =  new YMenuItem( _( "&Toggle    [SPACE]" ) );
	installItem = new YMenuItem( _( "&Install     [+]" ) );
	deleteItem =  new YMenuItem( _( "&Delete      [-]" ) );
	updateItem =  new YMenuItem( _( "&Update      [>]" ) );
	tabooItem =   new YMenuItem( _( "T&aboo       [!]" ) );
	lockItem =    new YMenuItem( _( "&Lock        [*]" ) );
	// end: Actions menu, set status of all packages (title of a submenu)
	allItem =     new YMenuItem( _( "&All Listed Packages" ) );

	items.push_back( toggleItem );
	items.push_back( installItem );
	items.push_back( deleteItem );
	items.push_back( updateItem );
	items.push_back( tabooItem );
	items.push_back( lockItem );  
	items.push_back( allItem );

	// begin: submenu items actions concerning all packages
	installAllItem = new YMenuItem( allItem, _( "&Install All" ) );
	deleteAllItem = new YMenuItem( allItem, _( "&Delete All" ) );
	keepAllItem = new YMenuItem( allItem,  _( "&Keep All" ) );
	updateAllItem = new YMenuItem( allItem, _( "U&pdate All Unconditionally") );
	// end: submenu items: actions concerning all packages
	updateNewerItem = new YMenuItem( allItem,  _( "&Update If Newer Version Available" ) );

	addItems( items );
    }
    else	// YOU mode
    {	// Please note: add an appropriate number of whitespaces to get a well
	// formated menu (the [ ]s should be in one column) and use unique hotkeys until end:
	// begin: Online Update Actions menu
	toggleItem =  new YMenuItem( _( "&Toggle       [SPACE]" ) );
	installItem = new YMenuItem( _( "&Install        [+]" ) );
	deleteItem =  new YMenuItem( _( "&Do Not Install [-]" ) );
	// end: Update Actions menu 
	// update isn't supported for patches
	
	items.push_back( toggleItem );
	items.push_back( installItem );
	items.push_back( deleteItem );

	addItems( items );
    }
}

bool NCPkgMenuAction::handleEvent ( const NCursesEvent & event)
{
    NCPkgTable *pkgList = pkg->PackageList();
    if ( !pkgList || !event.selection)
	return false;

    if ( pkgList->getNumLines() == 0)
	return true;

    if (event.selection == toggleItem)
    {
	pkgList->toggleObjStatus();
    }
    else if (event.selection ==  installItem)
    {
	pkgList->changeObjStatus( '+' );
    }
    else if (event.selection ==  deleteItem )
    {
	pkgList->changeObjStatus( '-' );
    }
    else if (event.selection ==  updateItem )
    {
	pkgList->changeObjStatus( '>' );
    }
    else if (event.selection ==  tabooItem )
    {
	pkgList->changeObjStatus( '!' );
    }
    else if (event.selection ==  lockItem )
    {
	pkgList->changeObjStatus( '*' );
    }
    else if (event.selection ==  installAllItem )
    {
	pkgList->changeListObjStatus( NCPkgTable::A_Install );
    }
    else if (event.selection ==  deleteAllItem )
    {
	pkgList->changeListObjStatus( NCPkgTable::A_Delete );
    }
    else if (event.selection == keepAllItem )
    {
	pkgList->changeListObjStatus( NCPkgTable::A_Keep );
    }
    else if ( event.selection == updateNewerItem )
    {
	pkgList->changeListObjStatus( NCPkgTable::A_UpdateNewer );
    }
    else if (event.selection == updateAllItem )
    {
	pkgList->changeListObjStatus( NCPkgTable::A_Update );
    }
    else 
	yuiError() << "zatim nic" << endl;

    return true;

}
