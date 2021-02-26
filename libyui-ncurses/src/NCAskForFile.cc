/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

   File:       NCAskForFile.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>

#include "NCAskForFile.h"

#include <yui/YDialog.h>

#include "NCWidgetFactory.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCFrame.h"
#include "NCi18n.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/errno.h>

/*
  Textdomain "ncurses"
*/


NCAskForFile::NCAskForFile( const wpos at,
			    const std::string & iniDir,
			    const std::string & filter,
			    const std::string & headline )
    : NCPopup( at, true )
    , okButton( 0 )
    , cancelButton( 0 )
    , dirName( 0 )
    , dirList( 0 )
    , detailed( 0 )
    , fileList( 0 )
    , fileName( 0 )
{

}


NCAskForFile::~NCAskForFile( )
{

}


std::string NCAskForFile::checkIniDir( std::string iniDir )
{
    std::string dname = "";

    struct stat64 statInfo;
    stat64( iniDir.c_str(), &statInfo );

    if ( S_ISDIR( statInfo.st_mode ) )
    {
	dname = iniDir;
    }
    else
    {
	std::string::size_type pos;

	pos = iniDir.find_last_of( "/" );

	if ( pos != std::string::npos
	     && pos != 0 )
	{
	    std::string dir = iniDir.substr( 0, pos );
	    stat64( dir.c_str(), &statInfo );

	    if ( S_ISDIR( statInfo.st_mode ) )
	    {
		dname = dir;
		iniFileName  = iniDir.substr( pos + 1 );
	    }
	}
    }

    return dname;
}


void NCAskForFile::createLayout( const std::string & iniDir,
				 const std::string & filter,
				 const std::string & headline,
				 bool edit )
{
    std::string startDir;
    std::string old_textdomain = textdomain( NULL );
    setTextdomain( "ncurses" );

    startDir = checkIniDir( iniDir );

    // the vertical split is the (only) child of the dialog
    YLayoutBox * split = YUI::widgetFactory()->createVBox( this );

    new NCLabel( split, headline, true, false ); // isHeading = true

    YFrame * frame = YUI::widgetFactory()->createFrame( split, "" );

    // label for text field showing the selected dir
    dirName = new NCComboBox( frame, _( "Selected Directory:" ), false );  // editable = false;
    dirName->setNotify( true );
    dirName->setStretchable( YD_HORIZ, true );

    // add the checkBox detailed
    YLayoutBox * hSplit = YUI::widgetFactory()->createHBox( split );

    // label for checkbox
    detailed = new NCCheckBox( hSplit, _( "&Detailed View" ), false );
    detailed->setNotify( true );

    // HBox for the lists
    YLayoutBox * hSplit1 = YUI::widgetFactory()->createHBox( split );

    // create table header for table type T_Overview
    YTableHeader * dirHeader = new YTableHeader();
    dirHeader->addColumn( " " );
    dirHeader->addColumn( _( "Directory name" ) );

    // add the list of directories
    dirList = new NCDirectoryTable( hSplit1,
				    dirHeader,
				    NCFileSelection::T_Overview,
				    startDir );
    dirList->setSendKeyEvents( true );

    // create table header for table type T_Overview
    YTableHeader * fileHeader = new YTableHeader();
    fileHeader->addColumn( " " );
    fileHeader->addColumn( _( "File name" ) );

    // add the list of files
    fileList = new NCFileTable( hSplit1,
				fileHeader,
				NCFileSelection::T_Overview,
				filter,
				startDir );

    fileList->setSendKeyEvents( true );

    YLayoutBox * hSplit2 = YUI::widgetFactory()->createHBox( split );

    // opt.isEditable.setValue( edit );
    // NCInputField doesn't support mode 'not editable' any longer
    // -> an InputField IS editable

    // add the text entry for the file name
    fileName = new NCInputField( hSplit2,
				 // label for text field showing the filename
				 _( "&File name:" ),
				 false,		// passWordMode = false
				 100,
				 50 );
    fileName->setValue( iniFileName );

    // label for text field showing the filter (e.g. *.bak)
    NCComboBox * extension = new NCComboBox( hSplit2, _( "Filter:" ), false );	// editable = false
    extension->setStretchable( YD_HORIZ, true );
    extension->addItem( filter,
			true );	 // selected

    YUI::widgetFactory()->createSpacing( split, YD_VERT, false, 1.0 );

    // HBox for the buttons
    YLayoutBox * hSplit3 = YUI::widgetFactory()->createHBox( split );

    YUI::widgetFactory()->createSpacing( hSplit3, YD_HORIZ, true, 0.2 );  // stretchable = true

    // add the OK button
    okButton = new NCPushButton( hSplit3, _( "&OK" ) );
    okButton->setFunctionKey( 10 );
    okButton->setStretchable( YD_HORIZ, true );

    YUI::widgetFactory()->createSpacing( hSplit3, YD_HORIZ, true, 0.4 );

    // add the Cancel button
    cancelButton = new NCPushButton( hSplit3, _( "&Cancel" ) );
    cancelButton->setFunctionKey( 9 );
    cancelButton->setStretchable( YD_HORIZ, true );

    YUI::widgetFactory()->createSpacing( hSplit3, YD_HORIZ, true, 0.2 );
    // restore former text domain
    setTextdomain( old_textdomain.c_str() );
}


