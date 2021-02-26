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

   File:       NCPkgFilterInstSummary.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/

#include "NCPkgFilterInstSummary.h"

using std::endl;

/*
  Textdomain "ncurses-pkg"
*/

NCPkgFilterInstSummary::NCPkgFilterInstSummary ( YWidget *parent, std::string label, NCPackageSelector *pkger)
	: NCMultiSelectionBox ( parent, label)
	, pkg( pkger )
{
    //setNotify(true);
    createLayout();
}

NCPkgFilterInstSummary::~NCPkgFilterInstSummary()
{

}

void NCPkgFilterInstSummary::createLayout()
{
   del = new YItem ( _( "Delete" ), true);
   items.push_back (del);
   inst = new YItem ( _( "Install" ), true);
   items.push_back (inst);
   update = new YItem ( _( "Update" ), true);
   items.push_back (update);
   taboo = new YItem ( _( "Taboo" ), true);
   items.push_back (taboo);
   protect = new YItem ( _( "Protected" ), true);
   items.push_back (protect);
   keep = new YItem ( _( "Keep" ));
   items.push_back (keep);
   dontinstall = new YItem ( _( "Do not install" ));
   items.push_back (dontinstall);

   addItems( items );
}

bool NCPkgFilterInstSummary::check( ZyppObj opkg, ZyppSel slb )
{
    if (!slb)
	return false;

    bool show = false;

    switch ( slb->status() )
    {
	//group these two together, due to lack of space
	case S_Del:
	case S_AutoDel: 	show = del->selected();	break;
	case S_Install:
	case S_AutoInstall:	show = inst->selected();	break;
	case S_KeepInstalled:	show = keep->selected();	break;
	case S_NoInst:		show = dontinstall->selected();	break;
	case S_Protected:	show = protect->selected();	break;
	case S_Taboo:		show = taboo->selected();	break;
	case S_Update:
	case S_AutoUpdate:	show = update->selected();	break;
    }

    if ( show )
    {
    	ZyppPkg pkg = tryCastToZyppPkg (opkg);

        if ( !pkg )
	    return false;
    }

    return show;
}

bool NCPkgFilterInstSummary::showInstSummaryPackages()
{

    NCPkgTable * packageList = pkg->PackageList();

    if ( !packageList )
    {
	yuiError() << "No valid NCPkgTable widget" << endl;
    	return false;
    }

    // clear the package table
    packageList->itemsCleared ();


    for_( listIt, zyppPkgBegin(), zyppPkgEnd() )
    {
	ZyppSel selectable = *listIt;
	ZyppObj obj = selectable->candidateObj();

	if (!obj)
	{
	// If there is neither an installed nor a candidate package, check
	// any other instance.
	    ( selectable->installedObj() ) ? (obj = selectable->installedObj())
					   : ( obj = selectable->theObj() );
	}

	if( check( obj, selectable ) )
	{
    	    ZyppPkg pkg = tryCastToZyppPkg (obj);
	    packageList->createListEntry( pkg, selectable);
	}

    }

    // show the package list
    packageList->setCurrentItem( 0 );
    packageList->drawList();
    packageList->showInformation();

    yuiMilestone() << "Fill package list" << endl;

    return true;

}

NCursesEvent NCPkgFilterInstSummary::wHandleInput( wint_t ch )
{
    NCursesEvent ret = NCursesEvent::none;

    //treat this like any other MultiSelBox input ...
    NCMultiSelectionBox::wHandleInput( ch ) ;
    switch ( ch )
    {
	//special case for toggling item status
	case KEY_SPACE:
	case KEY_RETURN: {
	    showInstSummaryPackages();
	}
    }

    //... but do not return to the main loop
    ret = NCursesEvent::handled;

    return ret;
}

