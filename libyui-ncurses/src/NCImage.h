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

   File:       NCImage.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCImage_h
#define NCImage_h

#include <iosfwd>

#include <yui/YImage.h>
#include "NCWidget.h"

class NCImage;


class NCImage : public YImage, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & str, const NCImage & obj );

    NCImage & operator=( const NCImage & );
    NCImage( const NCImage & );


    NClabel label;

protected:

    virtual const char * location() const { return "NCImage"; }

public:

    NCImage( YWidget * parent, std::string defaulttext, bool animated = false );
    virtual ~NCImage();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setEnabled( bool do_bv );
};


#endif // NCImage_h
