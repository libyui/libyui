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


  File:       NCPkgFilterSearch.h
  Author:     Gabriele Strattner <gs@suse.de>

*/


#ifndef NCPkgFilterSearch_h
#define NCPkgFilterSearch_h

#include <iosfwd>
#include <vector>
#include <string>
#include <set>

#include <yui/ncurses/NCCheckBox.h>
#include <yui/ncurses/NCComboBox.h>
#include <yui/ncurses/NCInputField.h>
#include <yui/ncurses/NCLabel.h>
#include <yui/ncurses/NCLayoutBox.h>
#include <yui/ncurses/NCPopup.h>
#include <yui/ncurses/NCPushButton.h>
#include <yui/ncurses/NCRichText.h>


class NCPackageSelector;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgFilterSearch
//
//	DESCRIPTION :
//
class NCPkgFilterSearch : public NCLayoutBox
{
    NCPkgFilterSearch & operator=( const NCPkgFilterSearch & );
    NCPkgFilterSearch            ( const NCPkgFilterSearch & );

private:

    enum SearchMode
    {
	Contains = 0,
	BeginsWith,
	ExactMatch,
	UseWildcard,
	UseRegexp
    };

    NCInputField * searchExpr;
    NCCheckBox * ignoreCase;
    NCComboBox * searchMode;

    NCPackageSelector * packager;		// connection to the package selector

    bool getCheckBoxValue( NCCheckBox * checkBox );

protected:

    std::string getSearchExpression() const;

public:

    NCPkgFilterSearch( YWidget *parent, YUIDimension dim, NCPackageSelector *pkger );
    virtual ~NCPkgFilterSearch();

    void createLayout( YWidget *parent );

    virtual bool showSearchResultPackages();

    bool match ( std::string s1, std::string s2, bool ignoreCase );

    bool fillSearchList( std::string & expr, bool ignoreCase );

    NCInputField * getSearchField() { return searchExpr; }

};

///////////////////////////////////////////////////////////////////


#endif // NCPkgFilterSearch_h
