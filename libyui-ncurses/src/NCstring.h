/*************************************************************************************************************

 Copyright (C) 2000 - 2010 Novell, Inc.   All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation; either version 2 of the License, or (at your
 option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*************************************************************************************************************/



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

   File:       NCstring.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCstring_h
#define NCstring_h

#include <iosfwd>
#include <string>

using std::string;
using std::wstring;


class NCstring
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCstring & OBJ );


    mutable wchar_t hotk;		// hotkey
    mutable wstring::size_type hotp;	// position of hotkey
    mutable wstring   wstr;

    static string	termEncoding;	// the encoding of the terminal

public:

    NCstring();

    NCstring( const NCstring & nstr );

    NCstring( const wstring & wstr );

    NCstring( const string & str );

    NCstring( const char * cstr );

    ~NCstring() {}

    string Str() const;

public:

    NCstring & operator=( const NCstring & nstr );

    NCstring & operator+=( const NCstring & nstr );

    const wstring & str()      const { return wstr; }

private:

    friend class NClabel;
    wchar_t	       hotkey() const { return hotk; }

    wstring::size_type hotpos() const { return hotp; }

public:

    static bool RecodeToWchar( const string& in, const string & from_encoding, wstring* out );
    static bool RecodeFromWchar( const wstring & in, const string & to_encoding, string* out );

    static const string & terminalEncoding()
    {
	return termEncoding;
    }

    static bool setTerminalEncoding( const string & encoding = "" );

    void getHotkey() const;
};


#endif // NCstring_h
