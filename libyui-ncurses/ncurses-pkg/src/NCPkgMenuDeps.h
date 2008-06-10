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

   File:       NCPkgMenuDeps.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/
#ifndef NCPkgMenuDeps_h
#define NCPkgMenuDeps_h

#include <string>

#include "NCurses.h"
#include "NCi18n.h"
#include "NCMenuButton.h"
#include "NCPopupInfo.h"
#include "NCPkgPopupTable.h"
#include "NCZypp.h"

class NCPackageSelector;

class NCPkgMenuDeps : public NCMenuButton {

    NCPkgMenuDeps & operator=( const NCPkgMenuDeps & );
    NCPkgMenuDeps            ( const NCPkgMenuDeps & );

public:

    NCPackageSelector *pkg;

    YItemCollection items;

    YMenuItem *autoCheckDeps;
    YMenuItem *checkNow;
    YMenuItem *verifySystem;
    YMenuItem *testCase;	

    NCPkgMenuDeps (YWidget *parent, string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuDeps();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);		

    bool checkDependencies();

    bool generateTestcase();

    bool setAutoCheck();

    bool verify();
};

#endif
