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

   File:       NCPopupTextEntry.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCPopupTextEntry_h
#define NCPopupTextEntry_h

#include <iosfwd>

#include "NCPopup.h"
#include "NCInputField.h"



class NCPopupTextEntry : public NCPopup
{
private:

    NCPopupTextEntry & operator=( const NCPopupTextEntry & );
    NCPopupTextEntry( const NCPopupTextEntry & );


    NCInputField * wtext;

    virtual bool postAgain();

public:

    NCPopupTextEntry( const wpos & at,
		      const std::string & label,
		      const std::string & text,
		      unsigned maxInput  = 0,
		      unsigned maxFld	 = 0,
		      NCInputField::FTYPE t = NCInputField::PLAIN );

    virtual ~NCPopupTextEntry();

    void   setValue( const std::string & ntext ) { wtext->setValue( ntext ); }

    std::string value() { return wtext->value(); }

    static std::string askForText( const wpos & at,
                                   const std::string & label,
                                   const std::string & text = "",
                                   unsigned maxInput = 0,
                                   unsigned maxFld   = 0 );
};



#endif // NCPopupTextEntry_h
