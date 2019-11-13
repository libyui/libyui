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

   File:       NCLabel.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCLabel_h
#define NCLabel_h

#include <iosfwd>

#include <yui/YLabel.h>
#include "NCWidget.h"

class NCLabel;


class NCLabel : public YLabel, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & str, const NCLabel & obj );

    NCLabel & operator=( const NCLabel & );
    NCLabel( const NCLabel & );


    bool    heading;
    NClabel label;

protected:

    virtual const char * location() const { return "NCLabel"; }

    virtual void wRedraw();

public:

    NCLabel( YWidget * parent,
	     const std::string & text,
	     bool isHeading = false,
	     bool isOutputField = false );

    virtual ~NCLabel();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setText( const std::string & nlabel );

    virtual void setEnabled( bool do_bv );
};


#endif // NCLabel_h
