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

   File:       NCPopupPkgDescr.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"


#include "YMenuButton.h"
#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"
#include "NCLabel.h"
#include "NCPushButton.h"
#include "NCPkgTable.h"
#include "NCRichText.h"

#include <Y2PM.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PkgDu.h>

#include "NCPopupPkgDescr.h"
#include "PackageSelector.h"

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupPkgDescr::NCPopupPkgDescr
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupPkgDescr::NCPopupPkgDescr( const wpos at, PackageSelector * pkger )
    : NCPopup( at, false )
      , pkgTable( 0 )
      , okButton( 0 )
      , headline( 0 )
      , packager( pkger )
      , hDim( 70 )
      , vDim( 17 )
{
    createLayout( );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupPkgDescr::~NCPopupPkgDescr
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupPkgDescr::~NCPopupPkgDescr()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupPkgDescr::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupPkgDescr::createLayout( )
{
    YWidgetOpt opt;

    // the vertical split is the (only) child of the dialog
    NCSplit * split = new NCSplit( this, opt, YD_VERT );
    addChild( split );

    split->addChild( new NCSpacing( split, opt, 0.8, false, true ) );

    // add the headline
    opt.isHeading.setValue( true );
    headline = new NCLabel( split, opt, YCPString( "" ) );
    split->addChild( headline );

    split->addChild( new NCSpacing( split, opt, 0.4, false, true ) );

    // add the rich text widget for the package description
    opt.isVStretchable.setValue( true );
    descrText = new NCRichText( split, opt, YCPString( "" ) );
    split->addChild( descrText );

    split->addChild( new NCSpacing( split, opt, 0.6, false, true ) );

    // add the package table (use default type T_Packages)
    pkgTable = new NCPkgTable( split, opt );
    pkgTable->setPackager( packager );
    pkgTable->fillHeader();

    split->addChild( pkgTable );
    split->addChild( new NCSpacing( split, opt, 0.6, false, true ) );
    
    opt.isHStretchable.setValue( false );
    NCLabel * helplb = new NCLabel( split, opt, YCPString(PkgNames::DepsHelpLine()) );
    split->addChild( helplb );
  
    split->addChild( new NCSpacing( split, opt, 0.6, false, true ) ); 

    // add the OK button
    opt.key_Fxx.setValue( 10 );
    okButton = new NCPushButton( split, opt, YCPString(PkgNames::OKLabel()) );
    okButton->setId( PkgNames::OkButton() );

    split->addChild( okButton );

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupPkgDescr::fillData
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupPkgDescr::fillData( PMPackagePtr & pkgPtr )
{
    if ( !pkgPtr )
	return false;

    pkgTable->itemsCleared();		// clear the table

    pkgTable->createListEntry( pkgPtr );

    pkgTable->drawList();

    headline->setLabel( YCPString(pkgPtr->summary()) );

    descrText->setText( YCPString(packager->createDescrText(pkgPtr->description())) );

    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupPkgDescr::showInfoPopup
//	METHOD TYPE : NCursesEvent event
//
//	DESCRIPTION :
//
NCursesEvent NCPopupPkgDescr::showInfoPopup( PMPackagePtr & pkgPtr )
{
    postevent = NCursesEvent();

    fillData( pkgPtr );

    do {
	// show the popup
	popupDialog( );
    } while ( postAgain() );

    popdownDialog();

    return postevent;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupPkgDescr::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPopupPkgDescr::nicesize(YUIDimension dim)
{
    return ( dim == YD_HORIZ ? hDim : vDim );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupPkgDescr::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    if ( ch == KEY_RETURN )
	return NCursesEvent::button;

    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupPkgDescr::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupPkgDescr::postAgain()
{
    if ( ! postevent.widget )
	return false;

    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( !currentId.isNull()
	 && currentId->compare( PkgNames::Cancel() ) == YO_EQUAL )
    {
	// close the dialog
	postevent = NCursesEvent::cancel;
    }

    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }
    return true;
}

