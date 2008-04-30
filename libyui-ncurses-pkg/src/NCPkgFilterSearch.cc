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
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

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

#include "NCi18n.h"

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
      , searchButton( 0 )
      , packager( pkger )
{
	createLayout( parent );
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
    new NCSpacing( vSplit, YD_VERT, true, 0.5 );

    NCLayoutBox * hSplit = new NCLayoutBox ( vSplit, YD_HORIZ);
    searchExpr = new NCComboBox( hSplit,
				 NCPkgStrings::SearchPhrase(),
				 true );	// editable = true

    searchExpr->setStretchable( YD_HORIZ, true );
    searchExpr->addItem( new YTableItem() );

    NCLayoutBox * bogus  = new NCLayoutBox ( hSplit, YD_VERT);
    new NCSpacing( bogus, YD_VERT, false, 1.0 );
    searchButton = new NCPushButton( bogus, NCPkgStrings::SearchButton());
    packager->setSearchButton( searchButton );

    new NCSpacing( vSplit, YD_VERT, false, 0.5 );
    
    if ( !packager->isYouMode() )
    {
	// add the checkBox ignore case
	NCAlignment *left = new NCAlignment( vSplit, YAlignBegin, YAlignUnchanged );

	ignoreCase = new NCCheckBox( left, NCPkgStrings::IgnoreCase(), true );
        new NCSpacing( vSplit, YD_VERT, true, 0.5 );
	
    }
        
}


///////////////////////////////////////////////////////////////////
//
//	DESCRIPTION :
//
string  NCPkgFilterSearch::getSearchExpression() const
{
    string value;

    if ( searchExpr )
    {
	// get the expression and store it in combo box list
	// value = searchExpr->getValue();

	value = searchExpr->text();
	searchExpr->addItem( value, true );
    }

    return value;
}

bool NCPkgFilterSearch::match( string s1, string s2, bool ignoreCase )
{
    string::iterator pos;

    if ( ignoreCase )
    {
	pos = ::search( s1.begin(), s1.end(),
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


bool NCPkgFilterSearch::fillSearchList( const string & expr,
                                        bool ignoreCase,
                                        bool checkName,
                                        bool checkSummary,
                                        bool checkDescr,
                                        bool checkProvides,
                                        bool checkRequires )
{
    NCPkgTable * packageList = packager->PackageList();

    if ( !packageList )
    {
	return false;
    }

    // clear the package table
    packageList->itemsCleared ();

// FIXME filter it inside zypp
    ZyppPoolIterator b = zyppPkgBegin ();
    ZyppPoolIterator e = zyppPkgEnd ();
    ZyppPoolIterator i;

    // get the package list and sort it
    list<ZyppSel> pkgList( b, e );
    pkgList.sort( sortByName );

    // fill the package table
    list<ZyppSel>::iterator listIt = pkgList.begin();
    ZyppPkg pkg;
    string description = "";
    string provides = "";
    string requires = "";

    while ( listIt != pkgList.end() )
    {
	if ( (*listIt)->installedObj() )
	   pkg = tryCastToZyppPkg ((*listIt)->installedObj());
	else
	   pkg = tryCastToZyppPkg ((*listIt)->theObj());

	if ( pkg )
	{
	    if ( checkDescr )
	    {
		description = pkg->description();
		//zypp::Text value = pkg->description();
		//description = createDescrText( value );
	    }
	    if ( checkProvides )
	    {
		zypp::Capabilities value = pkg->dep (zypp::Dep::PROVIDES);
		//provides = createRelLine( value );
	    }
	    if ( checkRequires )
	    {
		zypp::Capabilities value = pkg->dep (zypp::Dep::REQUIRES);
		//requires = createRelLine( value );
	    }
	    if ( ( checkName && match( pkg->name(), expr, ignoreCase )) ||
		 ( checkSummary && match( pkg->summary(), expr, ignoreCase) ) ||
		 ( checkDescr && match( description, expr, ignoreCase) ) ||
		 ( checkProvides && match( provides, expr, ignoreCase) ) ||
		 ( checkRequires && match( requires,  expr, ignoreCase) )
		 )
	    {
		// search sucessful
		packageList->createListEntry( pkg, *listIt );
	    }
	}

	++listIt;
    }

    // show the package list
    packageList->drawList();
    if ( packageList->getNumLines() > 0 )
    {
        packageList->setCurrentItem( 0 );
        packageList->showInformation(); 
    }


    return true;

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgFilterSearch::wHandleInput( wint_t ch )
{
    NCursesEvent ret = NCursesEvent::none;

    // start package search if Return is pressed
    if ( ch == KEY_RETURN )
	ret = NCursesEvent::button;
    else
       ret = NCWidget::wHandleInput( ch );

    return ret;
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
    NCPkgSearchSettings *settings = packager->SearchSettings();
    string filter =  getSearchExpression();
    
    if ( !packager->isYouMode() )
    {
        // fill the package list with packages matching the search expression
        fillSearchList( filter,
    			      getCheckBoxValue( ignoreCase ),
    			      settings->doCheckName(),
    			      settings->doCheckSummary(),
    			      settings->doCheckDescr(),
    			      settings->doCheckProvides(),
    			      settings->doCheckRequires()
    			      );
    }
    else
    {
        packager->fillPatchSearchList( filter );
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
