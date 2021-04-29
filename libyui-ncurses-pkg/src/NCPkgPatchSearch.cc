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


  File:       NCPkgPatchSearch.cc
  Author:     Gabriele Strattner <gs@suse.de>

*/


#include <yui/YDialog.h>
#include <yui/YMenuButton.h>

#include <yui/ncurses/NCFrame.h>
#include <yui/ncurses/NCLayoutBox.h>
#include <yui/ncurses/NCSpacing.h>
#include <yui/ncurses/NCTree.h>
#include <yui/ncurses/NCi18n.h>

#include "NCPkgStrings.h"
#include "NCPackageSelector.h"

#include "NCPkgPatchSearch.h"


using std::endl;

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
    createLayout( _( "Search for Patches" ) );
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
void NCPkgPatchSearch::createLayout( const std::string & headline )
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

    NCMultiSelectionBox * settings = new NCMultiSelectionBox( vSplit, NCPkgStrings::SearchIn() );
    YItemCollection items;
    searchName = new YItem ( _( "Name of the Patch" ), true);
    items.push_back( searchName );
    searchSum = new YItem ( _( "Summary" ), true);
    items.push_back( searchSum );
    settings->addItems( items );

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
    new NCSpacing( vSplit, YD_VERT, false, 0.6 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPatchSearch::showSearchPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
NCursesEvent & NCPkgPatchSearch::showSearchPopup()
{
    postevent = NCursesEvent();
    do
    {
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
std::string  NCPkgPatchSearch::getSearchExpression() const
{
    std::string value;

    if ( searchExpr )
    {
	// get the expression and store it in combo box list
	// value = searchExpr->getValue();

	value = searchExpr->text();
	searchExpr->getListSize();

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
    return 16;
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

	std::string filter =  postevent.result;
        bool checkName = searchName->selected();
        bool checkSum = searchSum->selected();
	packager->fillPatchSearchList( filter, checkName, checkSum );

    }

    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }
    return true;
}
