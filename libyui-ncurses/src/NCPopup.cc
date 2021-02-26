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

   File:       NCPopup.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPopup.h"


NCPopup::NCPopup( const wpos & at, bool boxed )
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
