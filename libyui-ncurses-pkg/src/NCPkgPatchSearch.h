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


  File:       NCPkgPatchSearch.h
  Author:     Gabriele Strattner <gs@suse.de>

*/


#ifndef NCPkgPatchSearch_h
#define NCPkgPatchSearch_h

#include <iosfwd>
#include <vector>
#include <string>

#include <yui/ncurses/NCCheckBox.h>
#include <yui/ncurses/NCComboBox.h>
#include <yui/ncurses/NCLabel.h>
#include <yui/ncurses/NCPopup.h>
#include <yui/ncurses/NCPushButton.h>
#include <yui/ncurses/NCRichText.h>


class NCPackageSelector;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPatchSearch
//
//	DESCRIPTION :
//
class NCPkgPatchSearch : public NCPopup
{
    NCPkgPatchSearch & operator=( const NCPkgPatchSearch & );
    NCPkgPatchSearch            ( const NCPkgPatchSearch & );

private:

    NCComboBox * searchExpr;

    YItem * searchName;
    YItem * searchSum;

    NCPushButton * cancelButton;
    NCPushButton * okButton;

    NCPackageSelector * packager;		// connection to the package selector

protected:

    std::string getSearchExpression() const;

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );

public:

    NCPkgPatchSearch( const wpos at, NCPackageSelector *pkger );
    virtual ~NCPkgPatchSearch();

    virtual int preferredWidth();
    virtual int preferredHeight();

    void createLayout( const std::string & headline );

    NCursesEvent & showSearchPopup();

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPatchSearch_h
