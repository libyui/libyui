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


  File:       NCPkgMenuHelp.cc
  Author:     Hedgehog Painter <kmachalkova@suse.cz>

*/


#ifndef NCPkgMenuHelp_h
#define NCPkgMenuHelp_h

#include <string>

#include <yui/ncurses/NCurses.h>
#include <yui/ncurses/NCi18n.h>
#include <yui/ncurses/NCMenuButton.h>
#include <yui/ncurses/NCPopupInfo.h>

#include "NCZypp.h"


class NCPackageSelector;

class NCPkgMenuHelp : public NCMenuButton
{
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

    NCPkgMenuHelp (YWidget *parent, std::string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuHelp();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);

};

#endif
