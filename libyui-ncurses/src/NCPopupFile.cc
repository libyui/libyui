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

   File:       NCPopupFile.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupFile.h"

#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupFile::NCPopupFile
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupFile::NCPopupFile( const wpos at,
			  const YCPString & headline,
			  const YCPString & text )
    : NCPopup( at, false )
      , textLabel( 0 )
      , okButton( 0 )
      , cancelButton( 0 )
      , fileName( 0 )
      , hDim( 50 )
      , vDim( 20 )
      , visible ( false )
{
    createLayout( headline, text );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupFile::~NCPopupFile
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupFile::~NCPopupFile()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupFile::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupFile::createLayout( const YCPString & headline,
				const YCPString & text )
{

    YWidgetOpt opt;

    // the vertical split is the (only) child of the dialog
    NCSplit * split = new NCSplit( this, opt, YD_VERT );
    addChild( split );
  
    // add the headline
    opt.isHeading.setValue( true );
    NCLabel * head = new NCLabel( split, opt, headline );
    split->addChild( head );

    NCSpacing * sp1 = new NCSpacing( split, opt, 0.8, false, true );
    split->addChild( sp1 );
    
    // add the text 
    opt.isHeading.setValue( false );
    textLabel = new NCLabel( split, opt, text );
    split->addChild( textLabel );

    split->addChild( sp1 );
    
    fileName = new NCTextEntry( split, opt,
				YCPString(PkgNames::FileName().str()),
				YCPString( "" ),
				100, 100 );
    split->addChild( fileName );

    NCSpacing * sp2 = new NCSpacing( split, opt, 0.8, false, true );
    split->addChild( sp2 );

    NCSplit * hSplit = new NCSplit( split, opt, YD_HORIZ );
    split->addChild( hSplit );

    opt.isHStretchable.setValue( true );
    
    opt.key_Fxx.setValue( 10 );
    // add the OK button
    okButton = new NCPushButton( hSplit, opt, YCPString(PkgNames::OKLabel().str()) );
    okButton->setId( PkgNames::OkButton() );
  
    hSplit->addChild( okButton );

    NCSpacing * sp3 = new NCSpacing( hSplit, opt, 0.4, true, false );
    hSplit->addChild( sp3 );
      
    opt.key_Fxx.setValue( 9 );
    // add the Cancel button
    cancelButton = new NCPushButton( hSplit, opt, YCPString(PkgNames::CancelLabel().str()) );
    cancelButton->setId( PkgNames::Cancel() );
  
    hSplit->addChild( cancelButton );
    NCSpacing * sp4 = new NCSpacing( hSplit, opt, 0.4, true, false );
    hSplit->addChild( sp4 ); 
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupFile::showInfoPopup
//	METHOD TYPE : NCursesEvent &
//
//	DESCRIPTION :
//
NCursesEvent & NCPopupFile::showInfoPopup( )
{
    postevent = NCursesEvent();
    do {
	popupDialog( );
    } while ( postAgain() );
    
    popdownDialog();

    return postevent;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupFile::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPopupFile::nicesize(YUIDimension dim)
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
NCursesEvent NCPopupFile::wHandleInput( int ch )
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
//	METHOD NAME : NCPopupFile::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupFile::postAgain()
{
    if ( ! postevent.widget )
	return false;

    if ( okButton && cancelButton )
    {
	YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();

	if ( currentId->compare( PkgNames::Cancel() ) == YO_EQUAL )
	{
	    // close the dialog 
	    postevent = NCursesEvent::cancel;
	}
	else if  ( currentId->compare( PkgNames::OkButton() ) == YO_EQUAL )
	{
	    postevent = NCursesEvent::button;
	}	
    }
    
    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
        // return false means: close the popup dialog
	return false;
    }
    return true;
}

