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
}

bool NCPkgFilterClassification::showPackages( )
{
    int index = getCurrentItem(); 
    YItem * group = itemAt( index );

    if ( group == recommended )
    {
        yuiMilestone() << "recommended is selected" << endl;
    }
    else if ( group == suggested )
    {
        yuiMilestone() << "suggested is selected" << endl;
    }
    else if ( group == unneeded )
    {
        yuiMilestone() << "unneeded is selected" << endl;
    }
    
    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterRepo::wHandleInput 
//	METHOD TYPE : NCursesEvent 
//
//	DESCRIPTION : default boring handle-input
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
	    break;
	}

	default:
	   ret = NCSelectionBox::wHandleInput( ch ) ;
     }

    return ret;
}

