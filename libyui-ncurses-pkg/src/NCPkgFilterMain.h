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


#ifndef NCPkgFilterMain_h
#define NCPkgFilterMain_h

#include <string>

#include <yui/ncurses/NCurses.h>
#include <yui/ncurses/NCi18n.h>
#include <yui/ncurses/NCComboBox.h>

#include "NCPackageSelector.h"
#include "NCZypp.h"

class NCPackageSelector;


class NCPkgFilterMain : public NCComboBox
{
    NCPkgFilterMain & operator=( const NCPkgFilterMain & );
    NCPkgFilterMain            ( const NCPkgFilterMain & );

public:

    YItemCollection items;
    NCPackageSelector *pkg;

    YItem *patterns;
    YItem *languages;
    YItem *repositories;
    YItem *services;
    YItem *search;
    YItem *inst_summary;
    YItem *pkg_class;

    NCPkgFilterMain (YWidget *parent, std::string label, NCPackageSelector *pkger );
    virtual ~NCPkgFilterMain();

    void createLayout();

    bool handleEvent();

    void setSummarySelected() { selectItem(inst_summary); }
    void setReposSelected() { selectItem(repositories); }
    void setPkgClassSelected() { selectItem(pkg_class); }
};

#endif
