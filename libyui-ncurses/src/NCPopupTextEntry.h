/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
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
|***************************************************************************/

/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

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

    NCPopupTextEntry( const wpos at,
		      const string & label,
		      const string & text,
		      unsigned maxInput  = 0,
		      unsigned maxFld	 = 0,
		      NCInputField::FTYPE t = NCInputField::PLAIN );

    virtual ~NCPopupTextEntry();

    void   setValue( const string & ntext ) { wtext->setValue( ntext ); }

    string value() { return wtext->value(); }
};



#endif // NCPopupTextEntry_h
