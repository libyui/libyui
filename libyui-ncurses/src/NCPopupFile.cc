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
#include "NCFrame.h"
#include "NCPopupInfo.h"
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
NCPopupFile::NCPopupFile( const wpos at )
    : NCPopup( at, false )
    , headline ( 0 )
    , textLabel( 0 )
    , okButton( 0 )
    , cancelButton( 0 )
    , fileName( 0 )
    , comboBox( 0 )
    , hDim( 50 )
    , vDim( 15 )
{
    createLayout( );
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
void NCPopupFile::createLayout( )
{

    YWidgetOpt opt;

    // the vertical split is the (only) child of the dialog
    NCSplit * split = new NCSplit( this, opt, YD_VERT );
    addChild( split );
  
    // add the headline
    opt.isHeading.setValue( true );
    headline = new NCLabel( split, opt, YCPString("") );
    split->addChild( headline );

    NCSpacing * sp1 = new NCSpacing( split, opt, 0.4, false, true );
    split->addChild( sp1 );
    
    // add the text 
    opt.isHeading.setValue( false );
    opt.isVStretchable.setValue( true );
    textLabel = new NCRichText( split, opt, YCPString(PkgNames::SaveSelText().str()) );
    split->addChild( textLabel );


    // add a frame 
    opt.isVStretchable.setValue( true );
    NCFrame * frame = new NCFrame( split, opt, YCPString("" ) );
    NCSplit * vSplit2 = new NCSplit( frame, opt, YD_VERT );

    // the combo box for selecting the medium
    opt.isHStretchable.setValue( true );
    comboBox = new NCComboBox( vSplit2, opt, YCPString( "Medium" ) );
    comboBox->itemAdded( YCPString( "Hard disk " ), 0, false );
    comboBox->itemAdded( YCPString( "Floppy " ), 0, true );
    vSplit2->addChild( comboBox );
  
    // the text entry field for the file name
    fileName = new NCTextEntry( vSplit2, opt,
				YCPString(PkgNames::FileName().str()),
				YCPString( "" ),
				100, 100 );
    vSplit2->addChild( fileName );
    frame->addChild( vSplit2 );
    split->addChild( frame );

    NCSpacing * sp2 = new NCSpacing( split, opt, 0.4, false, true );
    split->addChild( sp2 );

    opt.isVStretchable.setValue( false );
    
    // HBox for the buttons
    NCSplit * hSplit = new NCSplit( split, opt, YD_HORIZ );
    split->addChild( hSplit );

    opt.isHStretchable.setValue( true );
    NCSpacing * sp3 = new NCSpacing( hSplit, opt, 0.2, true, false );
    hSplit->addChild( sp3 );

    // add the OK button
    opt.key_Fxx.setValue( 10 );
    okButton = new NCPushButton( hSplit, opt, YCPString( "" ) );
    okButton->setId( PkgNames::OkButton() );
  
    hSplit->addChild( okButton );

    NCSpacing * sp4 = new NCSpacing( hSplit, opt, 0.4, true, false );
    hSplit->addChild( sp4 );

    // add the Cancel button
    opt.key_Fxx.setValue( 9 );
    cancelButton = new NCPushButton( hSplit, opt, YCPString(PkgNames::CancelLabel().str()) );
    cancelButton->setId( PkgNames::Cancel() );
  
    hSplit->addChild( cancelButton );
    hSplit->addChild( sp3 );
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
//	METHOD NAME : NCPopupFile::saveSelection
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupFile::saveToFile()
{
    if ( headline && textLabel && okButton )
    {
	headline->setLabel( YCPString( PkgNames::SaveSelHeadline().str() ) );
	textLabel->setText( YCPString( PkgNames::SaveSelText().str() ) );
	okButton->setLabel( YCPString( PkgNames::SaveLabel().str() ) );
    }
    NCursesEvent event = showInfoPopup();
    
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupFile::loadSelection
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupFile::loadFromFile()
{
    if ( headline && textLabel && okButton )
    {
	headline->setLabel( YCPString(PkgNames::LoadSelHeadline().str()) );
	string text = PkgNames::LoadSel2Text().str();
	textLabel->setText( YCPString(text) );
	okButton->setLabel( YCPString( PkgNames::LoadLabel().str() ) ); 
    }
    NCursesEvent event = showInfoPopup();

    if ( event == NCursesEvent::button )
    {
	NCPopupInfo info( wpos(2, 2),
			  YCPString( PkgNames::NotifyLabel().str() ),
			  YCPString( PkgNames::LoadSel1Text().str() ),
			  PkgNames::OKLabel().str(),
			  PkgNames::CancelLabel().str() );
	info.setNiceSize( 30, 10 );
	info.showInfoPopup();
    }
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
    
    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
        // return false means: close the popup dialog
	return false;
    }
    return true;
}

