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


  File:       NCPkgPopupTable.h
  Author:     Gabriele Strattner <gs@suse.de>

*/


#ifndef NCPkgPopupTable_h
#define NCPkgPopupTable_h

#include <iosfwd>
#include <string>
#include <vector>
#include <algorithm>

#include <yui/ncurses/NCPopup.h>

#include "NCZypp.h"

class NCPkgTable;
class NCPushButton;
class NCPackageSelector;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPopupTable
//
//	DESCRIPTION :
//
class NCPkgPopupTable : public NCPopup
{
    NCPkgPopupTable & operator=( const NCPkgPopupTable & );
    NCPkgPopupTable            ( const NCPkgPopupTable & );

private:

    NCPkgTable * pkgTable;
    NCPushButton * okButton;
    NCPushButton * cancelButton;
    NCPackageSelector * packager;

protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );

public:

    NCPkgPopupTable( const wpos at, NCPackageSelector * pkger,
                     std::string headline,
                     std::string label1,
                     std::string label2,
                     bool add_cancel = true );

    virtual ~NCPkgPopupTable();

    virtual int preferredWidth();
    virtual int preferredHeight();

    bool fillAutoChanges( NCPkgTable * pkgTable );

    bool fillAvailables( NCPkgTable * pkgTable, ZyppSel sel );

    void createLayout( std::string headline,
                       std::string label1,
                       std::string label2,
                       bool add_cancel );

    NCursesEvent showInfoPopup();
    NCursesEvent showAvailablesPopup( ZyppSel sel );

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPopupTable_h
