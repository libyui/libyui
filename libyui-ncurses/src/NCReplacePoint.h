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

   File:       NCReplacePoint.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCReplacePoint_h
#define NCReplacePoint_h

#include <iosfwd>

#include <yui/YReplacePoint.h>
#include "NCWidget.h"

class NCReplacePoint;


class NCReplacePoint : public YReplacePoint, public NCWidget
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCReplacePoint & OBJ );

    NCReplacePoint & operator=( const NCReplacePoint & );
    NCReplacePoint( const NCReplacePoint & );

protected:

    virtual const char * location() const { return "NCReplacePoint"; }

public:

    NCReplacePoint( YWidget * parent );
    virtual ~NCReplacePoint();

    virtual int preferredWidth() { return YReplacePoint::preferredWidth(); }
    virtual int preferredHeight() { return YReplacePoint::preferredHeight(); }

    virtual void setSize( int newWidth, int newHeight );

    virtual void setEnabled( bool do_bv );
};


#endif // NCReplacePoint_h
