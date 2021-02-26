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

   File:       NCRadioButton.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCRadioButton_h
#define NCRadioButton_h

#include <iosfwd>

#include <yui/YRadioButton.h>
#include "NCWidget.h"

class NCRadioButton;


class NCRadioButton : public YRadioButton, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCRadioButton & OBJ );

    NCRadioButton & operator=( const NCRadioButton & );
    NCRadioButton( const NCRadioButton & );

    bool     checked;
    NClabel  label;

protected:

    virtual const char * location() const { return "NCRadioButton"; }

    virtual void wRedraw();

public:

    NCRadioButton( YWidget * parent,
		   const std::string & label,
		   bool checked );

    virtual ~NCRadioButton();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const std::string & nlabel );

    virtual void setValue( bool newval );
    virtual bool value() { return checked; }

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }
};


#endif // NCRadioButton_h
