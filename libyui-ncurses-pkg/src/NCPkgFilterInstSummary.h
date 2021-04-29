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


  File:       NCPkgFilterMain.h
  Author:     Hedgehog Painter <kmachalkova@suse.cz>

*/


#ifndef NCPkgFilterInstSummary_h
#define NCPkgFilterInstSummary_h

#include <string>

#include <yui/ncurses/NCurses.h>
#include <yui/ncurses/NCi18n.h>
#include <yui/ncurses/NCMultiSelectionBox.h>

#include "NCPackageSelector.h"
#include "NCPkgTable.h"
#include "NCZypp.h"


class NCPkgFilterInstSummary : public NCMultiSelectionBox
{
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


   NCPkgFilterInstSummary (YWidget *parent, std::string label, NCPackageSelector *pkg);
   virtual ~NCPkgFilterInstSummary();

   void createLayout();
   bool showInstSummaryPackages();
   NCursesEvent wHandleInput( wint_t ch );

};

#endif
