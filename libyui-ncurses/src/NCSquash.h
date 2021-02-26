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

   File:       NCSquash.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCSquash_h
#define NCSquash_h

#include <iosfwd>

#include "YSquash.h"
#include "NCWidget.h"


class NCSquash : public YSquash, public NCWidget
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCSquash & OBJ );

private:
    
    NCSquash & operator=( const NCSquash & );
    NCSquash( const NCSquash & );

protected:

    virtual const char * location() const { return "NCSquash"; }

public:

    NCSquash( YWidget * parent, bool hsquash, bool vsquash );
    virtual ~NCSquash();

    virtual int preferredWidth() { return YSquash::preferredWidth(); }
    virtual int preferredHeight() { return YSquash::preferredHeight(); }

    virtual void setSize( int newWidth, int newHeight );

    virtual void setEnabled( bool do_bv );
};


#endif // NCSquash_h
