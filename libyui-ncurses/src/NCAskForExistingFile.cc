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

   File:       NCAskForExistingFile.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"

#include "NCAskForExistingFile.h"

#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"
#include "NCFrame.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/errno.h>


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingFile::NCAskForExistingFile
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCAskForExistingFile::NCAskForExistingFile( const wpos at,
					    const YCPString & iniDir,
					    const YCPString & headline )
    : NCPopup( at, true )
    , okButton( 0 )
    , cancelButton( 0 )
    , dirName( 0 )
    , dirList( 0 )
    , fileList( 0 )
    , detailed ( 0 )
{
    createLayout( iniDir, headline );

    dirList->fillList();
    fileList->fillList();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingFile::~NCAskForExistingFile
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCAskForExistingFile::~NCAskForExistingFile()
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingFile::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCAskForExistingFile::createLayout( const YCPString & iniDir,
					      const YCPString & headline )
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

    split->addChild( new NCSpacing( split, opt, 0.4, false, true ) );

    opt.isHStretchable.setValue( true );

    NCFrame * frame = new NCFrame( split, opt, YCPString("" ) );
    NCSplit * vSplit = new NCSplit( frame, opt, YD_VERT );

    opt.isEditable.setValue( false );
    opt.notifyMode.setValue( true );
    dirName = new NCComboBox( frame, opt, YCPString("Selected directory:") );
    frame->addChild( dirName );

    dirName->setId( PkgNames::DirName() );

    dirName->itemAdded( YCPString( iniDir ), // set initial value
			0,		 // index
			true );		 // selected
    vSplit->addChild( new NCSpacing( vSplit, opt, 0.6, false, true ) );

    split->addChild( frame );

    // add the checkBox detailed
    NCSplit * hSplit = new NCSplit( split, opt, YD_HORIZ );
    split->addChild( hSplit );
    detailed = new NCCheckBox( hSplit, opt, YCPString( "Details view" ), false );
    detailed->setId( PkgNames::Details() );
    hSplit->addChild( new NCSpacing( hSplit, opt, 0.1, true, false ) );
    hSplit->addChild( detailed );

    // HBox for the lists
    NCSplit * hSplit1 = new NCSplit( split, opt, YD_HORIZ );
    
    // add the list of directories
    dirList = new NCDirectoryTable( hSplit1, opt, NCFileSelection::T_Overview, iniDir );
    dirList->setId( PkgNames::DirList() );
    hSplit1->addChild( dirList );
    
    // add the list of files
    fileList = new NCFileTable( hSplit1, opt, NCFileSelection::T_Overview, iniDir );
    fileList->setId( PkgNames::FileList() );
    hSplit1->addChild( fileList );
    
    split->addChild( hSplit1 );
    opt.notifyMode.setValue( false );
    
    // add the text entry for the file name
    fileName = new NCTextEntry( split, opt,
			       YCPString( "File name:" ),
			       YCPString( "" ),
			       100, 100 );
    split->addChild( fileName );
    
    split->addChild( new NCSpacing( split, opt, 0.8, false, true ) );

    // HBox for the buttons
    NCSplit * hSplit2 = new NCSplit( split, opt, YD_HORIZ );
    split->addChild( hSplit2 ); 
    opt.isHStretchable.setValue( true );
    hSplit2->addChild( new NCSpacing( hSplit2, opt, 0.2, true, false ) );

    // add the OK button
    opt.key_Fxx.setValue( 10 );
    okButton = new NCPushButton( hSplit2, opt, YCPString(PkgNames::OKLabel()) );
    okButton->setId( PkgNames::OkButton() );

    hSplit2->addChild( okButton );

    hSplit2->addChild( new NCSpacing( hSplit2, opt, 0.4, true, false ) );
      
    // add the Cancel button
    opt.key_Fxx.setValue( 9 );
    cancelButton = new NCPushButton( hSplit2, opt, PkgNames::CancelLabel() );
    cancelButton->setId( PkgNames::Cancel() );

    hSplit2->addChild( cancelButton );
    hSplit2->addChild( new NCSpacing( hSplit2, opt, 0.2, true, false ) );  
  
}

///////////////////////////////////////////////////////////////////
//
// NCursesEvent & showDirPopup ()
//
//
NCursesEvent & NCAskForExistingFile::showDirPopup( )
{
    postevent = NCursesEvent();

    if ( !dirList )
	return postevent;

    dirList->setKeyboardFocus();

    // event loop
    do {
	popupDialog();
    } while ( postAgain() );

    popdownDialog();


    return postevent;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingFile::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//


long NCAskForExistingFile::nicesize(YUIDimension dim)
{
    return ( dim == YD_HORIZ ? 70 : 20 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCAskForExistingFile::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingFile::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCAskForExistingFile::postAgain( )
{
    if( !postevent.widget )
	return false;

    postevent.detail = NCursesEvent::NODETAIL;

    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();
    NCMIL << "Current ID: " << currentId->toString() << endl;

    if ( !currentId.isNull() )
    {
	if ( currentId->compare( PkgNames::OkButton () ) == YO_EQUAL )
	{
	    postevent.result = YCPString( dirList->getCurrentDir() );
	    // return false means: close the popup
	    return false;
	}
	else if ( currentId->compare( PkgNames::DirList() ) == YO_EQUAL )
	{
	    unsigned int i = dirName->getListSize();
	    // show the currently selected directory
	    dirName->itemAdded( postevent.result->asString(),
				i,
				true );
	    
	    fileList->setStartDir( dirList->getCurrentDir() );
	    // show the corresponding file list
	    fileList->fillList( );
	    
	    if ( postevent.reason == YEvent::Activated )
	    {
		bool details = getCheckBoxValue( detailed );
		if ( details )
		{
		    dirList->setTableType( NCFileTable::T_Detailed );
		}
		else
		{
		    dirList->setTableType( NCFileTable::T_Overview );
		}
		// fill directory list
		dirList->fillList();
	    }
	}
	else if ( currentId->compare( PkgNames::DirName() ) == YO_EQUAL )
	{
	    dirList->setStartDir( dirName->getValue() );
	    dirList->fillList();
	}
	else if ( currentId->compare( PkgNames::Details() ) == YO_EQUAL )
	{
	    bool details = getCheckBoxValue( detailed );
	    if ( details )
	    {
		dirList->setTableType( NCFileTable::T_Detailed );
	    }
	    else
	    {
		dirList->setTableType( NCFileTable::T_Overview );
	    }
	    // fill directory list
	    dirList->fillList(); 
	}
	else
	{
	    postevent.result = YCPNull();
	    return false;
	}
    }

    if (postevent == NCursesEvent::cancel)
    {
	postevent.result = YCPNull();	
	return false;
    }
    
    return true;
}



bool NCAskForExistingFile::getCheckBoxValue( NCCheckBox * checkBox )
{
    YCPValue value = YCPNull();

    if ( checkBox )
    {
	value = checkBox->getValue();

	// return whether the option is selected or not
	if ( !value.isNull() )
	{
	    return ( value->asBoolean()->toString() == "true" ? true : false );
	}
    }

    return false;
}
