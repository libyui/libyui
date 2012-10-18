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

   File:       NCCheckBoxFrame.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCCheckBoxFrame_h
#define NCCheckBoxFrame_h

#include <iosfwd>

#include <yui/YCheckBoxFrame.h>
#include "NCWidget.h"
#include "NCCheckBox.h"

class NCCheckBoxFrame;


class NCCheckBoxFrame : public YCheckBoxFrame, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCCheckBoxFrame & OBJ );

    NCCheckBoxFrame & operator=( const NCCheckBoxFrame & );
    NCCheckBoxFrame( const NCCheckBoxFrame & );


    NClabel label;
    bool isEnabled;

protected:

    bool gotBuddy();

    virtual const char * location() const { return "NCCheckBoxFrame"; }

    virtual void wRedraw();

public:

    NCCheckBoxFrame( YWidget * parent, const std::string & label,
		     bool checked );
    virtual ~NCCheckBoxFrame();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const std::string & nlabel );

    virtual void setEnabled( bool do_bv );

    virtual bool getValue() { return isEnabled; }

    // Do not forget to call Redraw(), so that UI::ChangeWidget works
    // correctly - bug #301370
    virtual void setValue( bool enable ) { isEnabled = enable; Redraw();}

    virtual bool setKeyboardFocus();

    virtual bool value();

    virtual NCursesEvent wHandleInput( wint_t key );

    bool getParentValue( NCWidget * widget, bool initial );
};


#endif // NCCheckBoxFrame_h
