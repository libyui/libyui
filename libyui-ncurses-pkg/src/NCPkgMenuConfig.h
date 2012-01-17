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

   File:       NCPkgMenuConfig.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/
#ifndef NCPkgMenuConfig_h
#define NCPkgMenuConfig_h

#include <string>

#include "NCurses.h"
#include "NCi18n.h"
#include "NCMenuButton.h"
//#include "NCPackageSelector.h"
#include "NCPkgStrings.h"
#include "NCPkgPopupDiskspace.h"


class NCPackageSelector;


class NCPkgMenuConfig : public NCMenuButton {

    NCPkgMenuConfig & operator=( const NCPkgMenuConfig & );
    NCPkgMenuConfig            ( const NCPkgMenuConfig & );

private:
   std::map <string, YMenuItem*> idToItemPtr;
   string exitAction;

public:

    YItemCollection items;

    YMenuItem *repoManager;
    YMenuItem *onlineUpdate;
    YMenuItem *actionOnExit;
    YMenuItem *webpinSearch;

    YMenuItem *restart;
    YMenuItem *close;
    YMenuItem *showSummary;

    NCPackageSelector *pkg;

    NCPkgMenuConfig (YWidget *parent, string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuConfig();

    void setSelected ( YMenuItem *item, bool selected);
    void createLayout();

    bool handleEvent (const NCursesEvent & event);

};

#endif
