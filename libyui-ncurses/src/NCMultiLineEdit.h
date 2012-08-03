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

   File:       NCMultiLineEdit.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCMultiLineEdit_h
#define NCMultiLineEdit_h

#include <iosfwd>

#include <yui/YMultiLineEdit.h>
#include "NCPadWidget.h"
#include "NCTextPad.h"


class NCMultiLineEdit : public YMultiLineEdit, public NCPadWidget
{
private:
    friend std::ostream & operator<<( std::ostream & STREAM, const NCMultiLineEdit & OBJ );

    NCMultiLineEdit & operator=( const NCMultiLineEdit & );
    NCMultiLineEdit( const NCMultiLineEdit & );

    NCstring ctext;

protected:

    /**
     * Overload myPad to narrow the type
     */
    virtual NCTextPad * myPad() const
    { return dynamic_cast<NCTextPad*>( NCPadWidget::myPad() ); }

protected:

    virtual const char * location() const { return "NCMultiLineEdit"; }

    virtual void wRedraw();

    virtual NCPad * CreatePad();
    virtual void    DrawPad();

public:

    NCMultiLineEdit( YWidget * parent, const std::string & label );

    virtual ~NCMultiLineEdit();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const std::string & nlabel );
    virtual void setValue( const std::string & ntext );

    virtual std::string value();

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    // sets the maximum number of characters of the NCTextPad
    void setInputMaxLength( int numberOfChars );

};


#endif // NCMultiLineEdit_h
