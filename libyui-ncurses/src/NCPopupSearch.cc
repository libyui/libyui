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

   File:       NCPopupSearch.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupSearch.h"

#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"
#include "PackageSelector.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSearch::NCPopupSearch
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupSearch::NCPopupSearch( const wpos at, PackageSelector * pkger )
    : NCPopup( at, false )
      , searchExpr( 0 )
      , helpText( 0 )
      , ignoreCase( 0 )
      , checkDescr( 0 )
      , cancelButton( 0 )
      , packager( pkger )
{
    createLayout( YCPString(PkgNames::PackageSearch().str()) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSearch::~NCPopupSearch
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupSearch::~NCPopupSearch()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSearch::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupSearch::createLayout( const YCPString & headline )
{

  YWidgetOpt opt;
 
  // horizontal split is the (only) child of the dialog
  NCSplit * hSplit = new NCSplit( this, opt, YD_HORIZ );
  addChild( hSplit );

  opt.hWeight.setValue( 30 );

  // the help text is the first child of the horizontal split
  helpText = new NCRichText( hSplit, opt, YCPString(PkgNames::HelpOnSearch().str()) );
  
  opt.hWeight.setValue( 80 );
  opt.notifyMode.setValue( false );
  
  // second child is a vertical split
  NCSplit * vSplit = new NCSplit( hSplit, opt, YD_VERT );
  
  hSplit->addChild( helpText );
  hSplit->addChild( vSplit );

  NCSpacing * sp0 = new NCSpacing( vSplit, opt, 0.2, false, true );
  vSplit->addChild( sp0 );
  
  // add the headline
  opt.isHeading.setValue( true );
  NCLabel * head = new NCLabel( vSplit, opt, headline );
  vSplit->addChild( head );

  NCSpacing * sp = new NCSpacing( vSplit, opt, 0.8, false, true );
  vSplit->addChild( sp );
  
  // add the input field (a editable combo box) 
  opt.isEditable.setValue( true );
  opt.isHStretchable.setValue( true );
  searchExpr = new NCComboBox( vSplit, opt, YCPString(PkgNames::SearchPhrase().str()) );
  searchExpr->setKeyboardFocus();
  vSplit->addChild( searchExpr );

  NCSpacing * sp1 = new NCSpacing( vSplit, opt, 0.8, false, true );
  vSplit->addChild( sp1 );
  
  // add the check boxes
  opt.isHStretchable.setValue( true );
  checkDescr = new NCCheckBox( vSplit, opt, YCPString(PkgNames::CheckDescr().str()), false );
  ignoreCase = new NCCheckBox( vSplit, opt, YCPString(PkgNames::IgnoreCase().str()), true );
  NCSpacing * sp2 = new NCSpacing( vSplit, opt, 0.8, false, true );
  NCSpacing * sp3 = new NCSpacing( vSplit, opt, 1.0, false, true );

  vSplit->addChild( checkDescr );
  vSplit->addChild( sp2 );
  vSplit->addChild( ignoreCase );
  vSplit->addChild( sp3 );
  
  // add the cancel button
  NCSplit * hSplit2 = new NCSplit( vSplit, opt, YD_HORIZ );
  vSplit->addChild( hSplit2 );

  opt.isHStretchable.setValue( false );
  
  cancelButton = new NCPushButton( hSplit2, opt, YCPString(PkgNames::CancelLabel().str()) );
  cancelButton->setId( PkgNames::Cancel () );

  okButton = new NCPushButton( hSplit2, opt, YCPString(PkgNames::OKLabel().str()) );
  okButton->setId( PkgNames::OkButton () );

  NCSpacing * sp4 = new NCSpacing( vSplit, opt, 0.4, true, false );
  hSplit2->addChild( cancelButton );
  hSplit2->addChild( sp4 );
  hSplit2->addChild( okButton );
  
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSearch::showSearchPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
NCursesEvent & NCPopupSearch::showSearchPopup( )
{
    postevent = NCursesEvent();
    do {
	popupDialog();
    } while ( postAgain() );
    
    popdownDialog();

    return postevent;
}

///////////////////////////////////////////////////////////////////
//
//	DESCRIPTION :
//
YCPString  NCPopupSearch::getSearchExpression() const
{
    return searchExpr->getValue();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSearch::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPopupSearch::nicesize(YUIDimension dim)
{
    return ( dim == YD_HORIZ ? 60 : 15 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupSearch::wHandleInput( int ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    // if ( ch == KEY_RETURN )
    // return NCursesEvent::button;
    
    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSearch::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupSearch::postAgain()
{
    if ( ! postevent.widget )
	return false;

    postevent.result = YCPNull();
	
    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();
    
    if ( currentId->compare( PkgNames::Cancel () ) == YO_EQUAL )
    {
	// set postevent.result to Null (checked in PackageSelector.cc)
	postevent.result = YCPNull();
    }
    else if ( currentId->compare( PkgNames::OkButton () ) == YO_EQUAL )
    {
	YCPValue value = YCPNull();
	bool ignore = true;
	bool descr = false;
	
	// get the search expression
	postevent.result =  getSearchExpression();

	if ( ignoreCase )
	{
	    value = ignoreCase->getValue();

	    // ignore case is not selected
	    if ( !value.isNull() && value->asBoolean()->toString() == "false" )
	    {
		ignore = false;
	    }
	}
	if ( checkDescr )
	{
	    value = checkDescr->getValue();

            // check description is selected 
	    if ( !value.isNull() && value->asBoolean()->toString() == "true" )
	    {
		descr = true;
	    }
	}

	// fill the package list with packages matching the search expression	
	packager->fillSearchList( postevent.result->asString(),
				  ignore,
				  descr  );
    }
    
    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }
    return true;
}

