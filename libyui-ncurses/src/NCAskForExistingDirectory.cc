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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/errno.h>

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingDirectory::NCAskForExistingDirectory
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCAskForExistingDirectory::NCAskForExistingDirectory( const wpos at,
						      const YCPString & iniDir,
						      const YCPString & headline )
    : NCPopup( at, true )
    , okButton( 0 )
    , cancelButton( 0 )
    , dirName( 0 )
    , dirList( 0 )
    , currentDir( iniDir->value() )
    , startDir( iniDir->value() ) 
{
    createLayout( headline );

    fillDirectoryList( );
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
void NCAskForExistingDirectory::createLayout( const YCPString & headline )
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
    dirList = new NCSelectionBox( split, opt, YCPString( "" ) );
    split->addChild( dirList );

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

    if ( !dirList )
	return postevent;

    dirList->setKeyboardFocus();

    // event loop
    do {
	popupDialog();
    } while ( postAgain() );

    popdownDialog();

    // if OK is clicked return the current directory
    if ( postevent.detail == NCursesEvent::USERDEF )
    {
	postevent.result = YCPString( currentDir );
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
    if ( !dirList )
	return "";

    int index = dirList->getCurrentItem();
    
    return ( (index != -1)?dirList->getLine(index):"" );
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

    NCursesEvent retEvent = NCDialog::wHandleInput( ch );

    string selected = getCurrentLine();

    if ( selected != ".." )
    {
	if ( startDir != "/" )
	{
	    currentDir = startDir + "/" + selected;
	}
	else
	{
	    currentDir = startDir + selected;
	}
    }
    else
    {
	size_t pos;
	if ( ( pos = currentDir.find_last_of("/") ) != 0 )
	{
	    currentDir = currentDir.substr( 0, pos );
	}
	else
	{
	    currentDir = "/";
	}
    }
	
    if ( ch == KEY_UP || ch == KEY_DOWN)
    {
	dirName->setText( currentDir );
    }

    if ( ch == KEY_RETURN )
    {
	NCMIL << "get dir list of: " << currentDir << endl;
	bool ok = fillDirectoryList( );

	if ( ok )
	{
	    startDir = currentDir;		// set new start directory 
	}

	dirName->setText( currentDir );	
    }
    return retEvent;
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
bool NCAskForExistingDirectory::fillDirectoryList ( )
{
    if ( !dirList )
	return false;

    struct stat 	statInfo;
    struct dirent *	entry;
    int n = 0;
    DIR * diskDir = opendir( currentDir.c_str() );

    // if the directory does not exist or is empty or is not a directory:
    // start with working directory
    if ( diskDir == NULL
	 && ( errno == ENOENT || errno == ENOTDIR ) )
    {
	// call `pwd`
    }

    dirName->setText( YCPString( currentDir ) );
	
    if ( ( diskDir = opendir( currentDir.c_str() ) ) )
    {
	dirList->clearTable();
	while ( ( entry = readdir( diskDir ) ) )
	{
	    string entryName = entry->d_name;

	    if ( entryName != "." )
	    {
		string fullName = currentDir + "/" + entryName;
		if ( lstat( fullName.c_str(), &statInfo ) == 0 )
		{
		    if ( S_ISDIR( statInfo.st_mode ) )
		    {
			NCMIL << "entryName: " << entryName << " currentDir: " << currentDir << endl;
			if ( ( entryName == ".." && currentDir != "/" )
			     || entryName != ".." )
			{
			    dirList->itemAdded( YCPString(entryName), n, false );
			}
			n++;
		    }
		}
	    }
	}
    }
    else
    {
	NCERR << "ERROR opening directory: " << currentDir << " errno: " << strerror( errno ) << endl;
	return false;
    }

    closedir( diskDir );
    
    return true;
}
