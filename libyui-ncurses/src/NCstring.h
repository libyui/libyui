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

   File:       NCstring.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCstring_h
#define NCstring_h

#include <iosfwd>
#include <string>


class NCstring
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCstring & OBJ );


    mutable wchar_t hotk;		// hotkey
    mutable std::wstring::size_type hotp;	// position of hotkey
    mutable std::wstring   wstr;

    static std::string	termEncoding;	// the encoding of the terminal

public:

    NCstring();

    NCstring( const NCstring & nstr );

    NCstring( const std::wstring & wstr );

    NCstring( const std::string & str );

    NCstring( const char * cstr );

    ~NCstring() {}

    std::string Str() const;

public:

    NCstring & operator=( const NCstring & nstr );

    NCstring & operator+=( const NCstring & nstr );

    const std::wstring & str()      const { return wstr; }

private:

    friend class NClabel;
    wchar_t	       hotkey() const { return hotk; }

    std::wstring::size_type hotpos() const { return hotp; }

public:

    static bool RecodeToWchar( const std::string& in, const std::string & from_encoding, std::wstring* out );
    static bool RecodeFromWchar( const std::wstring & in, const std::string & to_encoding, std::string* out );

    static const std::string & terminalEncoding()
    {
	return termEncoding;
    }

    static bool setTerminalEncoding( const std::string & encoding = "" );

    void getHotkey() const;
};


#endif // NCstring_h
