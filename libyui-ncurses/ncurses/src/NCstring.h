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
