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

   File:       NCPopupTextEntry.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPopupTextEntry.h"


NCPopupTextEntry::NCPopupTextEntry( const wpos at,
				    const string & label,
				    const string & text,
				    unsigned maxInput,
				    unsigned maxFld,
				    NCInputField::FTYPE t )
	: NCPopup( at )
	, wtext( 0 )
{
    wtext = new NCInputField( this,
			      label,
			      false,	    // passwordMode = false
			      maxInput,
			      maxFld
			    );
    YUI_CHECK_NEW( wtext );

    wtext->setValue( text );
    wtext->setFldtype( t );
    wtext->setReturnOnReturn( true );

}


NCPopupTextEntry::~NCPopupTextEntry()
{
}


bool NCPopupTextEntry::postAgain()
{
    if ( postevent == NCursesEvent::button )
	postevent.detail = 0;

    return false;
}
