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

public:

    YItemCollection items;

    YMenuItem *repoManager;
    YMenuItem *onlineUpdate;
    YMenuItem *actionOnExit;

    YMenuItem *restart;
    YMenuItem *close;
    YMenuItem *showSummary;

    NCPackageSelector *pkg;

    NCPkgMenuConfig (YWidget *parent, string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuConfig();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);

};

#endif
