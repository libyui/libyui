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

   File:       NCPkgFilterClassification.h

   Author:     Gabriele Mohr <gs@suse.com> 

/-*/

#ifndef NCPkgFilterClassification_h
#define NCPkgFilterClassification_h

#include <iosfwd>

#include <vector>
#include <string>
#include <algorithm>

#include "NCPadWidget.h"
#include "NCPopup.h"
#include "NCPushButton.h"
#include "NCSelectionBox.h"

#include "NCZypp.h"

class NCTable;
class NCPushButton;
class NCPackageSelector;

class NCPkgFilterClassification: public NCSelectionBox
{
private:

    NCPkgFilterClassification & operator=( const NCPkgFilterClassification & );
    NCPkgFilterClassification            ( const NCPkgFilterClassification & );

    NCPackageSelector *packager;

    YItem *recommended;
    YItem *suggested;
    YItem *orphaned;
    YItem *unneeded;

    bool check (ZyppSel selectable, ZyppPkg pkg, YItem * group );
    
public:

    /**
      * A helper class to hold repository data in a neat table
      * widget
      * @param parent A parent widget
      * @param opt Widget options
      */

    NCPkgFilterClassification  ( YWidget *parent, NCPackageSelector *pkg);

    virtual ~NCPkgFilterClassification() {};

    /**
      * Get currently selected package group item
      */
    YItem * getCurrentGroup();


    virtual NCursesEvent wHandleInput ( wint_t ch );

    /**
     * Fill package list
     */ 
    bool showPackages( );
    
    void showDescription( );

};
#endif
