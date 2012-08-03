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

   File:       NCIntField.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCIntField_h
#define NCIntField_h

#include <iosfwd>

#include <yui/YIntField.h>
#include "NCWidget.h"


class NCIntField : public YIntField, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCIntField & OBJ );

    NCIntField & operator=( const NCIntField & );
    NCIntField( const NCIntField & );


    static const unsigned taglen;

    NClabel label;
    NCursesWindow * lwin;
    NCursesWindow * twin;

    int      cvalue;
    unsigned vlen;
    unsigned vstart;

    void setDefsze();
    void tUpdate();

protected:

    virtual const char * location() const { return "NCIntField"; }

    virtual void wCreate( const wrect & newrect );
    virtual void wDelete();

    virtual void wRedraw();

    bool Increment( const bool bigstep = false );
    bool Decrement( const bool bigstep = false );

    int enterPopup( wchar_t first = L'\0' );

public:

    NCIntField( YWidget * parent,
		const std::string & label,
		int minValue, int maxValue,
		int initialValue );
    virtual ~NCIntField();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const std::string & nlabel );

    virtual void setValueInternal( int newValue );

    virtual int value() { return cvalue; }

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    virtual void setEnabled( bool do_bv );
};


#endif // NCIntField_h