NCursesEvent & NCAskForFile::showDirPopup( )
{
    postevent = NCursesEvent();

    if ( !dirList || !fileList || !dirName )
	return postevent;

    dirList->fillList();
    fileList->fillList();
    dirList->setKeyboardFocus();
    dirName->addItem( dirList->getCurrentDir(),
		      true );		 // selected

    if ( iniFileName == "" )
	// show the currently selected file
	fileName->setValue( fileList->getCurrentFile() );

    // event loop
    do
    {
	popupDialog();
    }
    while ( postAgain() );

    popdownDialog();

    return postevent;
}


int NCAskForFile::preferredWidth()
{
    return  NCurses::cols() - 10;
}


int NCAskForFile::preferredHeight()
{
    return NCurses::lines() - 4;
}


NCursesEvent NCAskForFile::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}


void NCAskForFile::updateFileList()
{
    // set new start dir and show the file list
    fileList->setStartDir( dirList->getCurrentDir() );
    fileList->fillList( );

    if ( iniFileName == "" )
	// show the currently selected file
	fileName->setValue( fileList->getCurrentFile() );
}


bool NCAskForFile::postAgain( )
{
    if ( !postevent.widget )
	return false;

    postevent.detail = NCursesEvent::NODETAIL;

    if ( postevent.keySymbol == "CursorLeft" )
    {
	dirList->setKeyboardFocus();
	return true;
    }
    else if ( postevent.keySymbol == "CursorRight" )
    {
	fileList->setKeyboardFocus();
	fileName->setValue( fileList->getCurrentFile() );
	return true;
    }

    if ( postevent.widget == okButton )
    {
	postevent.result = dirList->getCurrentDir() + "/" + getFileName();
	// return false means: close the popup
	return false;
    }
    else if (( postevent.widget == dirList ) &&
	     ( postevent.result != "" )	)
    {
	// show the currently selected directory
	dirName->addItem( postevent.result,
			  true );
	updateFileList();

	if ( postevent.reason == YEvent::Activated )
	{
	    // fill directory and file list
	    dirList->fillList();
	    updateFileList();
	}
    }
    else if ( postevent.widget == dirName )
    {
	dirList->setStartDir( dirName->text() );
	dirList->fillList();

	updateFileList();
    }
    else if ( postevent.widget == detailed )
    {
	bool details = getCheckBoxValue( detailed );

	if ( details )
	{
	    fileList->setTableType( NCFileTable::T_Detailed );
	    dirList->setTableType( NCFileTable::T_Detailed );
	}
	else
	{
	    fileList->setTableType( NCFileTable::T_Overview );
	    dirList->setTableType( NCFileTable::T_Overview );
	}

	fileList->fillList();

	dirList->fillList();
    }
    else if ( postevent.widget == fileList )
    {
	if ( postevent.result != "" )
	{
	    fileName->setValue( postevent.result );
	}
    }
    else
    {
	postevent.result = "";
	return false;
    }

    if ( postevent.widget == cancelButton ||
	 postevent == NCursesEvent::cancel )
    {
	postevent.result = "";
	return false;
    }

    return true;
}


bool NCAskForFile::getCheckBoxValue( NCCheckBox * checkBox )
{
    if ( checkBox )
    {
	// return whether the option is selected or not
	return ( checkBox->isChecked() );
    }

    return false;
}


NCAskForExistingFile::NCAskForExistingFile( const wpos at,
					    const std::string & iniDir,
					    const std::string & filter,
					    const std::string & headline )
    : NCAskForFile( at, iniDir, filter, headline )
{
    createLayout( iniDir,
		  filter,
		  headline,
		  false );	// file name is not editable
}


std::string NCAskForExistingFile::getFileName()
{
    if ( fileName->value() == "" )
	return fileList->getCurrentFile();
    else
	return fileName->value();
}


NCAskForSaveFileName::NCAskForSaveFileName( const wpos at,
					    const std::string & iniDir,
					    const std::string & filter,
					    const std::string & headline )
    : NCAskForFile( at, iniDir, filter, headline )
{
    createLayout( iniDir,
		  filter,
		  headline,
		  true );	// file name is editable
}


std::string NCAskForSaveFileName::getFileName()
{
    return fileName->value();
}
