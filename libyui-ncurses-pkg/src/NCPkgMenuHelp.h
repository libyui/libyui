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
#ifndef NCPkgMenuHelp_h
#define NCPkgMenuHelp_h

#include <string>

#include "NCurses.h"
#include "NCi18n.h"
#include "NCMenuButton.h"
#include "NCPopupInfo.h"
#include "NCZypp.h"

class NCPackageSelector; 

class NCPkgMenuHelp : public NCMenuButton {

    NCPkgMenuHelp & operator=( const NCPkgMenuHelp & );
    NCPkgMenuHelp            ( const NCPkgMenuHelp & );

public:
    NCPackageSelector *pkg;

    YItemCollection items;

    YMenuItem *generalHelp;
    YMenuItem *statusHelp;
    YMenuItem *filterHelp;
    YMenuItem *menuHelp;
    YMenuItem *patchHelp;

    NCPkgMenuHelp (YWidget *parent, string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuHelp();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);

};

#endif
