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

   File:       NCAskForExistingDirectory.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"

#include "NCAskForExistingDirectory.h"

#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"
#include "NCFrame.h"

#include <Y2PM.h>
#include <y2pm/RpmDb.h>
#include <y2pm/PMSelectionManager.h>


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingDirectory::NCAskForExistingDirectory
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCAskForExistingDirectory::NCAskForExistingDirectory( const wpos at,
						      const YCPString & startDir,
						      const YCPString & headline )
    : NCPopup( at, true )
    , dir( 0 )
    , okButton( 0 )
    , cancelButton( 0 )
{
    createLayout( startDir, headline );

    fillDirectoryList( dir, startDir );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingDirectory::~NCAskForExistingDirectory
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCAskForExistingDirectory::~NCAskForExistingDirectory()
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingDirectory::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCAskForExistingDirectory::createLayout(const YCPString & startDir,  const YCPString & headline )
{

    YWidgetOpt opt;

    // the vertical split is the (only) child of the dialog
    NCSplit * split = new NCSplit( this, opt, YD_VERT );
    addChild( split );

    opt.notifyMode.setValue( false );

    //the headline
    opt.isHeading.setValue( true );

    NCLabel * head = new NCLabel( split, opt, headline );
    split->addChild( head );

    split->addChild( new NCSpacing( split, opt, 0.8, false, true ) );

    opt.isHStretchable.setValue( true );

    // the field for the selected directory
    dirName = new NCTextEntry( split, opt,
			       YCPString( "Selected directory" ),
			       YCPString( "" ),
			       100, 100 );
    split->addChild( dirName );
    
    // add the list of directories
    dir = new NCSelectionBox( split, opt, YCPString( "" ) );
    split->addChild( dir );

    split->addChild( new NCSpacing( split, opt, 0.4, false, true ) );

    // HBox for the buttons
    NCSplit * hSplit = new NCSplit( split, opt, YD_HORIZ );
    split->addChild( hSplit ); 
    opt.isHStretchable.setValue( true );
    hSplit->addChild( new NCSpacing( hSplit, opt, 0.2, true, false ) );

    // add the OK button
    opt.key_Fxx.setValue( 10 );
    okButton = new NCPushButton( hSplit, opt, YCPString(PkgNames::OKLabel()) );
    okButton->setId( PkgNames::OkButton() );

    hSplit->addChild( okButton );

    hSplit->addChild( new NCSpacing( hSplit, opt, 0.4, true, false ) );
      
    // add the Cancel button
    opt.key_Fxx.setValue( 9 );
    cancelButton = new NCPushButton( hSplit, opt, PkgNames::CancelLabel() );
    cancelButton->setId( PkgNames::Cancel() );

    hSplit->addChild( cancelButton );
    hSplit->addChild( new NCSpacing( hSplit, opt, 0.2, true, false ) );  
  
}

///////////////////////////////////////////////////////////////////
//
// NCursesEvent & showDirPopup ()
//
//
NCursesEvent & NCAskForExistingDirectory::showDirPopup( )
{
    postevent = NCursesEvent();

    if ( !dir )
	return postevent;

    dir->setKeyboardFocus();

    // event loop
    do {
	popupDialog();
    } while ( postAgain() );

    popdownDialog();

    // if OK is clicked get the current item as return value
    if ( postevent.detail == NCursesEvent::USERDEF )
    {
	postevent.result = YCPString( "/usr/src/packages" );
    }
    else
    {
	postevent.result = YCPNull();
    }

    return postevent;
}


//////////////////////////////////////////////////////////////////
//
// getCurrentLine()
//
// returns the currently selected li
//
string  NCAskForExistingDirectory::getCurrentLine( )
{
    if ( !dir )
	return "";

    int index = dir->getCurrentItem();

    NCMIL << "Current line: " << index << endl;
    
    return ( "" );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingDirectory::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCAskForExistingDirectory::nicesize(YUIDimension dim)
{
    return ( dim == YD_HORIZ ? 50 : 20 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCAskForExistingDirectory::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingDirectory::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCAskForExistingDirectory::postAgain( )
{
    if( !postevent.widget )
	return false;

    postevent.detail = NCursesEvent::NODETAIL;

    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( !currentId.isNull() )
    {
	if ( currentId->compare( PkgNames::OkButton () ) == YO_EQUAL )
	{
	    postevent.detail = NCursesEvent::USERDEF ;
	}

        // return false means: close the popup
	return false;
    }

    if (postevent == NCursesEvent::cancel)
	return false;

    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingDirectory::fillDirectoryList
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCAskForExistingDirectory::fillDirectoryList ( NCSelectionBox * dir,
						    const YCPString & startDir )
{
    if ( !dir )
	return false;

    return true;
}
