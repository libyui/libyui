/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
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
|***************************************************************************/

/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCInputField.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCInputField_h
#define NCInputField_h

#include <iosfwd>

#include "YInputField.h"
#include "NCWidget.h"


class NCInputField : public YInputField, public NCWidget
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCInputField & OBJ );

    NCInputField & operator=( const NCInputField & );
    NCInputField( const NCInputField & );

public:

    enum FTYPE
    {
	PLAIN,
	NUMBER
    };

private:

    bool     passwd;
    NClabel  label;
    wstring   buffer;

    NCursesWindow * lwin;
    NCursesWindow * twin;

    unsigned maxFldLength;
    unsigned maxInputLength;

    unsigned fldstart;
    unsigned fldlength;
    unsigned curpos;

    FTYPE    fldtype;
    NCstring validChars;

    bool     returnOnReturn_b;

    void setDefsze();
    void tUpdate();

    bool     bufferFull() const;
    unsigned maxCursor() const;

    // specifies how much characters can be inserted. -1 for unlimited input
    int InputMaxLength;

protected:

    virtual const char * location() const { return "NCInputField"; }

    virtual void wCreate( const wrect & newrect );
    virtual void wDelete();

    virtual void wRedraw();

    bool validKey( wint_t key ) const;

public:

    NCInputField( YWidget * parent,
		  const string & label,
		  bool passwordMode = false,
		  unsigned maxInput = 0,
		  unsigned maxFld   = 0
		);
    virtual ~NCInputField();

    void setFldtype( FTYPE t )		 { fldtype = t; }

    void setReturnOnReturn( bool on_br ) { returnOnReturn_b = on_br; }

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    virtual void setValue( const std::string & ntext );
    virtual string value();

    virtual void setValidChars( const string & validchars );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    // limits  the input to numberOfChars characters and truncates the text
    // if appropriate
    void setInputMaxLength( int numberOfChars );

    void setCurPos( unsigned pos ) { curpos = pos; }
};


#endif // NCInputField_h
