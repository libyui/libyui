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


  File:       NCPkgFilterClassification.h
  Author:     Gabriele Mohr <gs@suse.com>

*/


#ifndef NCPkgFilterClassification_h
#define NCPkgFilterClassification_h

#include <iosfwd>
#include <vector>
#include <string>
#include <algorithm>

#include <yui/ncurses/NCPadWidget.h>
#include <yui/ncurses/NCPopup.h>
#include <yui/ncurses/NCPushButton.h>
#include <yui/ncurses/NCSelectionBox.h>

#include "NCZypp.h"


class NCTable;
class NCPackageSelector;

class NCPkgFilterClassification: public NCSelectionBox
{
public:

    /**
      * A helper class to hold repository data in a neat table
      * widget
      * @param parent A parent widget
      * @param opt Widget options
      */

    NCPkgFilterClassification( YWidget *parent, NCPackageSelector *pkg);

    virtual ~NCPkgFilterClassification() {};

    void showRetractedInstalled();

    virtual NCursesEvent wHandleInput( wint_t ch );

protected:

    YItem * currentPkgClass() const;
    void setCurrentPkgClass( YItem * item );
    int itemIndex( YItem * item ) const;

    bool showPackages();
    void showDescription();

    bool check(ZyppSel selectable, ZyppPkg pkg, YItem * group );


    // Data members

    NCPackageSelector *packager;

    YItem *recommended;
    YItem *suggested;
    YItem *orphaned;
    YItem *unneeded;
    YItem *multiversion;
    YItem *retracted;
    YItem *retractedInstalled;
    YItem *all;

private:

    NCPkgFilterClassification & operator=( const NCPkgFilterClassification & );
    NCPkgFilterClassification            ( const NCPkgFilterClassification & );

};
#endif
