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

#ifndef NCPkgFilterMain_h
#define NCPkgFilterMain_h

#include <string>

#include "NCurses.h"
#include "NCi18n.h"
#include "NCComboBox.h"
#include "NCPackageSelector.h"
#include "NCZypp.h"

class NCPkgFilterMain : public NCComboBox {

    NCPkgFilterMain & operator=( const NCPkgFilterMain & );
    NCPkgFilterMain            ( const NCPkgFilterMain & );

public:

    YItemCollection items;
    NCPackageSelector *pkg;

    YItem *patterns;
    YItem *languages;
    YItem *rpmgroups;
    YItem *repositories;
    YItem *search;		
    YItem *inst_summary;		

    NCPkgFilterMain (YWidget *parent, string label, NCPackageSelector *pkger );
    virtual ~NCPkgFilterMain();

    void createLayout();

    bool handleEvent( );

};

#endif
