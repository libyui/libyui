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



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////   __/\\\\\\_____________/\\\__________/\\\________/\\\___/\\\________/\\\___/\\\\\\\\\\\_           ////
 ////    _\////\\\____________\/\\\_________\///\\\____/\\\/___\/\\\_______\/\\\__\/////\\\///__          ////
 ////     ____\/\\\______/\\\__\/\\\___________\///\\\/\\\/_____\/\\\_______\/\\\______\/\\\_____         ////
 ////      ____\/\\\_____\///___\/\\\_____________\///\\\/_______\/\\\_______\/\\\______\/\\\_____        ////
 ////       ____\/\\\______/\\\__\/\\\\\\\\\_________\/\\\________\/\\\_______\/\\\______\/\\\_____       ////
 ////        ____\/\\\_____\/\\\__\/\\\////\\\________\/\\\________\/\\\_______\/\\\______\/\\\_____      ////
 ////         ____\/\\\_____\/\\\__\/\\\__\/\\\________\/\\\________\//\\\______/\\\_______\/\\\_____     ////
 ////          __/\\\\\\\\\__\/\\\__\/\\\\\\\\\_________\/\\\_________\///\\\\\\\\\/_____/\\\\\\\\\\\_    ////
 ////           _\/////////___\///___\/////////__________\///____________\/////////______\///////////__   ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                 widget abstraction library providing Qt, GTK and ncurses frontends                  ////
 ////                                                                                                     ////
 ////                                   3 UIs for the price of one code                                   ////
 ////                                                                                                     ////
 ////                                      ***  NCurses plugin  ***                                       ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                              (C) SUSE Linux GmbH    ////
 ////                                                                                                     ////
 ////                                                              libYUI-AsciiArt (C) 2012 Björn Esser   ////
 ////                                                                                                     ////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

using std::list;


class NCTextPad : public NCPad
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCTextPad & OBJ );

    NCTextPad & operator=( const NCTextPad & );
    NCTextPad( const NCTextPad & );

private:

    list<unsigned>	     lines;
    list<unsigned>::iterator cline;

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
    wstring getText() const;

    // limits  the input to numberOfChars characters and truncates the text
    // if appropriate
    void setInputMaxLength( int nr );
};


#endif // NCTextPad_h
