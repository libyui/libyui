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

#ifndef NCPkgFilterInstSummary_h
#define NCPkgFilterInstSummary_h

#include <string>

#include "NCurses.h"
#include "NCi18n.h"
#include "NCPackageSelector.h"
#include "NCPkgTable.h"
#include "NCMultiSelectionBox.h"
#include "NCZypp.h"

class NCPkgFilterInstSummary : public NCMultiSelectionBox {

    NCPkgFilterInstSummary & operator=( const NCPkgFilterInstSummary & );
    NCPkgFilterInstSummary             ( const NCPkgFilterInstSummary & );

    bool check (ZyppObj opkg, ZyppSel slb);
    NCPackageSelector *pkg;

public:

    YItemCollection items;
    YItem *del;
    YItem *inst;
    YItem *update;
    YItem *taboo;
    YItem *protect; 
    YItem *keep; 
    YItem *dontinstall; 


   NCPkgFilterInstSummary (YWidget *parent, string label, NCPackageSelector *pkg);
   virtual ~NCPkgFilterInstSummary();   

   void createLayout();
   bool showInstSummaryPackages();
   NCursesEvent wHandleInput( wint_t ch );

};

#endif
