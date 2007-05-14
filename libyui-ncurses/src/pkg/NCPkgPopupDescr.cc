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

   File:       NCPkgPopupDescr.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"


#include "YMenuButton.h"
#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "NCPkgNames.h"
#include "NCLabel.h"
#include "NCPushButton.h"
#include "NCPkgTable.h"
#include "NCRichText.h"

#include "NCZypp.h"

#include "NCPkgPopupDescr.h"
#include "NCPackageSelector.h"

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDescr::NCPkgPopupDescr
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgPopupDescr::NCPkgPopupDescr( const wpos at, NCPackageSelector * pkger )
    : NCPopup( at, false )
      , pkgTable( 0 )
      , okButton( 0 )
      , headline( 0 )
      , packager( pkger )
{
    createLayout( );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDescr::~NCPkgPopupDescr
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgPopupDescr::~NCPkgPopupDescr()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDescr::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgPopupDescr::createLayout( )
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
    NCLabel * helplb = new NCLabel( split, opt, YCPString(NCPkgNames::DepsHelpLine()) );
    split->addChild( helplb );
  
    split->addChild( new NCSpacing( split, opt, 0.6, false, true ) ); 

    // add the OK button
    opt.key_Fxx.setValue( 10 );
    okButton = new NCPushButton( split, opt, YCPString(NCPkgNames::OKLabel()) );
    okButton->setId( NCPkgNames::OkButton() );

    split->addChild( okButton );

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDescr::fillData
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgPopupDescr::fillData( ZyppPkg pkgPtr, ZyppSel slbPtr )
{
    if ( !pkgPtr )
	return false;

    pkgTable->itemsCleared();		// clear the table

    pkgTable->createListEntry( pkgPtr, slbPtr );

    pkgTable->drawList();

    headline->setLabel( YCPString(pkgPtr->summary()) );

    descrText->setText( YCPString(packager->createDescrText(pkgPtr->description())) );

    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDescr::showInfoPopup
//	METHOD TYPE : NCursesEvent event
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPopupDescr::showInfoPopup( ZyppPkg pkgPtr, ZyppSel slbPtr )
{
    postevent = NCursesEvent();

    fillData( pkgPtr, slbPtr );

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
//	METHOD NAME : NCPkgPopupDescr::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPkgPopupDescr::nicesize(YUIDimension dim)
{
    long vdim;
    if ( NCurses::lines() > 17 )
	vdim = 17;
    else
	vdim = NCurses::lines()-4;
	
    return ( dim == YD_HORIZ ? NCurses::cols()*2/3 : vdim );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPopupDescr::wHandleInput( wint_t ch )
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
//	METHOD NAME : NCPkgPopupDescr::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgPopupDescr::postAgain()
{
    if ( ! postevent.widget )
	return false;

    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( !currentId.isNull()
	 && currentId->compare( NCPkgNames::Cancel() ) == YO_EQUAL )
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

