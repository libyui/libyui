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

NCPkgSearchSettings::NCPkgSearchSettings ( YWidget *parent, string label)
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
   checkDescr = new YItem ( _( "Description (time-consuming)" ));
   items.push_back (checkDescr); 
   checkProvides = new YItem ( _( "Provides" ));
   items.push_back (checkProvides); 
   checkRequires = new YItem ( _( "Requires" ));
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

