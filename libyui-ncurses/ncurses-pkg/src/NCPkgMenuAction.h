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

   File:       NCPkgMenuAction.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/
#ifndef NCPkgMenuAction_h
#define NCPkgMenuAction_h

#include <string>

#include "NCurses.h"
#include "NCi18n.h"
#include "NCMenuButton.h"
#include "NCPopupInfo.h"
#include "NCZypp.h"

class NCPackageSelector;

class NCPkgMenuAction : public NCMenuButton {

    NCPkgMenuAction & operator=( const NCPkgMenuAction & );
    NCPkgMenuAction            ( const NCPkgMenuAction & );

public:

    NCPackageSelector *pkg;

    YItemCollection items;

    YMenuItem *toggleItem;
    YMenuItem *installItem;
    YMenuItem *deleteItem;
    YMenuItem *updateItem;
    YMenuItem *tabooItem;
    YMenuItem *lockItem;
    YMenuItem *allItem;	

    YMenuItem *installAllItem;	
    YMenuItem *deleteAllItem;	
    YMenuItem *keepAllItem;	
    YMenuItem *updateAllItem;	
    YMenuItem *updateNewerItem;	

    NCPkgMenuAction (YWidget *parent, string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuAction();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);		

};

#endif
