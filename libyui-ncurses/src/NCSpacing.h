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

   File:       NCSpacing.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCSpacing_h
#define NCSpacing_h

#include <iosfwd>

#include "YSpacing.h"
#include "YEmpty.h"
#include "YSpacing.h"
#include "NCWidget.h"


class NCSpacing : public YSpacing, public NCWidget
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCSpacing & OBJ );

    NCSpacing & operator=( const NCSpacing & );
    NCSpacing( const NCSpacing & );

    const char * l;

protected:

    virtual const char * location() const { return l; }

public:

    NCSpacing( YWidget * parent,
	       YUIDimension dim,
	       bool stretchable = false,
	       YLayoutSize_t layoutUnits = 0.0 );

    virtual ~NCSpacing();
    
    virtual int preferredWidth()  { return YSpacing::preferredWidth(); }
    virtual int preferredHeight() { return YSpacing::preferredHeight(); }

    virtual void setSize( int newWidth, int newHeight );

    virtual void setEnabled( bool do_bv );
};


#endif // NCSpacing_h
