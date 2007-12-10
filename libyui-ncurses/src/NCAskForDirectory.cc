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

#include "ycp/y2log.h"

#include "NCAskForDirectory.h"

#include "YWidgetID.h"
#include "YDialog.h"
#include "YTypes.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCFrame.h"
#include "NCi18n.h"

#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/errno.h>

/*
  Textdomain "packages"
*/

namespace
{
    const string idOk( "ok" );
    const string idCancel( "cancel" );
    const string idDirList( "dirlist" );
    const string idDirName( "dirname" );
    const string idDetails( "details" );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAskForExistingDirectory::NCAskForExistingDirectory
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCAskForExistingDirectory::NCAskForExistingDirectory( const wpos at,
						      const string & iniDir,
						      const string & headline )
    : NCPopup( at, true )
    , okButton( 0 )
    , cancelButton( 0 )
    , dirName( 0 )
    , dirList( 0 )
    , detailed ( 0 )
{
    createLayout( iniDir, headline );
    setTextdomain( "packages" );
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
void NCAskForExistingDirectory::createLayout( const string & iniDir,
					      const string & headline )
{
    // the vertical split is the (only) child of the dialog
    NCLayoutBox * split = new NCLayoutBox( this, YD_VERT );

    // the headline
    new NCLabel( split, headline, true, false );	// isHeading = true

    NCFrame * frame = new NCFrame( split, "" );
    
    // label for text field showing the selected dir
    dirName = new NCComboBox( frame, _( "Selected Directory:" ), false ); // editable = false
    dirName->setNotify( true );
    dirName->setStretchable( YD_HORIZ, true );

#warning is this widget ID really needed?
    YStringWidgetID * dirID = new YStringWidgetID( idDirName );
    dirName->setId( dirID );

    // add the checkBox detailed
    NCLayoutBox * hSplit = new NCLayoutBox( split, YD_HORIZ );

    // label for checkbox 
    detailed = new NCCheckBox( hSplit, _( "&Detailed View" ), false );
#warning is this widget ID really needed?
    YStringWidgetID * detailsID = new YStringWidgetID( idDetails );
    detailed->setId( detailsID );
    detailed->setNotify( true );
    
    // create table header for table type T_Overview
    YTableHeader * tableHeader = new YTableHeader();
    tableHeader->addColumn( " ", YAlignBegin );
    tableHeader->addColumn( _( "Directory Name" ), YAlignBegin );
    
    // add the list of directories
    dirList = new NCDirectoryTable( split,
				    tableHeader,
				    NCFileTable::T_Overview,
				    iniDir );

#warning is this widget ID really needed?
    YStringWidgetID * dirListID = new YStringWidgetID( idDirList );
    dirList->setId( dirListID );

    new NCSpacing( split, YD_VERT, false, 1.0 );

    // HBox for the buttons
    NCLayoutBox * hSplit1 = new NCLayoutBox( split, YD_HORIZ );

    new NCSpacing( hSplit1, YD_HORIZ, true, 0.2 );	// stretchable = true

    // add the OK button
    okButton = new NCPushButton( hSplit1, _( "&OK" ) );
    okButton->setFunctionKey( 10 );
    okButton->setStretchable( YD_HORIZ, true );
    
#warning is this widget ID really needed?
    YStringWidgetID * okID = new YStringWidgetID ( idOk );
    okButton->setId( okID );

    new NCSpacing( hSplit1, YD_HORIZ, true, 0.4 );
      
    // add the Cancel button
    cancelButton = new NCPushButton( hSplit1, _( "&Cancel" ) );
    cancelButton->setFunctionKey( 9 );
    cancelButton->setStretchable( YD_HORIZ, true);
    
#warning is this widget ID really needed?
    YStringWidgetID * cancelID = new YStringWidgetID (idCancel );
    cancelButton->setId( cancelID );

    new NCSpacing( hSplit1, YD_HORIZ, true, 0.2 );  
  
}

///////////////////////////////////////////////////////////////////
//
// NCursesEvent & showDirPopup ()
//
//
NCursesEvent & NCAskForExistingDirectory::showDirPopup( )
{
    postevent = NCursesEvent();

    if ( !dirList || !dirName )
	return postevent;

    dirList->fillList( );
    dirList->setKeyboardFocus();

    dirName->addItem( dirList->getCurrentDir(),
		      true );		// selected
    
    // event loop
    do {
	popupDialog();
    } while ( postAgain() );

    popdownDialog();


    return postevent;
}

int NCAskForExistingDirectory::preferredWidth()
{
    return NCurses::cols()-10;
}

int NCAskForExistingDirectory::preferredHeight()
{
    return NCurses::lines()-4;
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

#warning compare widget pointers here, not IDs
    YWidgetID * currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( currentId )
    {
	if ( currentId->toString() == idOk )
	{
	    postevent.result = dirList->getCurrentDir();
	    // return false means: close the popup
	    return false;
	}
	else if ( currentId->toString() == idDirList )
	{
	    if ( postevent.result == "" )
		return true;
		
            // show the currently selected directory
	    NCDBG << "Add item: " <<  postevent.result << endl;

	    dirName->addItem( postevent.result,
			      true );
	    
	    if ( postevent.reason == YEvent::Activated )
	    {
		// fill the directory list
		dirList->fillList();
	    }
	}
	else if ( currentId->toString() == idDirName )
	{
	    dirList->setStartDir( dirName->value() );
	    dirList->fillList();
	}
	else if ( currentId->toString() == idDetails )
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
	    dirList->fillList(); 
	}
	else
	{
	    postevent.result = "";
	    return false;
	}
    }

    if (postevent == NCursesEvent::cancel)
    {
	postevent.result = "";	
	return false;
    }
    
    return true;
}



bool NCAskForExistingDirectory::getCheckBoxValue( NCCheckBox * checkBox )
{
    if ( checkBox )
    {
	// return whether the option is selected or not
	return ( checkBox->isChecked() );
    }

    return false;
}
