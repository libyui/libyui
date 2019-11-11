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

   File:       NCLayoutBox.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCLayoutBox_h
#define NCLayoutBox_h

#include <iosfwd>

#include <yui/YLayoutBox.h>
#include "NCWidget.h"

class NCLayoutBox;


class NCLayoutBox : public YLayoutBox, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & str, const NCLayoutBox & obj );

    NCLayoutBox & operator=( const NCLayoutBox & );
    NCLayoutBox( const NCLayoutBox & );


protected:

    virtual const char * location() const
    {
	return primary() == YD_HORIZ ? "NC(H)LayoutBox" : "NC(V)LayoutBox" ;
    }

public:

    NCLayoutBox( YWidget * parent, YUIDimension dimension );
    virtual ~NCLayoutBox();

    virtual int preferredWidth() { return YLayoutBox::preferredWidth(); }
    virtual int preferredHeight() { return YLayoutBox::preferredHeight(); }

    virtual void setSize( int newWidth, int newHeight );

    virtual void moveChild( YWidget * child, int newx, int newy );

    virtual void setEnabled( bool do_bv );
};


#endif // NCLayoutBox_h
