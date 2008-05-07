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

   File:       NCPkgMenuView.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgMenuView.h"
#include "NCPackageSelector.h"


/*
  Textdomain "ncurses-pkg"
*/

NCPkgMenuView::NCPkgMenuView (YWidget *parent, string label, NCPackageSelector *pkger)
	: NCMenuButton( parent, label) 
        ,pkg (pkger)
{
    createLayout();
}

NCPkgMenuView::~NCPkgMenuView()
{

}

void NCPkgMenuView::createLayout()
{
    if ( !pkg->isYouMode() )
    {
	// begin: menu items of the view (package information) menu
	// please note: use unique hotkeys until end:
	technical = new YMenuItem( _("&Technical data") );
	description = new YMenuItem( _("&Package Description") );
	versions = new YMenuItem( _("Package &Versions") );
	files = new YMenuItem( _("&File List") );
	// end: menu items of the view menu
	deps = new YMenuItem( _("&Dependencies") );

	items.push_back( technical );
	items.push_back( description );
	items.push_back( versions );
	items.push_back( files );
	items.push_back( deps );
	
	addItems( items );
    }
    else
    {
	// menu items of the patch view menu - keep them short and
	// use unique hotkeys from begin: to end:
	// begin:
	patchDescription =  new YMenuItem( _( "&Long Description" ) );
	patchPackages = new YMenuItem( _( "&Package List" ) );
	// end: menu items of the view menu
	patchPkgVersions = new YMenuItem( _("&Versions") );

	items.push_back( patchDescription );
	items.push_back( patchPackages );
	items.push_back( patchPkgVersions );	
	addItems( items );
    }
}

 
bool NCPkgMenuView::handleEvent ( const NCursesEvent & event)
{
    if ( !event.selection)
	return false;

    NCPkgTable *pkgList = pkg->PackageList();
    int idx = pkgList->getCurrentItem();
    
    ZyppObj pkgPtr = pkgList->getDataPointer( idx );
    ZyppSel slbPtr = pkgList->getSelPointer( idx );

    if ( !pkgPtr || !slbPtr)
    {	
	yuiWarning() << "package list empty - no package pointer" << endl;
	return true;
    }

    // Call the appropriate method from NCPackageSelector for
    // the selected menu entry.
    // Set the information type of the package list (NCPkgTable)
    // acccordingly to to able to show the required information while
    // scrolling the list (NCPkgTable::updateList())

    if ( event.selection == versions )
    {
	pkg->showVersionsList();
	pkgList->setVisibleInfo (NCPkgTable::I_Versions);
    }
    else if ( event.selection == patchPackages )
    {
	pkg->showPatchPackages();
	pkgList->setVisibleInfo (NCPkgTable::I_PatchPkgs);
    }
    else if ( event.selection == patchPkgVersions )
    {
	pkg->showPatchPkgVersions();
	pkgList->setVisibleInfo (NCPkgTable::I_PatchPkgsVersions);	
    }
    else
    {
	pkg->showInformation();

	if ( !pkg->InfoText() )
	    return false;
	
	if (event.selection == description)
        { 
            pkg->InfoText()->longDescription( pkgPtr );
	    pkgList->setVisibleInfo (NCPkgTable::I_Descr);
        }
        else if (event.selection == technical )
        {
            pkg->InfoText()->technicalData( pkgPtr, slbPtr );
	    pkgList->setVisibleInfo (NCPkgTable::I_Technical); 
        }
        else if (event.selection == files )
        {
            pkg->InfoText()->fileList(  slbPtr );
	    pkgList->setVisibleInfo (NCPkgTable::I_Files);
        }
        else if (event.selection == deps )
        {
            pkg->InfoText()->dependencyList( pkgPtr, slbPtr );
	    pkgList->setVisibleInfo (NCPkgTable::I_Deps);
        }
	else if ( event.selection == patchDescription )
	{
	    pkg->InfoText()->patchDescription( pkgPtr, slbPtr );
	    pkgList->setVisibleInfo (NCPkgTable::I_PatchDescr);
	}
    }


    return true;
}
