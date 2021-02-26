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

   File:       NCTextPad.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCTextPad_h
#define NCTextPad_h

#include <iosfwd>
#include <list>

#include "NCPad.h"
#include "NCtext.h"


class NCTextPad : public NCPad
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCTextPad & OBJ );

    NCTextPad & operator=( const NCTextPad & );
    NCTextPad( const NCTextPad & );

private:

    std::list<unsigned>	     lines;
    std::list<unsigned>::iterator cline;

    wpos curs;
    bool curson;

    void cursor( bool on );
    void cursorOn()  { cursor( true ); }

    void cursorOff() { cursor( false ); }

    bool insert( wint_t key );
    bool delch( bool previous = false );
    bool openLine();

    void assertSze( wsze minsze );
    void assertWidth( unsigned minw );
    void assertHeight( unsigned minh );

    // specifies how much characters can be inserted. -1 for unlimited input
    int InputMaxLength;

protected:

    virtual int setpos( const wpos & newpos );

public:

    NCTextPad( int lines, int cols, const NCWidget & p );
    virtual ~NCTextPad();

public:

    virtual void resize( wsze nsze );
    virtual int setpos();
    virtual wpos CurPos() const;
    virtual bool handleInput( wint_t key );

    void setText( const NCtext & ntext );
    std::wstring getText() const;

    // limits  the input to numberOfChars characters and truncates the text
    // if appropriate
    void setInputMaxLength( int nr );
};


#endif // NCTextPad_h
