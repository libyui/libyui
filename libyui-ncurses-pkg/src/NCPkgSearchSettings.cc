/*
  Copyright (c) [2002-2011] Novell, Inc.
  Copyright (c) 2021 SUSE LLC

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA


  File:       NCPkgFilterMain.cc
  Author:     Hedgehog Painter <kmachalkova@suse.cz>

*/


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

