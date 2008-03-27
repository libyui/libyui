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


/*
  Textdomain "packages"
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
    description = new YMenuItem( _("Package Description") );
    items.push_back( description );

    technical = new YMenuItem( _("Technical data") );
    items.push_back( technical );

    versions = new YMenuItem( _("Package Versions") );
    items.push_back( versions );

    files = new YMenuItem( _("File List") );
    items.push_back( files );

    deps = new YMenuItem( _("Dependencies") );
    items.push_back( deps );

    addItems( items );
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
	yuiError() << "*cobe*-d up" << endl;
	return false;
    }

    if (event.selection == versions)
    {
	if ( !pkg->VersionsList() )
 	    //replaceInfoText ( NCPkgTable::I_Versions);
	    pkg->replaceInfoText( true );
        pkgList->setVisibleInfo (NCPkgTable::I_Versions);
	pkg->VersionsList()->fillAvailableList( slbPtr );
    }
    else
    {
        if ( !pkg->InfoText() )
	    pkg->replaceInfoText( false );
           //replaceInfoText();
        if (event.selection == description)
        { 
	    if( pkg->ReplacePoint()->firstChild() == pkg->InfoText() )
	       yuiError() << "snehulak" << endl;
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
    }
    return true;
}
