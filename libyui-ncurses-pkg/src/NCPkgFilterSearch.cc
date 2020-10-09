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

   File:       NCPkgFilterSearch.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>
#include <string>
#include <sstream>
#include <boost/format.hpp>

#include "NCPkgFilterSearch.h"

#include "NCAlignment.h"
#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCFrame.h"

#include "NCPkgStrings.h"
#include "NCPackageSelector.h"
#include "NCPkgSearchSettings.h"
#include "NCPopupInfo.h"

#include "NCi18n.h"

using std::endl;

/*
  Textdomain "ncurses-pkg"
*/

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterSearch::NCPkgFilterSearch
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgFilterSearch::NCPkgFilterSearch( YWidget *parent, YUIDimension dim, NCPackageSelector *pkger )
    : NCLayoutBox( parent, dim )
      , searchExpr( 0 )
      , ignoreCase( 0 )
      , packager( pkger )
{
    // the layout is created in NCPackageSelector
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterSearch::~NCPkgFilterSearch
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgFilterSearch::~NCPkgFilterSearch()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterSearch::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgFilterSearch::createLayout( YWidget *parent )
{

    NCFrame * frame0 = new NCFrame( this, "" );

    // add the input field (a editable combo box)
    NCLayoutBox * vSplit = new NCLayoutBox ( frame0, YD_VERT);
    // new NCSpacing( vSplit, YD_VERT, true, 0.5 );

    searchExpr = new NCInputField( vSplit, NCPkgStrings::SearchPhrase() );
    searchExpr->setStretchable( YD_HORIZ, true );

    // new NCSpacing( vSplit, YD_VERT, false, 0.5 );

    if ( !packager->isYouMode() )
    {
	// add the checkBox ignore case
	NCAlignment *left1 = new NCAlignment( vSplit, YAlignBegin, YAlignUnchanged );

	ignoreCase = new NCCheckBox( left1, _( "&Ignore Case" ), true );
        new NCSpacing( vSplit, YD_VERT, true, 0.5 );

    }

    NCAlignment *left2 = new NCAlignment( vSplit, YAlignBegin, YAlignUnchanged );
    searchMode = new NCComboBox (left2, _( "Search &Mode" ), false);

    searchMode->addItem( _( "Contains" ), false);
    searchMode->addItem( _( "Begins with" ), false);
    searchMode->addItem( _( "Exact Match" ), false);
    searchMode->addItem( _( "Use Wildcards" ), false);
    searchMode->addItem( _( "Use RegExp" ), false);

    new NCSpacing( vSplit, YD_VERT, true, 0.5 );
}


///////////////////////////////////////////////////////////////////
//
//	DESCRIPTION :
//
std::string  NCPkgFilterSearch::getSearchExpression() const
{
    std::string value;

    if ( searchExpr )
    {
	// get the expression and store it in combo box list
	// value = searchExpr->getValue();

	value = searchExpr->value();
	// searchExpr->addItem( value, true );
    }

    return value;
}

bool NCPkgFilterSearch::match( std::string s1, std::string s2, bool ignoreCase )
{
    std::string::iterator pos;

    if ( ignoreCase )
    {
	pos = search( s1.begin(), s1.end(),
		      s2.begin(), s2.end(),
		      ic_compare );
    }
    else
    {
	pos = search( s1.begin(), s1.end(),
		      s2.begin(), s2.end() );
    }

    return ( pos != s1.end() );
}


bool NCPkgFilterSearch::fillSearchList( std::string & expr,
                                        bool ignoreCase )
{
    NCPkgTable * packageList = packager->PackageList();

    if ( !packageList )
    {
	return false;
    }

    // clear the package table
    packageList->itemsCleared();

    NCPkgSearchSettings *settings = packager->SearchSettings();
    zypp::PoolQuery q;

    switch ( searchMode->getCurrentItem() )
    {
	case Contains:
	    q.setMatchSubstring();
	    break;
	case BeginsWith:
	    expr = '^' + expr;
	    q.setMatchRegex();
	    break;
	case ExactMatch:
	    q.setMatchExact();
	    break;
	case UseWildcard:
	    q.setMatchGlob();
	    break;
	case UseRegexp:
	    q.setMatchRegex();
	    break;
    }

    q.addString( expr );
    q.addKind( zypp::ResKind::package );

    if ( !ignoreCase )
	q.setCaseSensitive();
    if ( settings->doCheckName() )
	q.addAttribute( zypp::sat::SolvAttr::name );
    if ( settings->doCheckSummary() )
	q.addAttribute( zypp::sat::SolvAttr::summary );
    if ( settings->doCheckKeywords() )
	q.addAttribute( zypp::sat::SolvAttr::keywords );
    if ( settings->doCheckDescr() )
	q.addAttribute( zypp::sat::SolvAttr::description );
    if ( settings->doCheckProvides() )
        q.addAttribute( zypp::sat::SolvAttr::provides );
    if ( settings->doCheckRequires() )
        // attribute SolvAttr::requires means "required by"
        q.addAttribute( zypp::sat::SolvAttr::requires );

    NCPopupInfo * info = new NCPopupInfo( wpos( (NCurses::lines()-4)/2, (NCurses::cols()-18)/2 ),
					  "",
					  _( "Searching..." )
					  );
    info->setPreferredSize( 18, 4 );
    info->popup();

    try
    {
	for ( zypp::PoolQuery::Selectable_iterator it = q.selectableBegin();
	     it != q.selectableEnd(); it++)
	{
	    ZyppPkg pkg = tryCastToZyppPkg( (*it)->theObj() );
	    packageList->createListEntry ( pkg, *it);
	}
    }
    catch (const std::exception & e)
    {
	NCPopupInfo * info = new NCPopupInfo ( wpos( NCurses::lines()/10,
						     NCurses::cols()/10),
					       NCPkgStrings::ErrorLabel(),
					       // Popup informs the user that the query std::string
					       // entered for package search isn't correct
					       _( "Query Error:" ) + ("<br>") + e.what(),
					       NCPkgStrings::OKLabel() );
	info->setPreferredSize( 50, 10 );
	info->showInfoPopup();
	YDialog::deleteTopmostDialog();
	yuiError() << "Caught a std::exception: " << e.what() << endl;
    }

    info->popdown();
    YDialog::deleteTopmostDialog();

    int found_pkgs = packageList->getNumLines();
    std::ostringstream s;
    s << boost::format( _( "%d packages found" )) % found_pkgs;
    packager->PatternLabel()->setText( s.str() );

    // show the package list
    packageList->drawList();

    if ( found_pkgs > 0 )
    {
	packageList->scrollToFirstItem();
	packageList->showInformation();
	packageList->setKeyboardFocus();
    }
    else
	packager->clearInfoArea();

    return true;

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterSearch::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgFilterSearch::showSearchResultPackages()
{
    std::string filter =  getSearchExpression();

    if ( !packager->isYouMode() )
    {
        // fill the package list with packages matching the search expression
        fillSearchList( filter, getCheckBoxValue( ignoreCase ) );
    }

    return true;
}

bool NCPkgFilterSearch::getCheckBoxValue( NCCheckBox * checkBox )
{
    YCheckBoxState value = YCheckBox_off;

    if ( checkBox )
    {
	value = checkBox->value();

	return ( value == YCheckBox_on ? true : false );
    }

    return false;
}
