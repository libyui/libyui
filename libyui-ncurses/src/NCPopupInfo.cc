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

   File:       NCPopupInfo.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupInfo.h"

#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupInfo::NCPopupInfo
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupInfo::NCPopupInfo( const wpos at, const YCPString & headline, const YCPString & text )
    : NCPopup( at, false )
      , helpText( 0 )
      , okButton( 0 )
{
    createLayout( headline, text );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupInfo::~NCPopupInfo
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupInfo::~NCPopupInfo()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupInfo::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupInfo::createLayout( const YCPString & headline, const YCPString & text )
{

  YWidgetOpt opt;

  // the vertical split is the (only) child of the dialog
  NCSplit * split = new NCSplit( this, opt, YD_VERT );
  addChild( split );
  
  // add the headline
  opt.isHeading.setValue( true );
  NCLabel * head = new NCLabel( split, opt, headline );
  split->addChild( head );
  
  // add the rich text widget 
  helpText = new NCRichText( split, opt, text );
  split->addChild( helpText );

  // add the ok button
  okButton = new NCPushButton( split, opt, PkgNames::OKLabel() );
  okButton->setId( PkgNames::OkButton () );
  
  split->addChild( okButton );
  
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupInfo::showInfoPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupInfo::showInfoPopup( )
{
    postevent = NCursesEvent();
    do {
	popupDialog( );
    } while ( postAgain() );
    
    popdownDialog();
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupInfo::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPopupInfo::nicesize(YUIDimension dim)
{
    return ( dim == YD_HORIZ ? 50 : 20 );
}

			   
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupInfo::wHandleHotkey
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupInfo::wHandleHotkey( int key )
{

    return NCursesEvent::none;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupInfo::wHandleInput( int ch )
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
//	METHOD NAME : NCPopupInfo::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupInfo::postAgain()
{
    if ( ! postevent.widget )
	return false;
    
    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }
    return true;
}

