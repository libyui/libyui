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

   File:       NCPkgPatchSearch.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include "NCPkgPatchSearch.h"

#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCFrame.h"

#include "NCPkgStrings.h"
#include "NCPackageSelector.h"

#include "NCi18n.h"

/*
  Textdomain "ncurses-pkg"
*/

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPatchSearch::NCPkgPatchSearch
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgPatchSearch::NCPkgPatchSearch( const wpos at, NCPackageSelector * pkger )
    : NCPopup( at, false )
      , searchExpr( 0 )
      , packager( pkger )
{
    createLayout( _("Search for Patch Name") );	
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPatchSearch::~NCPkgPatchSearch
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgPatchSearch::~NCPkgPatchSearch()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPatchSearch::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgPatchSearch::createLayout( const string & headline )
{
    // vertical split is the (only) child of the dialog
    NCLayoutBox * vSplit = new NCLayoutBox( this, YD_VERT );

    new NCSpacing( vSplit, YD_VERT, false, 0.8 );	// stretchable = false

    new NCLabel( vSplit, headline, true, false );	// isHeading = true

    new NCSpacing( vSplit, YD_VERT, false, 0.6 );

    NCFrame * frame0 = new NCFrame( vSplit, "" );

    // add the input field (a editable combo box)
    searchExpr = new NCComboBox( frame0,
				 NCPkgStrings::SearchPhrase(),
				 true );	// editable = true

    searchExpr->setStretchable( YD_HORIZ, true );

    searchExpr->addItem( new YTableItem() );

    new NCSpacing( vSplit, YD_VERT, false, 0.6 );
    
    NCLayoutBox * hSplit3 = new NCLayoutBox( vSplit, YD_HORIZ );

    new NCSpacing( hSplit3, YD_HORIZ, true, 0.2 );	

    // add the cancel and the ok button
    okButton = new NCPushButton( hSplit3, NCPkgStrings::OKLabel() );
    okButton->setFunctionKey( 10 );

    new NCSpacing( hSplit3, YD_HORIZ, true, 0.4 );

    cancelButton = new NCPushButton( hSplit3, NCPkgStrings::CancelLabel() );
    cancelButton->setFunctionKey( 9 );
    
    new NCSpacing( hSplit3, YD_HORIZ, true, 0.2 );
    
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPatchSearch::showSearchPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
NCursesEvent & NCPkgPatchSearch::showSearchPopup( )
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
string  NCPkgPatchSearch::getSearchExpression() const
{
    string value;
    unsigned int i = 0;

    if ( searchExpr )
    {
	// get the expression and store it in combo box list
	// value = searchExpr->getValue();

	value = searchExpr->text();
	i = searchExpr->getListSize();

	searchExpr->addItem( value, true );
    }

    return value;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPatchSearch::preferredWidth
//	METHOD TYPE : int
//
int NCPkgPatchSearch::preferredWidth()
{
    return NCurses::cols()/2;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPatchSearch::preferredHeight
//	METHOD TYPE : int
//
int NCPkgPatchSearch::preferredHeight()
{
    return 10;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPatchSearch::wHandleInput( wint_t ch )
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
//	METHOD NAME : NCPkgPatchSearch::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgPatchSearch::postAgain()
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

	packager->fillPatchSearchList( filter );

    }

    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }
    return true;
}
