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
    
    NCPkgMenuView (YWidget *parent, string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuView();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);		

};

#endif
