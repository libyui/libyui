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

   File:       NCPkgFilterMain.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/

#include "NCPkgSearchSettings.h"
#include "NCPkgStrings.h"

/*
  Textdomain "ncurses-pkg"
*/

NCPkgSearchSettings::NCPkgSearchSettings ( YWidget *parent, std::string label)
	: NCMultiSelectionBox ( parent, label)
{
    createLayout();
}

NCPkgSearchSettings::~NCPkgSearchSettings()
{

}

void NCPkgSearchSettings::createLayout()
{
   checkName = new YItem ( _( "Name of the Package" ), true);
   items.push_back (checkName);
   checkSummary = new YItem ( _( "Summary" ), true);
   items.push_back (checkSummary);
   checkKeywords = new YItem ( _( "Keywords" ));
   items.push_back (checkKeywords);
   checkDescr = new YItem ( _( "Description (time-consuming)" ));
   items.push_back (checkDescr);
   checkProvides = new YItem ( _( "Provides" ));
   items.push_back (checkProvides);
   checkRequires = new YItem ( _( "Required by" ));
   items.push_back (checkRequires);

   addItems( items );
}

bool NCPkgSearchSettings::doCheckName()
{
    return checkName->selected();
}
bool NCPkgSearchSettings::doCheckSummary()
{
    return checkSummary->selected();
}
bool NCPkgSearchSettings::doCheckKeywords()
{
    return checkKeywords->selected();
}
bool NCPkgSearchSettings::doCheckDescr()
{
    return checkDescr->selected();
}
bool NCPkgSearchSettings::doCheckProvides()
{
    return checkProvides->selected();
}
bool NCPkgSearchSettings::doCheckRequires()
{
    return checkRequires->selected();
}

