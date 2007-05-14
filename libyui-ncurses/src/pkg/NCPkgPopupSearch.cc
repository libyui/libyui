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
#include "NCSplit.h"
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
	createLayout( YCPString(NCPkgNames::PackageSearch()) );
    else
	createLayout( YCPString( _("Search for Patch Name") ) );	
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
void NCPkgPopupSearch::createLayout( const YCPString & headline )
{
    YWidgetOpt opt;

    // vertical split is the (only) child of the dialog
    NCSplit * vSplit = new NCSplit( this, opt, YD_VERT );
    addChild( vSplit );

    vSplit->addChild( new NCSpacing( vSplit, opt, 0.8, false, true ) );

    // add the headline
    opt.isHeading.setValue( true );
    NCLabel * head = new NCLabel( vSplit, opt, headline );
    vSplit->addChild( head );

    vSplit->addChild( new NCSpacing( vSplit, opt, 0.6, false, true ) );

    // add the input field (a editable combo box)
    opt.isHStretchable.setValue( true );

    NCFrame * frame0 = new NCFrame( vSplit, opt, YCPString("" ) );
    NCSplit * vSplit2 = new NCSplit( frame0, opt, YD_VERT );

    opt.isEditable.setValue( true );
    searchExpr = new NCComboBox( frame0, opt, YCPString(NCPkgNames::SearchPhrase()) );
    frame0->addChild( searchExpr );
    searchExpr->setId( NCPkgNames::SearchBox() );
    searchExpr->itemAdded( YCPString( "" ), 	// set initial value
			   0,		// index
			   false );	// not selected
    vSplit2->addChild( new NCSpacing( vSplit, opt, 0.6, false, true ) );
    vSplit->addChild( frame0 );

    if ( !packager->isYouMode() )
    {
	// add the checkBox ignore case
	NCSplit * hSplit2 = new NCSplit( vSplit, opt, YD_HORIZ );
	vSplit->addChild( hSplit2 );
	ignoreCase = new NCCheckBox( hSplit2, opt, YCPString(NCPkgNames::IgnoreCase()), true );
	hSplit2->addChild( new NCSpacing( hSplit2, opt, 0.1, true, false ) );
	hSplit2->addChild( ignoreCase );

	vSplit->addChild( new NCSpacing( vSplit, opt, 0.6, false, true ) );	// VSpacing

	// add a frame containing the other check boxes
	opt.isHStretchable.setValue( true );
	opt.isVStretchable.setValue( true );
	NCFrame * frame = new NCFrame( vSplit, opt, YCPString(NCPkgNames::SearchIn()) );
	NCSplit * vSplit3 = new NCSplit( frame, opt, YD_VERT );

	opt.isVStretchable.setValue( false );
	checkName = new NCCheckBox( vSplit3, opt, YCPString(NCPkgNames::CheckName()), true );
	checkSummary = new NCCheckBox( vSplit3, opt, YCPString(NCPkgNames::CheckSummary()), true );
	checkDescr = new NCCheckBox( vSplit3, opt, YCPString(NCPkgNames::CheckDescr()), false );
	checkProvides = new NCCheckBox( vSplit3, opt, YCPString(NCPkgNames::CheckProvides()), false );
	checkRequires = new NCCheckBox( vSplit3, opt, YCPString(NCPkgNames::CheckRequires()), false );

	vSplit3->addChild( checkName );
	vSplit3->addChild( checkSummary );
	vSplit3->addChild( checkDescr );
	vSplit3->addChild( checkProvides );
	vSplit3->addChild( checkRequires );

	frame->addChild( vSplit3 );
	vSplit->addChild( frame );

	vSplit->addChild( new NCSpacing( vSplit, opt, 0.8, false, true ) );		// VSpacing
    }

    NCSplit * hSplit3 = new NCSplit( vSplit, opt, YD_HORIZ );
    vSplit->addChild( hSplit3 );
    
    // add the cancel and the ok button
    opt.key_Fxx.setValue( 10 );
    okButton = new NCPushButton( hSplit3, opt, YCPString(NCPkgNames::OKLabel()) );
    okButton->setId( NCPkgNames::OkButton () );

    opt.key_Fxx.setValue( 9 );
    opt.isVStretchable.setValue( false );
    cancelButton = new NCPushButton( hSplit3, opt, YCPString(NCPkgNames::CancelLabel()) );
    cancelButton->setId( NCPkgNames::Cancel () );

    opt.isHStretchable.setValue( true );
    hSplit3->addChild( new NCSpacing( hSplit3, opt, 0.2, true, false ) );
    hSplit3->addChild( okButton );
    hSplit3->addChild( new NCSpacing( hSplit3, opt, 0.4, true, false ) );
    hSplit3->addChild( cancelButton );
    hSplit3->addChild( new NCSpacing( hSplit3, opt, 0.2, true, false ) );
    
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
YCPString  NCPkgPopupSearch::getSearchExpression() const
{
    YCPString value = YCPNull();
    unsigned int i = 0;

    if ( searchExpr )
    {
	// get the expression and store it in combo box list
	value = searchExpr->getValue();
	i = searchExpr->getListSize();

	searchExpr->itemAdded( value,	// the search expression
			       i,	// index
			       true );	// selected
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

    postevent.result = YCPNull();

    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( !currentId.isNull() &&
	 currentId->compare( NCPkgNames::Cancel () ) == YO_EQUAL )
    {
	postevent = NCursesEvent::cancel;
    }
    else if ( postevent == NCursesEvent::button )
    {
	YCPString filter( "" );

	// get the search expression
	postevent.result =  getSearchExpression();
	if ( !postevent.result.isNull() )
	{
	    filter =  postevent.result->asString();
	}

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
    YCPValue value = YCPNull();

    if ( checkBox )
    {
	value = checkBox->getValue();

	// return whether the option is selected or not
	if ( !value.isNull() )
	{
	    return ( value->asBoolean()->toString() == "true" ? true : false );
	}
    }

    return false;
}
