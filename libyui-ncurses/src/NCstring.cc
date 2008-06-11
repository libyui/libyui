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

   File:       NCstring.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#include <errno.h>
#include <iconv.h>
#include <malloc.h>

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCstring.h"


// The default encoding is UTF-8. For real terminals this may be
// changed with setConsoleFont().

string	NCstring::termEncoding( "UTF-8" );






NCstring:: NCstring()
	: hotk( 0 )
	, hotp( wstring::npos )
	, wstr( L"" )

{
}



NCstring::NCstring( const NCstring & nstr )
	: hotk( nstr.hotk )
	, hotp( nstr.hotp )
	, wstr( nstr.wstr )
{
}



NCstring::NCstring( const wstring & widestr )
	: hotk( 0 )
	, hotp( wstring::npos )
	, wstr( widestr )
{
}



NCstring::NCstring( const string & str )
	: hotk( 0 )
	, hotp( wstring::npos )
{
    bool ok = RecodeToWchar( str, "UTF-8", &wstr );

    if ( !ok )
    {
	yuiError() << "ERROR: RecodeToWchar() failed" << endl;
    }
}



NCstring::NCstring( const char * cstr )
	: hotk( 0 )
	, hotp( wstring::npos )
{
    bool ok = RecodeToWchar( cstr, "UTF-8", &wstr );

    if ( !ok )
    {
	yuiError() << "ERROR: RecodeToWchar() failed" << endl;
    }
}



std::ostream & operator<<( std::ostream & STREAM, const NCstring & OBJ )
{
    return STREAM <<  OBJ.Str() ;
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
static string  to_name		= "";



bool NCstring::RecodeFromWchar( const wstring & in, const string & to_encoding, string* out )
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

	yuiDebug() << "iconv_open( " << to_encoding.c_str() << ", \"WCHAR_T\" )" << endl;

	if ( fromwchar_cd == ( iconv_t )( -1 ) )
	{
	    if ( !complained )
	    {
		yuiError() << "ERROR: iconv_open failed" << endl;
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

    size_t in_len = in.length() * sizeof( wstring::value_type );	// number of in bytes
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
	*out += string( tmp );

	if ( iconv_ret == ( size_t )( -1 ) )
	{
	    if ( !complained )
	    {
		yuiError() << "ERROR iconv: " << errno << endl;
		complained = true;
	    }

	    if ( errno == EINVAL || errno == EILSEQ )
	    {
		*out += '?';
	    }

	    in_ptr += sizeof( wstring::value_type );

	    in_len -= sizeof( wstring::value_type );
	}

    }
    while ( in_len != 0 );

    free( tmp );

    return true;
}

static iconv_t towchar_cd	= ( iconv_t )( -1 );
static string  from_name	= "";



bool NCstring::RecodeToWchar( const string& in, const string &from_encoding, wstring* out )
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

	yuiDebug() << "iconv_open( \"WCHAR_T\", " << from_encoding.c_str() << " )" << endl;

	if ( towchar_cd == ( iconv_t )( -1 ) )
	{
	    if ( !complained )
	    {
		yuiError() << "Error: RecodeToWchar iconv_open() failed" << endl;
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

    size_t in_len = in.length();		// number of bytes of input string
    char* in_ptr = const_cast <char*>( in.c_str() );

    size_t tmp_size = in_len * sizeof( wchar_t );	// buffer size: at most in_len wide characters
    char* tmp = ( char* ) malloc( tmp_size + sizeof( wchar_t ) );		// + L'\0'

    do
    {

	size_t tmp_len = tmp_size;
	char* tmp_ptr = tmp;

	size_t iconv_ret = iconv( cd, &in_ptr, &in_len, &tmp_ptr, &tmp_len );

	*(( wchar_t* ) tmp_ptr ) = L'\0';

	*out += wstring(( wchar_t* ) tmp );

	if ( iconv_ret == ( size_t )( -1 ) )
	{
	    if ( !complained )
	    {
		// EILSEQ	84	Illegal byte sequence.
		// EINVAL	22	Invalid argument
		// E2BIG	7	Argument list too long
		yuiError() << "ERROR iconv: " << errno << endl;
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



string NCstring::Str() const
{
    string utf8str;
    RecodeFromWchar( wstr, "UTF-8", &utf8str );

    return utf8str;
}



void NCstring::getHotkey( ) const
{

    hotp = wstring::npos;
    // strip hotkey from wstr
    wstring::size_type tpos = wstr.find_first_of( L'&' );

    if ( tpos != wstring::npos && tpos != wstr.size() - 1 )
    {
	size_t realpos = 0, t;

	for ( t = 0; t < tpos; t++ )
	    realpos += wcwidth( wstr[t] );

	wstr.erase( tpos, 1 );

	hotk = wstr[tpos];

	hotp = realpos;
    }
}



bool NCstring::setTerminalEncoding( const string & encoding )
{
    if ( termEncoding != encoding )
    {
	yuiMilestone() << "Terminal encoding SET to: " << encoding << endl;
	termEncoding = encoding;
	return true;
    }
    else
    {
	return false;
    }
}
