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

   File:       NCPkgMenuView.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/
#ifndef NCPkgMenuView_h
#define NCPkgMenuView_h

#include <string>

#include <YReplacePoint.h>
#include <YWidget.h>
#include "NCurses.h"
#include "NCi18n.h"
#include "NCMenuButton.h"
#include "NCZypp.h"

class NCPackageSelector;

class NCPkgMenuView : public NCMenuButton {

    NCPkgMenuView & operator=( const NCPkgMenuView & );
    NCPkgMenuView            ( const NCPkgMenuView & );

private:
    NCPackageSelector *pkg;

public:

    YItemCollection items;

    YMenuItem *description;
    YMenuItem *technical;
    YMenuItem *versions;
    YMenuItem *files;	
    YMenuItem *deps;	

    // Online Update
    YMenuItem * patchDescription;
    YMenuItem * patchPackages;
    YMenuItem * patchPkgVersions;
    
    NCPkgMenuView (YWidget *parent, std::string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuView();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);		

};

#endif
