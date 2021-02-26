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

   File:       NCAskForExistingDirectory.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#include "NCAskForDirectory.h"


#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include <yui/YDialog.h>
#include <yui/YTypes.h>
#include "NCWidgetFactory.h"
#include "NCWidgetFactory.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCFrame.h"
#include "NCi18n.h"

#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

/*
  Textdomain "ncurses"
*/



NCAskForExistingDirectory::NCAskForExistingDirectory( const wpos & at,
						      const std::string & iniDir,
						      const std::string & headline )
    : NCPopup( at, true )
    , okButton( 0 )
    , cancelButton( 0 )
    , dirName( 0 )
    , dirList( 0 )
    , detailed( 0 )
{
    createLayout( iniDir, headline );
}


NCAskForExistingDirectory::~NCAskForExistingDirectory()
{
}


void NCAskForExistingDirectory::createLayout( const std::string & iniDir,
					      const std::string & headline )
{
    std::string old_textdomain = textdomain( NULL );
    setTextdomain( "ncurses" );

    // the vertical split is the (only) child of the dialog
    YLayoutBox * split = YUI::widgetFactory()->createVBox( this );

    // the headline
    new NCLabel( split, headline, true, false );	// isHeading = true

    YFrame * frame = YUI::widgetFactory()->createFrame( split, "" );

    // label for text field showing the selected dir
    dirName = new NCComboBox( frame, _( "Selected Directory:" ), false ); // editable = false
    dirName->setNotify( true );
    dirName->setStretchable( YD_HORIZ, true );

    // add the checkBox detailed
    YLayoutBox * hSplit = YUI::widgetFactory()->createHBox( split );

    // label for checkbox
    detailed = new NCCheckBox( hSplit, _( "&Detailed View" ), false );
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

    YUI::widgetFactory()->createSpacing( split, YD_VERT, false, 1.0 );

    // HBox for the buttons
    YLayoutBox * hSplit1 = YUI::widgetFactory()->createHBox( split );

    YUI::widgetFactory()->createSpacing( hSplit1, YD_HORIZ, true, 0.2 );	// stretchable = true

    // add the OK button
    okButton = new NCPushButton( hSplit1, _( "&OK" ) );
    okButton->setFunctionKey( 10 );
    okButton->setStretchable( YD_HORIZ, true );

    YUI::widgetFactory()->createSpacing( hSplit1, YD_HORIZ, true, 0.4 );

    // add the Cancel button
    cancelButton = new NCPushButton( hSplit1, _( "&Cancel" ) );
    cancelButton->setFunctionKey( 9 );
    cancelButton->setStretchable( YD_HORIZ, true );

    YUI::widgetFactory()->createSpacing( hSplit1, YD_HORIZ, true, 0.2 );
    // restore former text domain
    setTextdomain( old_textdomain.c_str() );
}


NCursesEvent & NCAskForExistingDirectory::showDirPopup()
{
    postevent = NCursesEvent();

    if ( !dirList || !dirName )
	return postevent;

    dirList->fillList();

    dirList->setKeyboardFocus();

    dirName->addItem( dirList->getCurrentDir(),
		      true );		// selected

    // event loop
    do
    {
	popupDialog();
    }
    while ( postAgain() );

    popdownDialog();


    return postevent;
}


int NCAskForExistingDirectory::preferredWidth()
{
    return NCurses::cols() - 10;
}


int NCAskForExistingDirectory::preferredHeight()
{
    return NCurses::lines() - 4;
}


NCursesEvent NCAskForExistingDirectory::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}


bool NCAskForExistingDirectory::postAgain()
{
    if ( !postevent.widget )
	return false;

    postevent.detail = NCursesEvent::NODETAIL;

    if ( postevent.widget == okButton )
    {
	postevent.result = dirList->getCurrentDir();
	// return false means: close the popup
	return false;
    }
    else if ( postevent.widget == dirList )
    {
	if ( postevent.result == "" )
	    return true;

	// show the currently selected directory
	yuiDebug() << "Add item: " <<  postevent.result << std::endl;

	dirName->addItem( postevent.result,
			  true );

	if ( postevent.reason == YEvent::Activated )
	{
	    // fill the directory list
	    dirList->fillList();
	}
    }
    else if ( postevent.widget == dirName )
    {
	dirList->setStartDir( dirName->value() );
	dirList->fillList();
    }
    else if ( postevent.widget == detailed )
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

    if ( postevent.widget == cancelButton ||
	 postevent == NCursesEvent::cancel )
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
