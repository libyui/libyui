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

   File:       NCPkgMenuHelp.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgMenuHelp.h"
#include "NCPkgStrings.h"
#include "NCPackageSelector.h"


/*
  Textdomain "ncurses-pkg"
*/

NCPkgMenuHelp::NCPkgMenuHelp (YWidget *parent, string label, NCPackageSelector *pkger)
	: NCMenuButton( parent, label) 
	, pkg( pkger)
{
    createLayout();
    setFunctionKey( 1 );
}

NCPkgMenuHelp::~NCPkgMenuHelp()
{

}

void NCPkgMenuHelp::createLayout()
{
    if ( !pkg->isYouMode())
    {
        generalHelp = new YMenuItem( _( "&General Help" ) );
        items.push_back( generalHelp );

        statusHelp = new YMenuItem( _( "&Package Status and Symbols" ) );
        items.push_back( statusHelp );

        filterHelp = new YMenuItem( _( "&How to Use the Filters" ) );
        items.push_back( filterHelp );

        menuHelp = new YMenuItem( _( "&Useful Functions in Menu" ) );
        items.push_back( menuHelp );
    }
    else {
	patchHelp = new YMenuItem( _( "&Patch Status and Patch Installation" ));
	items.push_back ( patchHelp );
    }

    addItems( items);
}

bool NCPkgMenuHelp::handleEvent ( const NCursesEvent & event)
{
    if (!event.selection)
	return false;

    string text = "";
    string headline = "";

    if ( event.selection == generalHelp ) 
    {
        headline = NCPkgStrings::PackageHelp();
	text +=	NCPkgStrings::HelpPkgGen1();
	text +=	NCPkgStrings::HelpPkgGen2();
	text +=	NCPkgStrings::HelpPkgGen3();
	text +=	NCPkgStrings::HelpPkgGen4();
	text +=	NCPkgStrings::HelpPkgGen5();
	text +=	NCPkgStrings::HelpPkgGen6();

    }
    else if ( event.selection == statusHelp ) 
    { 
        headline = NCPkgStrings::PackageStatusHelp();
	text +=	NCPkgStrings::HelpOnStatus1();
	text +=	NCPkgStrings::HelpOnStatus2();
	text +=	NCPkgStrings::HelpOnStatus3();
	text +=	NCPkgStrings::HelpOnStatus4();
	text +=	NCPkgStrings::HelpOnStatus5();
    }
    else if ( event.selection == menuHelp ) 
    { 
        headline = NCPkgStrings::PackageMenuHelp();
	text +=	NCPkgStrings::HelpPkgMenu1();
	text +=	NCPkgStrings::HelpPkgMenu2();
	text += NCPkgStrings::HelpPkgMenu2_2();
	text +=	NCPkgStrings::HelpPkgMenu3();
	text +=	NCPkgStrings::HelpPkgMenu4();
	text +=	NCPkgStrings::HelpPkgMenu5();
    }
    else if ( event.selection == filterHelp ) 
    { 
        headline = NCPkgStrings::PackageFiltersHelp();
	text +=	NCPkgStrings::HelpOnFilters1();
	text +=	NCPkgStrings::HelpOnFilters2();
	text +=	NCPkgStrings::HelpOnFilters3();
	text +=	NCPkgStrings::HelpOnFilters4();
	text +=	NCPkgStrings::HelpOnFilters5();
    }
    else if ( event.selection == patchHelp )
    {
	headline = NCPkgStrings::YouHelp();
        text += NCPkgStrings::YouHelp1();
        text += NCPkgStrings::YouHelp2();
        text += NCPkgStrings::YouHelp3();
    }
    else 
       yuiError() << "zatim nic" << endl;

    NCPopupInfo * pkgHelp = new NCPopupInfo( wpos( (NCurses::lines()*8)/100, (NCurses::cols()*18)/100 ),
					     headline,
					     text
					     );
    pkgHelp->setPreferredSize( (NCurses::cols()*65)/100, (NCurses::lines()*85)/100 );
    pkgHelp->showInfoPopup( );

    YDialog::deleteTopmostDialog();

    return true;
}
