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

   File:       NCPkgFilterSearch.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#ifndef NCPkgFilterSearch_h
#define NCPkgFilterSearch_h

#include <iosfwd>

#include <vector>
#include <string>
#include <set>

#include "NCPopup.h"
#include "NCInputField.h"
#include "NCLabel.h"
#include "NCLayoutBox.h"
#include "NCRichText.h"
#include "NCCheckBox.h"
#include "NCComboBox.h"
#include "NCPushButton.h"

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
