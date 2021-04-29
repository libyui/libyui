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


  File:       NCPkgMenuDeps.cc
  Author:     Hedgehog Painter <kmachalkova@suse.cz>

*/


#ifndef NCPkgMenuDeps_h
#define NCPkgMenuDeps_h

#include <string>

#include <yui/ncurses/NCurses.h>
#include <yui/ncurses/NCi18n.h>
#include <yui/ncurses/NCMenuButton.h>
#include <yui/ncurses/NCPopupInfo.h>

#include "NCPkgPopupTable.h"
#include "NCZypp.h"


class NCPackageSelector;

class NCPkgMenuDeps : public NCMenuButton
{
    NCPkgMenuDeps & operator=( const NCPkgMenuDeps & );
    NCPkgMenuDeps            ( const NCPkgMenuDeps & );

public:

    NCPackageSelector *pkg;

    YItemCollection items;

    YMenuItem *autoCheckDeps;
    YMenuItem *checkNow;
    YMenuItem *verifySystem;
    YMenuItem *verifySystemOpt;
    YMenuItem *installRecommendedOpt;
    YMenuItem *installRecommendedNow;
    YMenuItem *cleanDepsOnRemove;
    YMenuItem *allowVendorChange;
    YMenuItem *testCase;

    NCPkgMenuDeps (YWidget *parent, std::string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuDeps();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);

    bool checkDependencies();

    bool generateTestcase();

    bool setAutoCheck();

    bool setCleanDepsOnRemove();

    bool setInstallRecommended();

    bool doInstallRecommended();

    bool setAllowVendorChange();

    bool setVerifySystem();

    bool verify();

    void setSelected( YMenuItem *item, bool selected);
};

#endif
