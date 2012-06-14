/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/


/*-/

   File:       NCCheckBox.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCCheckBox_h
#define NCCheckBox_h

#include <iosfwd>

#include <yui/YCheckBox.h>
#include "NCWidget.h"


class NCCheckBox : public YCheckBox, public NCWidget
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCCheckBox & OBJ );

    NCCheckBox & operator=( const NCCheckBox & );
    NCCheckBox( const NCCheckBox & );

protected:

    enum State
    {
	S_DC  = 0,
	S_OFF = 1,
	S_ON  = 2
    };

private:

    static unsigned char statetag[3];

    bool     tristate;
    State    checkstate;
    NClabel  label;

protected:

    virtual const char * location() const { return "NCCheckBox"; }

    virtual void wRedraw();

public:

    NCCheckBox( YWidget * parent,
		const string & label,
		bool checked );
    virtual ~NCCheckBox();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    virtual void setValue( YCheckBoxState state );

    virtual YCheckBoxState value();

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }
};


#endif // NCCheckBox_h
