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


  File:       NCPkgPopupDescr.h
  Author:     Gabriele Strattner <gs@suse.de>

*/


#ifndef NCPkgPopupDescr_h
#define NCPkgPopupDescr_h

#include <iosfwd>

#include <vector>
#include <string>

#include <yui/ncurses/NCPopup.h>

class NCPkgTable;
class NCPushButton;
class NCRichText;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPopupDescr
//
//	DESCRIPTION :
//
class NCPkgPopupDescr : public NCPopup
{
    NCPkgPopupDescr & operator=( const NCPkgPopupDescr & );
    NCPkgPopupDescr            ( const NCPkgPopupDescr & );

private:

    NCPkgTable * pkgTable;
    NCPushButton * okButton;
    NCRichText *descrText;
    NCLabel *headline;

    NCPackageSelector * packager;

protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );

public:

    NCPkgPopupDescr( const wpos at, NCPackageSelector * pkger );

    virtual ~NCPkgPopupDescr();

    virtual long nicesize(YUIDimension dim);

    bool fillData( ZyppPkg pkgPtr, ZyppSel slbPtr );

    void createLayout();

    NCursesEvent showInfoPopup( ZyppPkg pkgPtr, ZyppSel slbPtr );

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPopupDescr_h
