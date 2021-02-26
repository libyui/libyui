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

   File:       NCButtonBox.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCButtonBox_h
#define NCButtonBox_h

#include <iosfwd>

#include <yui/YButtonBox.h>
#include "NCWidget.h"

class NCButtonBox;


class NCButtonBox : public YButtonBox, public NCWidget
{

public:

    NCButtonBox( YWidget * parent );
    virtual ~NCButtonBox();

    virtual void moveChild( YWidget * child, int newX, int newY );
    virtual void setSize( int newWidth, int newHeight );
    virtual void setEnabled( bool enabled );

private:

    friend std::ostream & operator<<( std::ostream & stream, const NCButtonBox & widget );

    NCButtonBox & operator=( const NCButtonBox & );
    NCButtonBox( const NCButtonBox & );

};


#endif // NCButtonBox_h
