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

   File:       NCPkgPopupFile.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPkgPopupFile.h"

#include "NCTree.h"
#include "NCFrame.h"
#include "NCPopupInfo.h"
#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "NCPkgNames.h"
#include "NCPackageSelector.h"

//#include <y2pm/zypp::PackageImEx.h>
#include "y2util/PathInfo.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupFile::NCPkgPopupFile
//	METHOD TYPE : Constructor
//
NCPkgPopupFile::NCPkgPopupFile( const wpos at, string device,  NCPackageSelector * pkger )
    : NCPopup( at, false )
    , headline ( 0 )
    , textLabel( 0 )
    , okButton( 0 )
    , cancelButton( 0 )
    , fileName( 0 )
    , comboBox( 0 )
    , packager( pkger )
    , pathName( "" )
    , mountFloppy( true )
    , floppyDevice( device )
{
    createLayout( );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupFile::~NCPkgPopupFile
//	METHOD TYPE : Destructor
//
NCPkgPopupFile::~NCPkgPopupFile()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupFile::createLayout
//	METHOD TYPE : void
//
void NCPkgPopupFile::createLayout( )
{
    YWidgetOpt opt;

    // the vertical split is the (only) child of the dialog
    NCSplit * split = new NCSplit( this, opt, YD_VERT );
    addChild( split );

    // add the headline
    opt.isHeading.setValue( true );
    headline = new NCLabel( split, opt, YCPString("") );
    split->addChild( headline );

    split->addChild( new NCSpacing( split, opt, 0.4, false, true ) );

    // add the text
    opt.isHeading.setValue( false );
    opt.isVStretchable.setValue( true );
    textLabel = new NCRichText( split, opt, YCPString(NCPkgNames::SaveSelText()) );
    split->addChild( textLabel );

    // add a frame
    opt.isVStretchable.setValue( true );
    NCFrame * frame = new NCFrame( split, opt, YCPString("") );
    NCSplit * vSplit2 = new NCSplit( frame, opt, YD_VERT );

    // the combo box for selecting the medium
    opt.isHStretchable.setValue( true );
    comboBox = new NCComboBox( vSplit2, opt, YCPString(NCPkgNames::MediumLabel()) );
    comboBox->itemAdded( YCPString(NCPkgNames::Floppy()), 0, true );
    comboBox->itemAdded( YCPString(NCPkgNames::Harddisk()), 0, false );
    vSplit2->addChild( comboBox );

    // the text entry field for the file name
    fileName = new NCTextEntry( vSplit2, opt,
				YCPString(NCPkgNames::FileName()),
				YCPString( "" ),
				100, 100 );
    vSplit2->addChild( fileName );
    frame->addChild( vSplit2 );
    split->addChild( frame );

    split->addChild( new NCSpacing( split, opt, 0.4, false, true ) );

    // HBox for the buttons
    NCSplit * hSplit = new NCSplit( split, opt, YD_HORIZ );
    split->addChild( hSplit );

    opt.isHStretchable.setValue( true );
    hSplit->addChild( new NCSpacing( hSplit, opt, 0.2, true, false ) );

    // add the OK button
    opt.key_Fxx.setValue( 10 );
    okButton = new NCPushButton( hSplit, opt, YCPString( "" ) );
    okButton->setId( NCPkgNames::OkButton() );

    hSplit->addChild( okButton );

    hSplit->addChild( new NCSpacing( hSplit, opt, 0.4, true, false ) );

    // add the Cancel button
    opt.key_Fxx.setValue( 9 );
    cancelButton = new NCPushButton( hSplit, opt, YCPString(NCPkgNames::CancelLabel()) );
    cancelButton->setId( NCPkgNames::Cancel() );

    hSplit->addChild( cancelButton );
    hSplit->addChild( new NCSpacing( hSplit, opt, 0.2, true, false ) );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : NCPkgPopupFile::showFilePopup
//	METHOD TYPE : NCursesEvent &
//
NCursesEvent & NCPkgPopupFile::showFilePopup( )
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
//	METHOD NAME : NCPkgPopupFile::niceSize
//	METHOD TYPE : void
//
//
long NCPkgPopupFile::nicesize(YUIDimension dim)
{
    long vdim;
    if ( NCurses::lines() > 13 )
	vdim = 13;
    else
	vdim = NCurses::lines()-4;
	
    return ( dim == YD_HORIZ ? NCurses::cols()/2 : vdim );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//
NCursesEvent NCPkgPopupFile::wHandleInput( wint_t ch )
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

bool NCPkgPopupFile::mountDevice( string device, string errText )
{
    bool mounted = false;
    bool tryAgain = true;
    while ( !mounted && tryAgain )
    {
	int exitCode = system( string( "/bin/mount " + device +
				       " /media/floppy" + " >/dev/null 2>&1" ).c_str() );
	if ( exitCode == 0 )
	{
	    NCMIL << device << " mounted on /media/floppy" << endl;
	    mounted = true;
	}
	else
	{
	    NCERR << "mount " << device << " exit code: " << exitCode << endl;
	}

	if ( !mounted )
	{
	    NCPopupInfo info1( wpos(2, 2),
			       YCPString( NCPkgNames::ErrorLabel() ),
			       YCPString( errText ),
			       NCPkgNames::OKLabel(),
			       NCPkgNames::CancelLabel() );
	    info1.setNiceSize( 35, 10 );
	    NCursesEvent event =info1.showInfoPopup();

	    if ( event == NCursesEvent::cancel )
	    {
		tryAgain = false;
	    }
	}
    }

    return mounted;
}

void NCPkgPopupFile::unmount()
{
    int result = system( string( "/bin/umount /media/floppy >/dev/null 2>&1" ).c_str() );

    if ( result != 0 )
    {
	NCPopupInfo info2( wpos(2, 2),
			   YCPString( NCPkgNames::ErrorLabel() ),
			   YCPString( "\"/bin/umount /media/floppy\" failed !" ) );
	info2.setNiceSize( 35, 10 );
	info2.showInfoPopup();
	NCERR << "Umount /media/floppy failed !" << endl;
    }
    else
    {
	NCMIL << "/media/floppy unmounted succesfully" << endl;
    }
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : NCPkgPopupFile::saveSelection
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgPopupFile::saveToFile()
{
    if ( headline && textLabel && okButton )
    {
	headline->setLabel( YCPString( NCPkgNames::SaveSelHeadline() ) );
	textLabel->setText( YCPString( NCPkgNames::SaveSelText() ) );
	okButton->setLabel( YCPString( NCPkgNames::SaveLabel() ) );
	setDefaultPath();
    }
    // show the save selection popup
    NCursesEvent event = showFilePopup();

    if ( event == NCursesEvent::button )
    {
	NCPopupInfo saveInfo( wpos(5, 5),  YCPString( "" ),
			      YCPString(NCPkgNames::Saving()) );
	saveInfo.setNiceSize( 18, 4 );
	saveInfo.popup();
	bool mounted = false;

	// if the medium is a floppy mount the device
	if ( !mountFloppy
	     || (mounted = mountDevice( floppyDevice, NCPkgNames::SaveErr1Text()) ) )
	{
#ifdef FIXME
	    zypp::PackageImEx pkgExport;

	    // remember the current Package/SelectionManagers state.
	    pkgExport.getPMState();

	    // write package selection to file
	    if ( ! pkgExport.doExport( pathName ) )
	    {
		NCPopupInfo info2( wpos(2, 2),
				   YCPString( NCPkgNames::ErrorLabel() ),
				   YCPString( NCPkgNames::SaveErr2Text() ) );
		info2.setNiceSize( 35, 10 );
		info2.showInfoPopup();
		NCERR << "Error: could not write selection to: " << pathName << endl;
	    }
	    else
	    {
		NCMIL << "Writing selection to: " << pathName << endl;
	    }
#endif
	}
	if ( mounted )
	{
	    unmount();
	}

	saveInfo.popdown();
    }
}



///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : NCPkgPopupFile::loadSelection
//	METHOD TYPE : void
//
//
void NCPkgPopupFile::loadFromFile()
{
    if ( headline && textLabel && okButton )
    {
	headline->setLabel( YCPString(NCPkgNames::LoadSelHeadline()) );
	string text = NCPkgNames::LoadSel2Text();
	textLabel->setText( YCPString(text) );
	okButton->setLabel( YCPString( NCPkgNames::LoadLabel() ) );
	setDefaultPath();
    }
    // show the load selection popup
    NCursesEvent event = showFilePopup();

    if ( event == NCursesEvent::button )
    {
	// show popup "Really overwrite current selection?"
	NCPopupInfo info1( wpos(2, 2),
			   YCPString( NCPkgNames::NotifyLabel() ),
			   YCPString( NCPkgNames::LoadSel1Text() ),
			   NCPkgNames::OKLabel(),
			   NCPkgNames::CancelLabel() );
	info1.setNiceSize( 30, 8 );
	NCursesEvent event = info1.showInfoPopup();

	if ( event == NCursesEvent::button )
	{
	    NCPopupInfo loadInfo( wpos(5, 5),  YCPString( "" ),
				  YCPString(NCPkgNames::Loading()) );
	    loadInfo.setNiceSize( 18, 4 );
	    loadInfo.popup();
	    bool mounted = false;

	    if ( !mountFloppy
		 || (mounted = mountDevice( floppyDevice, NCPkgNames::LoadErr1Text())) )
	    {
#ifdef FIXME
		zypp::PackageImEx pkgImport;

		// read selection from file
		if ( ! pkgImport.doImport( pathName) )
		{
		    NCPopupInfo info2( wpos(2, 2),
				       YCPString( NCPkgNames::ErrorLabel() ),
				       YCPString( NCPkgNames::LoadErr2Text() ) );
		    info2.setNiceSize( 35, 10 );
		    info2.showInfoPopup();
		    NCERR << "Could not load selection from: " << pathName << endl;
		}
		else
		{
		    // restore Package/SelectionManagers state according to the
		    pkgImport.setPMState();
		    NCMIL << "Package selection loaded from: " << pathName << endl;
		    // show package dependencies (in case dependency check in on)
		    packager->showPackageDependencies(false);
		    // always show selection dependencies
		    packager->showSelectionDependencies();
		}
#endif
	    }
	    if ( mounted )
	    {
		unmount();
	    }

    	    loadInfo.popdown();
	}
    }
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : NCPkgPopupFile::setDefaultPath
//	METHOD TYPE : void
//
//
const string YAST2PATH = "/var/lib/YaST2/";
const string USERFILE = "user.sel";

void NCPkgPopupFile::setDefaultPath()
{
    YCPString value = comboBox->getValue();

    if ( value->compare( YCPString(NCPkgNames::Harddisk()) ) == YO_EQUAL )
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
//	METHOD NAME : NCPkgPopupFile::postAgain
//	METHOD TYPE : bool
//
//
bool NCPkgPopupFile::postAgain()
{
    if ( ! postevent.widget )
	return false;

    YCPValue currentId = dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( currentId->compare( NCPkgNames::Cancel() ) == YO_EQUAL )
    {
	// close the dialog
	postevent = NCursesEvent::cancel;
    }
    else if  ( currentId->compare( NCPkgNames::OkButton() ) == YO_EQUAL )
    {
	postevent = NCursesEvent::button;
	YCPString value = comboBox->getValue();

	YCPString path = fileName->getText();
	pathName = path->value();

	if ( value->compare( YCPString(NCPkgNames::Harddisk()) ) == YO_EQUAL )
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

