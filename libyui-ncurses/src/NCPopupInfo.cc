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

   File:       NCPopupInfo.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPopupInfo.h"

#include "NCTree.h"
#include <yui/YMenuButton.h>
#include <yui/YDialog.h>
#include "NCLayoutBox.h"
#include "NCSpacing.h"


namespace
{
    const std::string idOk( "ok" );
    const std::string idCancel( "cancel" );
}


NCPopupInfo::NCPopupInfo( const wpos & at,
			  const std::string & headline,
			  const std::string & text,
			  std::string okButtonLabel,
			  std::string cancelButtonLabel )
	: NCPopup( at, false )
	, helpText( 0 )
	, okButton( 0 )
	, cancelButton( 0 )
	, hDim( 50 )
	, vDim( 20 )
	, visible( false )
{
    createLayout( headline, text, okButtonLabel, cancelButtonLabel );
}


NCPopupInfo::~NCPopupInfo()
{
}


void NCPopupInfo::createLayout( const std::string & headline,
				const std::string & text,
				std::string okButtonLabel,
				std::string cancelButtonLabel )
{
    std::string old_textdomain = textdomain( NULL );
    setTextdomain( "ncurses" );

    // the vertical split is the (only) child of the dialog
    NCLayoutBox * split = new NCLayoutBox( this, YD_VERT );

    // add the headline
    new NCLabel( split, headline, true, false ); // isHeading = true

    // add the rich text widget
    helpText = new NCRichText( split, text );

    NCLayoutBox * hSplit = new NCLayoutBox( split, YD_HORIZ );

    if ( okButtonLabel != "" && cancelButtonLabel != "" )
    {
	new NCSpacing( hSplit, YD_HORIZ, true, 0.4 ); // stretchable = true
    }

    if ( okButtonLabel != "" )
    {
	// add the OK button
	okButton = new NCPushButton( hSplit, okButtonLabel );
	okButton->setFunctionKey( 10 );
    }

    if ( cancelButtonLabel != "" )
    {
	new NCSpacing( hSplit, YD_HORIZ, true, 0.4 );

	// add the Cancel button
	cancelButton = new NCPushButton( hSplit, cancelButtonLabel );
	cancelButton->setFunctionKey( 9 );

	new NCSpacing( hSplit, YD_HORIZ, true, 0.4 );
    }

    //If we don't have cancel button and have single ok button instead
    //let's focus it by default (#397393)
    if ( cancelButtonLabel == "" && okButton )
	focusOkButton();

    //the same with missing ok button and single cancel button
    if ( okButtonLabel == "" && cancelButton )
	focusCancelButton();

    // restore former text domain
    setTextdomain( old_textdomain.c_str() );
}


NCursesEvent & NCPopupInfo::showInfoPopup()
{
    postevent = NCursesEvent();

    do
    {
	popupDialog();
    }
    while ( postAgain() );

    popdownDialog();

    return postevent;
}


void NCPopupInfo::popup()
{
    initDialog();
    showDialog();
    activate( true );
    visible = true;
}


void NCPopupInfo::popdown()
{
    activate( false );
    closeDialog();
    visible = false;
}


int NCPopupInfo::preferredWidth()
{
    int horDim = hDim;

    if ( hDim >= NCurses::cols() )
	horDim = NCurses::cols() - 10;

    return horDim;
}


int NCPopupInfo::preferredHeight()
{
    int vertDim = vDim;

    if ( vDim >= NCurses::lines() )
	vertDim = NCurses::lines() - 5;

    return vertDim;
}


NCursesEvent
NCPopupInfo::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    if ( ch == KEY_RETURN )
	return NCursesEvent::button;

    return NCDialog::wHandleInput( ch );
}


bool NCPopupInfo::postAgain()
{
    if ( ! postevent.widget )
	return false;

    if ( okButton && cancelButton )
    {
	if ( postevent.widget == cancelButton )
	{
	    yuiMilestone() << "Cancel button pressed" << std::endl;
	    // close the dialog
	    postevent = NCursesEvent::cancel;
	}

	// else - nothing to do (postevent is already set)
    }

    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }

    return true;
}


