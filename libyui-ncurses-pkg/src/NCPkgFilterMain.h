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
    YItem *pkg_class;
    
    NCPkgFilterMain (YWidget *parent, string label, NCPackageSelector *pkger );
    virtual ~NCPkgFilterMain();

    void createLayout();

    bool handleEvent( );

    void setSummarySelected() { selectItem(inst_summary); }
    void setReposSelected() { selectItem(repositories); }
};

#endif
