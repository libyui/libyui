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
    , detailed ( 0 )
    , currentDir( iniDir->value() )
    , startDir( iniDir->value() ) 
{
    createLayout( headline );

    if ( iniDir->value().empty() )
    {
	size_t bufSize	= 1024 * sizeof(char);	
	char wDir[bufSize+1];
    
	// start with working directory
	if ( getcwd( wDir, bufSize ) )
	{
	    startDir = wDir;
	    currentDir = wDir; 
	}
	else
	{
	    startDir = "/";
	    currentDir = "/";
	}
    }
    
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

    split->addChild( new NCSpacing( split, opt, 0.4, false, true ) );

    opt.isHStretchable.setValue( true );

    NCFrame * frame = new NCFrame( split, opt, YCPString("" ) );
    NCSplit * vSplit = new NCSplit( frame, opt, YD_VERT );

    opt.isEditable.setValue( false );
    dirName = new NCComboBox( frame, opt, YCPString("Selected directory:") );
    frame->addChild( dirName );

    dirName->setId( PkgNames::SearchBox() );	// FIXME

    dirName->itemAdded( YCPString( "" ), // set initial value
			0,		 // index
			false );	 // not selected
    vSplit->addChild( new NCSpacing( vSplit, opt, 0.6, false, true ) );

    split->addChild( frame );

    // add the checkBox detailed
    NCSplit * hSplit = new NCSplit( split, opt, YD_HORIZ );
    split->addChild( hSplit );
    detailed = new NCCheckBox( hSplit, opt, YCPString( "Details view" ), false );
    hSplit->addChild( new NCSpacing( hSplit, opt, 0.1, true, false ) );
    hSplit->addChild( detailed );
    
    // add the list of directories
    dirList = new NCFileTable( split, opt, NCFileTable::T_Overview );
    split->addChild( dirList );

    split->addChild( new NCSpacing( split, opt, 0.4, false, true ) );

    // HBox for the buttons
    NCSplit * hSplit1 = new NCSplit( split, opt, YD_HORIZ );
    split->addChild( hSplit1 ); 
    opt.isHStretchable.setValue( true );
    hSplit1->addChild( new NCSpacing( hSplit1, opt, 0.2, true, false ) );

    // add the OK button
    opt.key_Fxx.setValue( 10 );
    okButton = new NCPushButton( hSplit1, opt, YCPString(PkgNames::OKLabel()) );
    okButton->setId( PkgNames::OkButton() );

    hSplit1->addChild( okButton );

    hSplit1->addChild( new NCSpacing( hSplit1, opt, 0.4, true, false ) );
      
    // add the Cancel button
    opt.key_Fxx.setValue( 9 );
    cancelButton = new NCPushButton( hSplit1, opt, PkgNames::CancelLabel() );
    cancelButton->setId( PkgNames::Cancel() );

    hSplit1->addChild( cancelButton );
    hSplit1->addChild( new NCSpacing( hSplit1, opt, 0.2, true, false ) );  
  
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
// returns the currently selected line 
//
string  NCAskForExistingDirectory::getCurrentLine( )
{
    if ( !dirList )
	return "";

    int index = dirList->getCurrentItem();
    
    if ( index != -1 )
    {
	NCFileInfo info = dirList->getFileInfo( index );
	return info._name;
    }
    else
    {
	return "";
    }
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
NCursesEvent NCAskForExistingDirectory::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    unsigned int old_pos = dirList->getCurrentItem();

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
    unsigned int i = dirName->getListSize();;
    
    switch ( ch )
    {
	case KEY_UP:
	case KEY_PPAGE:
	case KEY_HOME: {
	    if ( old_pos != 0 )
	    {
		dirName->itemAdded( YCPString(currentDir),
				    i,
				    true );
	    }
	    break;
	}
	case KEY_DOWN:
	case KEY_NPAGE:
	case KEY_END: {
	    dirName->itemAdded( YCPString(currentDir),
				    i,
				    true );
	    break;
	}
	case KEY_RETURN: {
	    
	    bool ok = fillDirectoryList( );

	    if ( ok )
	    {
		startDir = currentDir;		// set new start directory 
		dirName->itemAdded( YCPString(currentDir),
				    i,
				    true );
	    }
	    break;
	}
	default:
	    ;
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
    struct stat		linkInfo;
    struct dirent *	entry;
    list<string>	tmpList;
    list<string>::iterator   it;

    bool details = getCheckBoxValue( detailed );

    if ( details )
    {
	dirList->fillHeader();
	dirList->setTableType( NCFileTable::T_Detailed );
    }
    else
    {
	dirList->setTableType( NCFileTable::T_Overview );
    }
	
    DIR * diskDir = opendir( currentDir.c_str() );
    unsigned int i = dirName->getListSize();
    
    dirName->itemAdded( YCPString(currentDir),
			i,
			true );
	
    if ( ( diskDir = opendir( currentDir.c_str() ) ) )
    {
	dirList->itemsCleared();
	while ( ( entry = readdir( diskDir ) ) )
	{
	    string entryName = entry->d_name;
	    if ( entryName != "." )
	    {
		tmpList.push_back( entryName );
	    }
	}
	
	// sort the list and fill the table widget with directory entries
	tmpList.sort( );
	it = tmpList.begin();
	while ( it != tmpList.end() )
	{
	    string fullName = currentDir + "/" + (*it);
	    if ( lstat( fullName.c_str(), &statInfo ) == 0 )
	    {
		if ( S_ISDIR( statInfo.st_mode ) )
		{
		    if ( ( (*it) == ".." && currentDir != "/" )
			 || (*it) != ".." )
		    {
			dirList->createListEntry( NCFileInfo( (*it), &statInfo ) );
		    }
		}
		else if ( S_ISLNK( statInfo.st_mode ) )
		{
		    if ( stat( fullName.c_str(), &linkInfo ) == 0 )
		    {
			if ( S_ISDIR( linkInfo.st_mode ) )
			{
			    dirList->createListEntry( NCFileInfo( (*it), &statInfo ) );
			}
		    }
		}
	    }	
	    ++it;
	}
	dirList->drawList();
	
    }
    else
    {
	NCERR << "ERROR opening directory: " << currentDir << " errno: "
	      << strerror( errno ) << endl;
	return false;
    }
    closedir( diskDir );
    
    return true;
}

bool NCAskForExistingDirectory::getCheckBoxValue( NCCheckBox * checkBox )
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
