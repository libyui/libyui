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
    toggleItem =  new YMenuItem( _( "&Toggle    [SPACE]" ) );
    items.push_back( toggleItem );

    installItem = new YMenuItem( _( "&Install      [+]" ) );
    items.push_back( installItem );

    deleteItem =  new YMenuItem( _( "&Delete      [-]" ) );
    items.push_back( deleteItem );

    updateItem =  new YMenuItem( _( "&Update      [>]" ) );
    items.push_back( updateItem );

    tabooItem =   new YMenuItem( _( "T&aboo       [!]" ) );
    items.push_back( tabooItem );

    lockItem =    new YMenuItem( _( "&Lock        [*]" ) );
    items.push_back( lockItem );

    allItem =     new YMenuItem( _( "All Listed &Packages" ) );
    items.push_back( allItem );

    installAllItem = new YMenuItem( allItem, _( "Install All" ) );
    deleteAllItem = new YMenuItem( allItem, _( "Delete All" ) );
    keepAllItem = new YMenuItem( allItem,  _( "Keep All" ) );
    updateAllItem = new YMenuItem( allItem, _( "Update All Unconditionally") );
    updateNewerItem = new YMenuItem( allItem,  _( "Update If Newer Version Available" ) );

    addItems( items );
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
