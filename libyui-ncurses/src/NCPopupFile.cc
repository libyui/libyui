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

#include "y2util/PathInfo.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupFile::NCPopupFile
//	METHOD TYPE : Constructor
//
NCPopupFile::NCPopupFile( const wpos at, string device )
    : NCPopup( at, false )
    , headline ( 0 )
    , textLabel( 0 )
    , okButton( 0 )
    , cancelButton( 0 )
    , fileName( 0 )
    , comboBox( 0 )
    , pathName( "" )
    , mountFloppy( true )
    , floppyDevice( device )
    , hDim( 50 )
    , vDim( 13 )
{
    createLayout( );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupFile::~NCPopupFile
//	METHOD TYPE : Destructor
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
    NCFrame * frame = new NCFrame( split, opt, YCPString("") );
    NCSplit * vSplit2 = new NCSplit( frame, opt, YD_VERT );

    // the combo box for selecting the medium
    opt.isHStretchable.setValue( true );
    comboBox = new NCComboBox( vSplit2, opt, YCPString(PkgNames::MediumLabel().str()) );
    comboBox->itemAdded( YCPString(PkgNames::Floppy().str()), 0, true );
    comboBox->itemAdded( YCPString(PkgNames::Harddisk().str()), 0, false );
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
//	METHOD NAME : NCPopupFile::showInfoPopup
//	METHOD TYPE : NCursesEvent &
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
//	METHOD NAME : NCPopupFile::niceSize
//	METHOD TYPE : void
//
//
long NCPopupFile::nicesize(YUIDimension dim)
{
    return ( dim == YD_HORIZ ? hDim : vDim );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//
NCursesEvent NCPopupFile::wHandleInput( int ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    if ( ch == KEY_RETURN )
	return NCursesEvent::button;

    // call NCDialog::wHandleInput to handle KEY_DOWN 
    NCursesEvent retEvent = NCDialog::wHandleInput( ch );

    if ( ch == KEY_DOWN )
    {
	// use NCursesEvent::menu (is checked in postAgain())
	retEvent = NCursesEvent::menu;
    }
    
    return retEvent;
}

bool NCPopupFile::mountDevice( )
{
    bool mounted = false;
    
    int exitCode = system( string( "/bin/mount " + floppyDevice +
				   " /media/floppy" + " >/dev/null 2>&1" ).c_str() );
    if ( exitCode == 0 )
    {
	NCMIL << floppyDevice << " mounted on /media/floppy" << endl;
	mounted = true;
    }
    else
    {
	NCERR << "mount " << floppyDevice << " exit code: " << exitCode << endl;
    }

    return mounted;
}

///////////////////////////////////////////////////////////////////
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
	setDefaultPath();
    }
    NCursesEvent event = showInfoPopup();

    if ( event == NCursesEvent::button )
    {
	bool tryAgain = true;
	bool mounted = false;
	
	// if the medium is a floppy mount the device first
	if ( mountFloppy )
	{
	    while ( !mounted && tryAgain )
	    {
		mounted = mountDevice( );
		if ( !mounted )
		{
		    NCPopupInfo info( wpos(2, 2),
				      YCPString( PkgNames::ErrorLabel().str() ),
				      YCPString( PkgNames::SaveErr1Text().str() ),
				      PkgNames::OKLabel().str(),
				      PkgNames::CancelLabel().str() );
		    info.setNiceSize( 35, 10 );
		    NCursesEvent event =info.showInfoPopup();

		    if ( event == NCursesEvent::cancel )
		    {
			tryAgain = false;
		    }
		}
	    }
	}
	if ( !mountFloppy
	     || (mountFloppy && mounted) )
	{
	    // FIXME: write selection to file
	    // PathInfo pathInfo( pathName );
	    // int ret = pathInfo.copy( "/tmp/testfile", pathName );
	    // NCMIL << "Writing selection to: " << pathName << " returned: " << ret << endl;
	    NCMIL << "Writing selection to: " << pathName << " NOT yet implemented" << endl;
	}
	
	if ( mountFloppy )
	{
	    system( string( "/bin/umount /media/floppy >/dev/null 2>&1" ).c_str() ); 	    
	}
    }
    
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : NCPopupFile::loadSelection
//	METHOD TYPE : void
//
//
void NCPopupFile::loadFromFile()
{
    if ( headline && textLabel && okButton )
    {
	headline->setLabel( YCPString(PkgNames::LoadSelHeadline().str()) );
	string text = PkgNames::LoadSel2Text().str();
	textLabel->setText( YCPString(text) );
	okButton->setLabel( YCPString( PkgNames::LoadLabel().str() ) );
	setDefaultPath();
    }
    NCursesEvent event = showInfoPopup();

    if ( event == NCursesEvent::button )
    {
	NCPopupInfo info( wpos(2, 2),
			  YCPString( PkgNames::NotifyLabel().str() ),
			  YCPString( PkgNames::LoadSel1Text().str() ),
			  PkgNames::OKLabel().str(),
			  PkgNames::CancelLabel().str() );
	info.setNiceSize( 30, 8 );
	info.showInfoPopup();

	NCMIL << "Reading selection from: " << pathName << " NOT yet implemented" << endl;
    }
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : NCPopupFile::setDefaultPath
//	METHOD TYPE : void
//
//
const string YAST2PATH = "/usr/lib/YaST2/";
const string USERFILE = "user.sel";

void NCPopupFile::setDefaultPath()
{
    YCPString value = comboBox->getValue();

    if ( value->compare( YCPString(PkgNames::Harddisk().str()) ) == YO_EQUAL ) 
    {
	pathName = YAST2PATH + USERFILE;
	mountFloppy = false;
    }
    else
    {
	pathName = USERFILE;
	mountFloppy = true;
    }
    fileName->setText( YCPString(pathName) );
    
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : NCPopupFile::postAgain
//	METHOD TYPE : bool
//
//
bool NCPopupFile::postAgain()
{
    if ( ! postevent.widget )
	return false;

    YCPValue currentId = dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( currentId->compare( PkgNames::Cancel() ) == YO_EQUAL )
    {
	// close the dialog 
	postevent = NCursesEvent::cancel;
    }
    else if  ( currentId->compare( PkgNames::OkButton() ) == YO_EQUAL )
    {
	postevent = NCursesEvent::button;
	YCPString value = comboBox->getValue();

	YCPString path = fileName->getText();
	pathName = path->value();
	
	if ( value->compare( YCPString(PkgNames::Harddisk().str()) ) == YO_EQUAL )
	{
	    mountFloppy = false;
	}
	else
	{
	    mountFloppy = true;
	    pathName = "/media/floppy/" + pathName;
	}

    }

    if ( postevent == NCursesEvent::menu )
    {
	setDefaultPath();
    }
    
    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
        // return false means: close the popup dialog
	return false;
    }
    return true;
}

