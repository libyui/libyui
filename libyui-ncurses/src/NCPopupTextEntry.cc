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

   File:       NCPopupTextEntry.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPopupTextEntry.h"
#include "NCInputField.h"


NCPopupTextEntry::NCPopupTextEntry( const wpos at,
				    const std::string & label,
				    const std::string & text,
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


std::string NCPopupTextEntry::askForText( const wpos at,
                                          const std::string & label,
                                          const std::string & text,
                                          unsigned maxInput,
                                          unsigned maxFld )
{
    NCPopupTextEntry * dialog = new NCPopupTextEntry( at, label, text, maxInput, maxFld,
						      NCInputField::PLAIN);
    YUI_CHECK_NEW( dialog );
    std::string result;

    dialog->post();
    result = dialog->value();
    YDialog::deleteTopmostDialog();

    return result;
}
