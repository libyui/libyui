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

   File:       NCFrame.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCFrame_h
#define NCFrame_h

#include <iosfwd>

#include <yui/YFrame.h>
#include "NCWidget.h"

class NCFrame;


class NCFrame : public YFrame, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCFrame & OBJ );

    NCFrame & operator=( const NCFrame & );
    NCFrame( const NCFrame & );


    NClabel label;

protected:

    bool gotBuddy();

    virtual const char * location() const { return "NCFrame"; }

    virtual void wRedraw();

public:

    NCFrame( YWidget * parent, const std::string & label );
    virtual ~NCFrame();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const std::string & nlabel );

    virtual void setEnabled( bool do_bv );
};


#endif // NCFrame_h
