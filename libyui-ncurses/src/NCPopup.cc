/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/


/*-/

   File:       NCPopup.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPopup.h"


NCPopup::NCPopup( const wpos at, const bool boxed )
	: NCDialog( YPopupDialog, at, boxed )
{
}


NCPopup::~NCPopup()
{
}


void NCPopup::popupDialog()
{
    initDialog();
    showDialog();
    activate( true );

    while ( !( postevent = userInput() ) )
	;

    activate( false );
}


void NCPopup::popdownDialog()
{
    closeDialog();
}


NCursesEvent NCPopup::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}


int NCPopup::post( NCursesEvent * returnevent )
{
    postevent = NCursesEvent();

    do
    {
	popupDialog();
    }
    while ( postAgain() );

    popdownDialog();

    if ( returnevent )
	*returnevent = postevent;

    yuiDebug() << "Return event.detail:  " << postevent.detail << std::endl;

    return postevent.detail;
}
