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
#include "YWidgetID.h"

#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCPkgNames.h"
#include "NCLabel.h"
#include "NCPushButton.h"
#include "NCPkgTable.h"
#include "NCRichText.h"

#include "NCZypp.h"
#include "NCi18n.h"

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
    NCLayoutBox * split = new NCLayoutBox( this, YD_VERT );

    // addChild() is obsolete (handled by new libyui )

    //split->addChild( new NCSpacing( split, opt, 0.8, false, true ) );
    new NCSpacing( split, YD_VERT, false, 0.8 );	// stretchable = false

    headline = new NCLabel( split, "", true, false );		// isHeading = true
    //split->addChild( new NCSpacing( split, opt, 0.4, false, true ) );
    new NCSpacing( split, YD_VERT, false, 0.4 ); 

    // add the rich text widget for the package description
    opt.isVStretchable.setValue( true );
    descrText = new NCRichText( split, "" );

    //split->addChild( new NCSpacing( split, opt, 0.6, false, true ) );
    new NCSpacing( split, YD_VERT, true, 0.6 );	// stretchable = true

    YTableHeader * tableHeader = new YTableHeader();
    
    // add the package table (use default type T_Packages)
    pkgTable = new NCPkgTable( split, tableHeader );
    pkgTable->setPackager( packager );
    pkgTable->fillHeader();

    //split->addChild( new NCSpacing( split, opt, 0.6, false, true ) );
    new NCSpacing( split, YD_VERT, true, 0.6 );

    opt.isHStretchable.setValue( false );
    //NCLabel * helplb = new NCLabel( split, opt, YCPString(NCPkgNames::DepsHelpLine()) );
    // a help line for the dependency popup
    new NCLabel( split, _( " [+] Select    [-] Delete    [>] Update " ), false, false );
  
    //split->addChild( new NCSpacing( split, opt, 0.6, false, true ) );
    new NCSpacing( split, YD_VERT, false, 0.6 );	// stretchable = false

    // add the OK button
    opt.key_Fxx.setValue( 10 );
    okButton = new NCPushButton( split, NCPkgNames::OKLabel() );
    YStringWidgetID * okID = new YStringWidgetID( "ok" );
    okButton->setId( okID );
    
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

    headline->setLabel( pkgPtr->summary() );

    descrText->setText( packager->createDescrText(pkgPtr->description()) );

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
//x
bool NCPkgPopupDescr::postAgain()
{
    if ( ! postevent.widget )
	return false;

    // YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();
    YWidgetID * currentId = dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( currentId
	 && currentId->toString() == "cancel" )
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

