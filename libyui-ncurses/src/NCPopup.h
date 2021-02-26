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

#ifndef NCPopup_h
#define NCPopup_h

#include <iosfwd>

#include "NCDialog.h"


class NCPopup : public NCDialog
{

    NCPopup & operator=( const NCPopup & );
    NCPopup( const NCPopup & );

protected:

    NCursesEvent postevent;

    void popupDialog();
    void popdownDialog();

    virtual NCursesEvent wHandleInput( wint_t ch );

    NCPopup( const wpos at, const bool boxed = true );
    virtual ~NCPopup();

    virtual bool postAgain()
    {
	if ( postevent != NCursesEvent::cancel )
	    postevent.detail = 0;

	return false;
    }

public:

    int post( NCursesEvent * returnevent = 0 );

};


#endif // NCPopup_h


