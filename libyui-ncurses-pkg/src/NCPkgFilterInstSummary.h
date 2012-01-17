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
