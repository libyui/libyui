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

   File:       NCstring.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#include <errno.h>
#include <iconv.h>
#include <malloc.h>

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCstring.h"


// The default encoding is UTF-8. For real terminals this may be
// changed with setConsoleFont().

std::string	NCstring::termEncoding( "UTF-8" );






NCstring:: NCstring()
	: hotk( 0 )
	, hotp( std::wstring::npos )
	, wstr( L"" )

{
}



NCstring::NCstring( const NCstring & nstr )
	: hotk( nstr.hotk )
	, hotp( nstr.hotp )
	, wstr( nstr.wstr )
{
}



NCstring::NCstring( const std::wstring & widestr )
	: hotk( 0 )
	, hotp( std::wstring::npos )
	, wstr( widestr )
{
}



NCstring::NCstring( const std::string & str )
	: hotk( 0 )
	, hotp( std::wstring::npos )
{
    bool ok = RecodeToWchar( str, "UTF-8", &wstr );

    if ( !ok )
    {
	yuiError() << "ERROR: RecodeToWchar() failed" << std::endl;
    }
}



NCstring::NCstring( const char * cstr )
	: hotk( 0 )
	, hotp( std::wstring::npos )
{
    bool ok = RecodeToWchar( cstr, "UTF-8", &wstr );

    if ( !ok )
    {
	yuiError() << "ERROR: RecodeToWchar() failed" << std::endl;
    }
}



std::ostream & operator<<( std::ostream & str, const NCstring & obj )
{
    return str <<  obj.Str() ;
}



NCstring & NCstring::operator=( const NCstring & nstr )
{
    if ( &nstr != this )
    {
	hotk	  = nstr.hotk;
	hotp	  = nstr.hotp;
	wstr	  = nstr.wstr;
    }

    return *this;
}



NCstring & NCstring::operator+=( const NCstring & nstr )
{
    wstr.append( nstr.wstr );
    return *this;
}

static iconv_t fromwchar_cd	= ( iconv_t )( -1 );
static std::string  to_name		= "";



bool NCstring::RecodeFromWchar( const std::wstring & in, const std::string & to_encoding, std::string* out )
{
    iconv_t cd = ( iconv_t )( -1 );
    static bool complained = false;
    *out = "";

    if ( in.length() == 0 )
	return true;

    // iconv_open not yet called
    if ( fromwchar_cd == ( iconv_t )( -1 )
	 || to_name != to_encoding )
    {
	if ( fromwchar_cd != ( iconv_t )( -1 ) )
	{
	    iconv_close( fromwchar_cd );
	}

	fromwchar_cd = iconv_open( to_encoding.c_str(), "WCHAR_T" );

	yuiDebug() << "iconv_open( " << to_encoding.c_str() << ", \"WCHAR_T\" )" << std::endl;

	if ( fromwchar_cd == ( iconv_t )( -1 ) )
	{
	    if ( !complained )
	    {
		yuiError() << "ERROR: iconv_open failed" << std::endl;
		complained = true;
	    }

	    return false;
	}
	else
	{
	    to_name = to_encoding;
	}
    }

    cd = fromwchar_cd;		// set iconv handle

    size_t in_len = in.length() * sizeof( std::wstring::value_type );	// number of in bytes
    char* in_ptr = ( char * )in.data();

    size_t tmp_size = ( in_len * sizeof( char ) ) * 2;
    // tmp buffer size: in_len bytes * 2, that means 1 wide charatcer (4 Byte) can be transformed
    // into an encoding which needs at most 8 Byte for one character (should be enough)

    char* tmp = ( char* ) malloc( tmp_size + sizeof( char ) );

    do
    {

	char *tmp_ptr = tmp;
	size_t tmp_len = tmp_size;
	*(( char* ) tmp_ptr ) = '\0';

	size_t iconv_ret = iconv( cd, &in_ptr, &in_len, &tmp_ptr, &tmp_len );

	*(( char* ) tmp_ptr ) = '\0';
	*out += std::string( tmp );

	if ( iconv_ret == ( size_t )( -1 ) )
	{
	    if ( !complained )
	    {
		yuiError() << "ERROR iconv: " << errno << std::endl;
		complained = true;
	    }

	    if ( errno == EINVAL || errno == EILSEQ )
	    {
		*out += '?';
	    }

	    in_ptr += sizeof( std::wstring::value_type );

	    in_len -= sizeof( std::wstring::value_type );
	}

    }
    while ( in_len != 0 );

    free( tmp );

    return true;
}

static iconv_t towchar_cd	= ( iconv_t )( -1 );
static std::string  from_name	= "";



