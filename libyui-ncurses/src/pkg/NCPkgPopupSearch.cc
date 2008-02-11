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

   File:       NCPkgPopupSearch.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPkgPopupSearch.h"

#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCFrame.h"

#include "NCPkgNames.h"
#include "NCPackageSelector.h"

#include "NCi18n.h"

/*
  Textdomain "packages"
*/

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSearch::NCPkgPopupSearch
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgPopupSearch::NCPkgPopupSearch( const wpos at, NCPackageSelector * pkger )
    : NCPopup( at, false )
      , searchExpr( 0 )
      , ignoreCase( 0 )
      , checkName( 0 )
      , checkSummary( 0 )
      , checkDescr( 0 )
      , checkProvides( 0 )
      , checkRequires( 0 )
      , cancelButton( 0 )
      , packager( pkger )
{
    if ( !packager->isYouMode() )
	createLayout( _( "Package Search" ) );
    else
	createLayout( _("Search for Patch Name") );	
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSearch::~NCPkgPopupSearch
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgPopupSearch::~NCPkgPopupSearch()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSearch::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgPopupSearch::createLayout( const string & headline )
{
    // vertical split is the (only) child of the dialog
    NCLayoutBox * vSplit = new NCLayoutBox( this, YD_VERT );

    new NCSpacing( vSplit, YD_VERT, false, 0.8 );	// stretchable = false

    new NCLabel( vSplit, headline, true, false );	// isHeading = true

    new NCSpacing( vSplit, YD_VERT, false, 0.6 );

    NCFrame * frame0 = new NCFrame( vSplit, "" );

    // add the input field (a editable combo box)
    searchExpr = new NCComboBox( frame0,
				 NCPkgNames::SearchPhrase(),
				 true );	// editable = true

    searchExpr->setStretchable( YD_HORIZ, true );

    searchExpr->addItem( new YTableItem() );

    new NCSpacing( vSplit, YD_VERT, false, 0.6 );
    
    if ( !packager->isYouMode() )
    {
	// add the checkBox ignore case
	NCLayoutBox * hSplit2 = new NCLayoutBox( vSplit, YD_HORIZ );
	new NCSpacing( hSplit2, YD_HORIZ, true, 0.1 );

	ignoreCase = new NCCheckBox( hSplit2, NCPkgNames::IgnoreCase(), true );  // checked = true
	new NCSpacing( vSplit, YD_VERT, false, 0.6 );
	
	// label of a frame in search popup (without hotkey)
	NCFrame * frame = new NCFrame( vSplit, _( " Search in " ) );
	NCLayoutBox * vSplit3 = new NCLayoutBox( frame, YD_VERT );

	checkName = new NCCheckBox( vSplit3, NCPkgNames::CheckName(), true );
	checkName->setStretchable(YD_HORIZ, true );
	
	checkSummary = new NCCheckBox( vSplit3, NCPkgNames::CheckSummary(), true );
	checkSummary->setStretchable(YD_HORIZ, true );
	
	checkDescr = new NCCheckBox( vSplit3, NCPkgNames::CheckDescr(), false );
	checkDescr->setStretchable(YD_HORIZ, true );
	
	checkProvides = new NCCheckBox( vSplit3, NCPkgNames::CheckProvides(), false );
	checkProvides->setStretchable(YD_HORIZ, true );
	
	checkRequires = new NCCheckBox( vSplit3, NCPkgNames::CheckRequires(), false );
	checkRequires->setStretchable(YD_HORIZ, true );

	new NCSpacing( vSplit, YD_VERT, false, 0.8 );
    }

    NCLayoutBox * hSplit3 = new NCLayoutBox( vSplit, YD_HORIZ );

    new NCSpacing( hSplit3, YD_HORIZ, true, 0.2 );	

    // add the cancel and the ok button
    okButton = new NCPushButton( hSplit3, NCPkgNames::OKLabel() );
    okButton->setFunctionKey( 10 );

    new NCSpacing( hSplit3, YD_HORIZ, true, 0.4 );

    cancelButton = new NCPushButton( hSplit3, NCPkgNames::CancelLabel() );
    cancelButton->setFunctionKey( 9 );
    
    new NCSpacing( hSplit3, YD_HORIZ, true, 0.2 );
    
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSearch::showSearchPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
NCursesEvent & NCPkgPopupSearch::showSearchPopup( )
{
    postevent = NCursesEvent();
    do {
	popupDialog();
	if ( searchExpr )
	{
	    searchExpr->setKeyboardFocus();
	}
    } while ( postAgain() );

    popdownDialog();

    return postevent;
}

///////////////////////////////////////////////////////////////////
//
//	DESCRIPTION :
//
string  NCPkgPopupSearch::getSearchExpression() const
{
    string value;
    unsigned int i = 0;

    if ( searchExpr )
    {
	// get the expression and store it in combo box list
	// value = searchExpr->getValue();

	value = searchExpr->text();
	i = searchExpr->getListSize();
#if 0
	searchExpr->itemAdded( value,	// the search expression
			       i,	// index
			       true );	// selected
#endif
	searchExpr->addItem( value, true );
    }

    return value;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSearch::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
long NCPkgPopupSearch::nicesize(YUIDimension dim)
{
    long vdim;

    if ( !packager->isYouMode() )
    {
	if ( NCurses::lines() > 20 )
	    vdim = 20;
	else
	    vdim = NCurses::lines()-4;
    }
    else
    {
	vdim = 10; 
    }
    
    return ( dim == YD_HORIZ ? NCurses::cols()/2 : vdim );
}
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPopupSearch::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    // start package search if Return is pressed
    if ( ch == KEY_RETURN )
	return NCursesEvent::button;

    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSearch::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgPopupSearch::postAgain()
{
    if ( ! postevent.widget )
	return false;

    postevent.result = "";

    if ( postevent.widget == cancelButton )
    {
	postevent = NCursesEvent::cancel;
    }
    else if ( postevent == NCursesEvent::button )
    {
	// get the search expression
	postevent.result =  getSearchExpression();

	string filter =  postevent.result;

	if ( !packager->isYouMode() )
	{
	    // fill the package list with packages matching the search expression
	    packager->fillSearchList( filter,
				      getCheckBoxValue( ignoreCase ),
				      getCheckBoxValue( checkName ),
				      getCheckBoxValue( checkSummary ),
				      getCheckBoxValue( checkDescr ),
				      getCheckBoxValue( checkProvides ),
				      getCheckBoxValue( checkRequires )
				      );
	}
	else
	{
	    packager->fillPatchSearchList( filter );
	}
    }

    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }
    return true;
}

bool NCPkgPopupSearch::getCheckBoxValue( NCCheckBox * checkBox )
{
    YCheckBoxState value = YCheckBox_off;
    
    if ( checkBox )
    {
	value = checkBox->value();

	// return whether the option is selected or not
	//if ( !value.isNull() )
	// return ( value->asBoolean()->toString() == "true" ? true : false );

	return ( value == YCheckBox_on ? true : false );
    }

    return false;
}
