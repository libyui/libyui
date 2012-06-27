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

   File:       NCPkgFilterRPMGroups.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#ifndef NCPkgFilterRPMGroups_h
#define NCPkgFilterRPMGroups_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCTree.h"
#include "NCLabel.h"
#include "NCRichText.h"
#include "NCZypp.h"

#include <YRpmGroupsTree.h>

class NCTree;
class NCPackageSelector;
class NCRpmGroupItem;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgFilterRPMGroups
//
//	DESCRIPTION :
//
class NCPkgFilterRPMGroups : public NCTree {

    NCPkgFilterRPMGroups & operator=( const NCPkgFilterRPMGroups & );
    NCPkgFilterRPMGroups            ( const NCPkgFilterRPMGroups & );

private:

    NCTree * filterTree;		// the YTree
    
    NCPackageSelector * packager;		// connection to the PackageSelector

    YRpmGroupsTree * _rpmGroupsTree;	// rpm groups of the found packages

    // internal use (copies tree items)
    void cloneTree( YStringTreeItem * parentOrig, NCRpmGroupItem * parentClone );

protected:

    void setCurrentItem( int index );

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    
    NCPkgFilterRPMGroups( YWidget *parent, std::string label, NCPackageSelector * packager );
    virtual ~NCPkgFilterRPMGroups();

    bool handleEvent();

    YStringTreeItem * getDefaultRpmGroup();

    bool checkPackage( ZyppObj opkg, ZyppSel slb, YStringTreeItem *rpmGroup);

    bool showRPMGroupPackages ( const std::string & label, YStringTreeItem *rpmGroup );

    void addItem( YTreeItem * newItem );

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgFilterRPMGroups_h
