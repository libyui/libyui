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


/// A string with an optional hot key.
///
/// The current implementation stores a std::wstring.
class NCstring
{
private:

    friend std::ostream & operator<<( std::ostream & str, const NCstring & obj );

    mutable wchar_t hotk;                 ///< hotkey
    /// Position of hotkey in columns(!). *npos* means unset.
    ///
    /// CJK characters take 2 columns. So in the Chinese "Yes" button,
    /// L"是(Y)", *hotp* for L'Y' is 3 even though
    ///its wchar_t offset is 2 (and its UTF-8 offset is 4).
    mutable std::wstring::size_type hotp;
    mutable std::wstring   wstr;	  ///< the actual string

    /// The encoding of the terminal; the value is ignored by this class.
    /// WTF, really: other classes care
    /// but this class just uses UTF-8 for non-wide characters.
    static std::string	termEncoding;

public:

    NCstring();

    NCstring( const NCstring & nstr );

    NCstring( const std::wstring & wstr );

    /// Init from a UTF-8 string.
    NCstring( const std::string & str );

    /// Init from a UTF-8 string.
    NCstring( const char * cstr );

    ~NCstring() {}

    /// Get a UTF-8 string.
    std::string Str() const;

public:

    NCstring & operator=( const NCstring & nstr );

    NCstring & operator+=( const NCstring & nstr );

    const std::wstring & str()      const { return wstr; }

private:

    friend class NClabel;

    wchar_t	            hotkey() const { return hotk; }
    std::wstring::size_type hotpos() const { return hotp; }

public:

    static bool RecodeToWchar  ( const std::string & in,  const std::string & from_encoding, std::wstring * out );
    static bool RecodeFromWchar( const std::wstring & in, const std::string & to_encoding,   std::string  * out );

    static const std::string & terminalEncoding()
    {
	return termEncoding;
    }

    static bool setTerminalEncoding( const std::string & encoding = "" );

    /// (mutates the const object)
    void getHotkey() const;
};


#endif // NCstring_h
