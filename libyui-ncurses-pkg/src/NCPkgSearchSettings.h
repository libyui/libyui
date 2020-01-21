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

   File:       NCPkgFilterMain.h

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/

#ifndef NCPkgSearchSettings_h
#define NCPkgSearchSettings_h

#include <string>

#include "NCurses.h"
#include "NCi18n.h"
#include "NCMultiSelectionBox.h"

class NCPkgSearchSettings : public NCMultiSelectionBox
{
    NCPkgSearchSettings & operator=( const NCPkgSearchSettings & );
    NCPkgSearchSettings             ( const NCPkgSearchSettings & );

public:

    YItemCollection items;
    YItem *checkName;
    YItem *checkSummary;
    YItem *checkKeywords;
    YItem *checkDescr;
    YItem *checkProvides;
    YItem *checkRequires;


   NCPkgSearchSettings (YWidget *parent, std::string label);
   virtual ~NCPkgSearchSettings();

   void createLayout();

   bool doCheckName();
   bool doCheckSummary();
   bool doCheckKeywords();
   bool doCheckDescr();
   bool doCheckProvides();
   bool doCheckRequires();

};

#endif
