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

   File:       NCPkgFilterRepo.cc

   Author:     Gabriele Mohr <gs@suse.com> 

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgFilterClassification.h"

#include "YDialog.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCPackageSelector.h"

#include "NCZypp.h"

/*
  Textdomain "ncurses-pkg"
*/


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterClassification::NCPkgFilterClassification
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//

NCPkgFilterClassification::NCPkgFilterClassification( YWidget *parent, NCPackageSelector *pkg )
    :NCSelectionBox( parent, "" )
    ,packager(pkg)
{
    // fill seclection box
    recommended = new YItem( _("Recommended") );
    addItem( recommended );

    suggested = new YItem( _("Suggested") );
    addItem( suggested );

    orphaned = new YItem( _("Orphaned") );
    addItem( orphaned );
    
    unneeded = new YItem( _("Unneeded" ) );
    addItem( unneeded );

    showPackages();
    showDescription();
}

YItem * NCPkgFilterClassification::getCurrentGroup()
{
    int index = getCurrentItem();

    return itemAt( index );
    
}

bool NCPkgFilterClassification::showPackages( )
{
    NCPkgTable * packageList = packager->PackageList();

    YItem * group = getCurrentGroup();

    if ( !group )
        return false;

    if ( !packageList )
    {
	yuiError() << "No valid NCPkgTable widget" << endl;
    	return false;
    }

    // clear the package table
    packageList->itemsCleared ();

    // filter packages (like done in YQPkgPackageKitGroupsFilterView::filter())
    for ( ZyppPoolIterator it = zyppPkgBegin();
          it != zyppPkgEnd();
          ++it )
    {
        ZyppSel selectable = *it;

        // Multiple instances of this package may or may not be in the same
        // group, so let's check both the installed version (if there
        // is any) and the candidate version.
        //
        // Make sure to add only one package entry if both exist and both are
        // in the same group.
        // We don't want multiple list entries for the same package!

        bool match =
            check( selectable, tryCastToZyppPkg( selectable->candidateObj() ), group ) ||
            check( selectable, tryCastToZyppPkg( selectable->installedObj() ), group );

        // If there is neither an installed nor a candidate package, check
        // any other instance.

        if ( ! match			&&
             ! selectable->candidateObj()	&&
             ! selectable->installedObj()	  )
            check( selectable, tryCastToZyppPkg( selectable->theObj() ), group );
    }
        
    // show the package list
    packageList->setCurrentItem( 0 );
    packageList->drawList();
    packageList->showInformation();

    yuiMilestone() << "Filling package list \"" << group->label() <<  "\"" << endl;

    return true;
}

bool NCPkgFilterClassification::check( ZyppSel selectable, ZyppPkg pkg, YItem * group )
{
    NCPkgTable * packageList = packager->PackageList();

    if ( !packageList )
    {
	yuiError() << "No valid package table widget" << endl;
    	return false;
    }
    
    if ( group == recommended &&
         zypp::PoolItem(pkg).status().isRecommended() )
    {
       	packageList->createListEntry( pkg, selectable );
        return true;
    }
    if ( group == suggested &&
         zypp::PoolItem(pkg).status().isSuggested() ) 
    {
        packageList->createListEntry( pkg, selectable );
        return true;
    }
    if ( group == orphaned &&
         zypp::PoolItem(pkg).status().isOrphaned() )
    {
        packageList->createListEntry( pkg, selectable );
        return true; 
    }
    if ( group == unneeded &&
         zypp::PoolItem(pkg).status().isUnneeded() )
    {
        packageList->createListEntry( pkg, selectable );
        return true; 
    }

    return false;
}


void NCPkgFilterClassification::showDescription( )
{
    string description;
    
    YItem * group = getCurrentGroup();

    if ( group == recommended )
    {
        description = _("This is a list of useful packages. They are automatically selected for installation (unless the option <b>Ignore Recommended Packages</b> from <b>Dependencies</b> menu is set).");
    }
    else if ( group == suggested )
    {
        description = _("It's suggested to install these packages because they fit to already installed packages. The decision to install it is by the user."); 
    }
    else if ( group == orphaned )
    {
        description = _("The solver has detected that these packages are without a repository, i.e. updates aren't possible.");
    }
    else if ( group == unneeded )
    {
        description = _("These packages might be unneeded because former dependencies don't apply any longer.");
    }
    packager->FilterDescription()->setText ( description );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterRepo::wHandleInput 
//	METHOD TYPE : NCursesEvent 
//
//	DESCRIPTION : show packages for selected group
//

NCursesEvent NCPkgFilterClassification::wHandleInput( wint_t ch )
{
    NCursesEvent ret = NCursesEvent::none;
    handleInput( ch );

    switch ( ch )
    {
	case KEY_UP:
	case KEY_DOWN:
	case KEY_NPAGE:
	case KEY_PPAGE:
	case KEY_END:
	case KEY_HOME: {
	    ret = NCursesEvent::handled;
            showPackages();
            showDescription();
	    break;
	}

	default:
	   ret = NCSelectionBox::wHandleInput( ch ) ;
     }

    return ret;
}