bool NCstring::RecodeToWchar( const std::string& in, const std::string &from_encoding, std::wstring* out )
{
    iconv_t cd = ( iconv_t )( -1 );
    static bool complained = false;
    *out = L"";

    if ( in.length() == 0 )
	return true;

    // iconv_open not yet called
    if ( towchar_cd == ( iconv_t )( -1 )
	 || from_name != from_encoding )
    {
	if ( towchar_cd != ( iconv_t )( -1 ) )
	{
	    iconv_close( towchar_cd );
	}

	towchar_cd = iconv_open( "WCHAR_T", from_encoding.c_str() );

	yuiDebug() << "iconv_open( \"WCHAR_T\", " << from_encoding.c_str() << " )" << std::endl;

	if ( towchar_cd == ( iconv_t )( -1 ) )
	{
	    if ( !complained )
	    {
		yuiError() << "Error: RecodeToWchar iconv_open() failed" << std::endl;
		complained = true;
	    }

	    return false;
	}
	else
	{
	    from_name = from_encoding;
	}
    }

    cd = towchar_cd;		// set iconv handle

    size_t in_len = in.length();		// number of bytes of input std::string
    char* in_ptr = const_cast <char*>( in.c_str() );

    size_t tmp_size = in_len * sizeof( wchar_t );	// buffer size: at most in_len wide characters
    char* tmp = ( char* ) malloc( tmp_size + sizeof( wchar_t ) );		// + L'\0'

    do
    {

	size_t tmp_len = tmp_size;
	char* tmp_ptr = tmp;

	size_t iconv_ret = iconv( cd, &in_ptr, &in_len, &tmp_ptr, &tmp_len );

	*(( wchar_t* ) tmp_ptr ) = L'\0';

	*out += std::wstring(( wchar_t* ) tmp );

	if ( iconv_ret == ( size_t )( -1 ) )
	{
	    if ( !complained )
	    {
		// EILSEQ	84	Illegal byte sequence.
		// EINVAL	22	Invalid argument
		// E2BIG	7	Argument list too long
		yuiError() << "ERROR iconv: " << errno << std::endl;
		complained = true;
	    }

	    if ( errno == EINVAL || errno == EILSEQ )
	    {
		*out += L'?';
	    }

	    in_ptr++;

	    in_len--;
	}

    }
    while ( in_len != 0 );

    free( tmp );

    return true;
}



std::string NCstring::Str() const
{
    std::string utf8str;
    RecodeFromWchar( wstr, "UTF-8", &utf8str );

    return utf8str;
}



void NCstring::getHotkey( ) const
{

    hotp = std::wstring::npos;
    const wchar_t shortcutMarker = L'&';
    const wchar_t replacementShortcutMarker = L'_';

    // I'm not really happy with using replacement markers and copying the std::string
    // but is there an other way?
    // If hotkey is looked up before un-escaping, its position won't be up-to-date anymore
    // as chars got deleted from the std::string
    // And vice versa: if un-escaping is done before looking up hotkey position, it's no
    // longer possible to tell hotkey marker and regular & (previous &&) apart (this is
    // the 'Foo&&Bar&Geeez' case) fB.

    bool have_shortcut = false;
    std::wstring::size_type len = wstr.length();
    std::wstring newstr;
    newstr.reserve( len );

    for (std::wstring::iterator it = wstr.begin(); it != wstr.end(); it++) {
        if ( *it == shortcutMarker &&
             (it + 1 != wstr.end()) ) {

    	    // double && un-escaping - bnc#559226
    	    // foo&&bar => foo&bar
	    if ( *(it+1) == shortcutMarker) {
                newstr += shortcutMarker;  // add only one &
                it++; // .. and jump forth to skip the 2nd one
	    }
	    // regular hotkey &X
	    else {
		// take the first one only (we can't do multiple hotkeys per 1 line
	        // so we just discard the rest, argh)
		if ( !have_shortcut) {
		    newstr += replacementShortcutMarker;
		    have_shortcut = true;
		}
	    }
        }
        else
           newstr += *it;
    }

    wstr = newstr;

    std::wstring::size_type tpos = wstr.find_first_of( replacementShortcutMarker );

    if ( tpos != std::wstring::npos && tpos != wstr.size() - 1 )
    {
	size_t realpos = 0, t;

	for ( t = 0; t < tpos; t++ )
	    realpos += wcwidth( wstr[t] );

	wstr.erase( tpos, 1 );

	hotk = wstr[tpos];

	hotp = realpos;
    }

}



bool NCstring::setTerminalEncoding( const std::string & encoding )
{
    if ( termEncoding != encoding )
    {
	yuiMilestone() << "Terminal encoding SET to: " << encoding << std::endl;
	termEncoding = encoding;
	return true;
    }
    else
    {
	return false;
    }
}
