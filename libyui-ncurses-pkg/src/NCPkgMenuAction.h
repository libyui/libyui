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


  File:       NCPkgMenuAction.cc
  Author:     Hedgehog Painter <kmachalkova@suse.cz>

*/


#ifndef NCPkgMenuAction_h
#define NCPkgMenuAction_h

#include <string>

#include <yui/ncurses/NCurses.h>
#include <yui/ncurses/NCi18n.h>
#include <yui/ncurses/NCMenuButton.h>
#include <yui/ncurses/NCPopupInfo.h>

#include "NCZypp.h"

class NCPackageSelector;

class NCPkgMenuAction : public NCMenuButton
{
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

    NCPkgMenuAction (YWidget *parent, std::string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuAction();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);

};

#endif
