/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCstring.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include <ycp/YCPString.h>

#include "Y2Log.h"
#include "NCstring.h"

//////////////////////////////////////////////////////////////////

string       NCstring::termEncoding( "" );

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::NCstring
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCstring::NCstring( const YCPString & ystr )
    : hotk     ( 0 )
    , hotp     ( wstring::npos )
{
    bool ok = RecodeToWchar( ystr->value(), "UTF-8", &mstr );

    if ( !ok )
    {
	NCERR << "ERROR: RecodeToWchar() failed" << endl;
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::NCstring
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCstring:: NCstring()
    : hotk     ( 0 )
    , hotp     ( wstring::npos )
    , mstr     ( L"" )

{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::NCstring
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCstring::NCstring( const NCstring & nstr )
    : hotk     ( nstr.hotk )
    , hotp     ( nstr.hotp )
    , mstr     ( nstr.mstr )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::NCstring
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCstring::NCstring( const wstring & wstr )
    : hotk     ( 0 )
    , hotp     ( wstring::npos )
    , mstr     ( wstr )
{
}
    
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::NCstring
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCstring::NCstring( const string & str )
    : hotk     ( 0 )
    , hotp     ( wstring::npos )
{
    bool ok = RecodeToWchar( str, "UTF-8", &mstr );

    if ( !ok )
    {
	NCERR << "ERROR: RecodeToWchar() failed" << endl;
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::NCstring
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCstring::NCstring( const char * cstr )
    : hotk     ( 0 )
    , hotp     ( wstring::npos )
{
    bool ok = RecodeToWchar( cstr, "UTF-8", &mstr );

    if ( !ok )
    {
	NCERR << "ERROR: RecodeToWchar() failed" << endl;
    }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::operator=
//	METHOD TYPE : NCstring &
//
//	DESCRIPTION :
//
NCstring & NCstring::operator=( const YCPString & ystr )
{
  hotk      = 0;
  hotp      = wstring::npos;

  bool ok =  RecodeToWchar( ystr->value(), "UTF-8", &mstr );

  if ( !ok )
  {
      NCERR << "ERROR: RecodeToWchar() failed" << endl;
  }
  return *this;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::operator<<
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & operator<<( ostream & STREAM, const NCstring & OBJ )
{
   return STREAM <<  OBJ.Str() ;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::operator=
//	METHOD TYPE : NCstring &
//
//	DESCRIPTION :
//
NCstring & NCstring::operator=( const NCstring & nstr )
{
    if ( &nstr != this ) {
	hotk      = nstr.hotk;
	hotp      = nstr.hotp;
	mstr      = nstr.mstr;
    }
    return *this;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::operator+=
//	METHOD TYPE : NCstring &
//
//	DESCRIPTION :
//
NCstring & NCstring::operator+=( const NCstring & nstr )
{
    mstr = this->mstr + nstr.mstr;

    return *this;
}

static iconv_t fromwchar_cd	= (iconv_t)(-1);
static string  to_name		= "";

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::RecodeWchar
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCstring::RecodeFromWchar( const wstring & in, const string & to_encoding, string* out)
{
    iconv_t cd = (iconv_t)(-1);
    static bool complained = false;

    // iconv_open not yet called
    if ( fromwchar_cd == (iconv_t)(-1)
	 || to_name != to_encoding ) 
    {
	if ( fromwchar_cd != (iconv_t)(-1) )
	{
	    iconv_close ( fromwchar_cd );
	}
	
	fromwchar_cd = iconv_open ( to_encoding.c_str(), "WCHAR_T" );
	NCMIL << "iconv_open( " << to_encoding.c_str() << ", \"WCHAR_T\" )" << endl;
	
	if ( fromwchar_cd == (iconv_t)(-1) )
	{
	    if (!complained)
	    {
		NCERR << "ERROR: iconv_open failed" << endl;
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
    
    size_t in_len = in.length () * sizeof( wstring::value_type );	// number of in bytes
    char* in_ptr = (char *)in.data();
    
    size_t tmp_size = (in_len * sizeof (char)) * 2;
    // tmp buffer size: in_len bytes * 2, that means 1 wide charatcer (4 Byte) can be transformed
    // into an encoding which needs at most 8 Byte for one character (should be enough)
    
    char tmp[tmp_size + sizeof (char)];

    *out = "";

    do {
	char *tmp_ptr = tmp;
	size_t tmp_len = tmp_size;
	*((char*) tmp_ptr) = '\0';
	
	size_t iconv_ret = iconv (cd, &in_ptr, &in_len, &tmp_ptr, &tmp_len);

	*((char*) tmp_ptr) = '\0';
	*out += string ((char *)&tmp);

	if (iconv_ret == (size_t)(-1))
        {
	    if (!complained)
	    {
		NCERR << "ERROR iconv: " << errno << endl;
		complained = true;
	    }
	    if (errno == EINVAL || errno == EILSEQ)
            {
		*out += '?';
	    }
	    in_ptr += sizeof( wstring::value_type );
	    in_len -= sizeof( wstring::value_type );
	}
	
    } while (in_len != 0 );

    return true;  
}

static iconv_t towchar_cd	= (iconv_t)(-1);
static string  from_name 	= "";

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::RecodeToWchar
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCstring::RecodeToWchar (const string& in, const string &from_encoding, wstring* out)
{
    iconv_t cd = (iconv_t)(-1);
    static bool complained = false;
   
    // iconv_open not yet called 
    if ( towchar_cd == (iconv_t)(-1)
	 || from_name != from_encoding )
    {
	if ( towchar_cd != (iconv_t)(-1) )
	{
	    iconv_close( towchar_cd );
	}
	
	towchar_cd = iconv_open( "WCHAR_T", from_encoding.c_str() );
	NCMIL << "iconv_open( \"WCHAR_T\", " << from_encoding.c_str() << " )" << endl;
	
	if ( towchar_cd == (iconv_t)(-1))
	{
	    if (!complained)
	    {
		NCERR << "Error: RecodeToWchar iconv_open() failed" << endl;
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
    
    size_t in_len = in.length ();		// number of bytes of input string
    char* in_ptr = const_cast <char*> (in.c_str ());
    
    size_t tmp_size = in_len * sizeof (wchar_t);	// buffer size: at most in_len wide characters
    char tmp[tmp_size + sizeof (wchar_t)];		// + L'\0'

    *out = L"";

    do {

	size_t tmp_len = tmp_size;
	char* tmp_ptr = tmp;

	size_t iconv_ret = iconv (cd, &in_ptr, &in_len, &tmp_ptr, &tmp_len);

	*((wchar_t*) tmp_ptr) = L'\0';

	*out += wstring ((wchar_t*) &tmp);

	if (iconv_ret == (size_t)(-1))
        {
	    if ( !complained )
	    {
		// EILSEQ 	84	Illegal byte sequence.
		// EINVAL   	22      Invalid argument
		// E2BIG     	7	Argument list too long
		NCERR << "ERROR iconv: " << errno << endl;
		complained = true;
	    }
	    if (errno == EINVAL || errno == EILSEQ)
            {
		*out += L'?';
	    }
	    in_ptr++;
	    in_len--;
	}

    } while (in_len != 0);

    return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::YCPstr
//	METHOD TYPE : YCPString
//
//	DESCRIPTION :
//
YCPString NCstring::YCPstr() const
{
    string utf8str;
    RecodeFromWchar ( mstr, "UTF-8", &utf8str );
    
    return utf8str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::YCPstr
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string NCstring::Str() const
{
    string utf8str;
    RecodeFromWchar ( mstr, "UTF-8", &utf8str );
    
    return utf8str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::getHotkey
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCstring::getHotkey( ) const
{

    hotp = wstring::npos;
    // strip hotkey from mstr
    wstring::size_type tpos = mstr.find_first_of( L'&' );
    if ( tpos != wstring::npos && tpos != mstr.size()-1 )
    {
	mstr.erase( tpos, 1 );
	hotk = mstr[tpos];
	hotp = tpos;
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::setTerminalEncoding
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCstring::setTerminalEncoding( const string & encoding )
{
    // set terminal encoding only once - cannot change
    if ( termEncoding == "" )
    {
	NCMIL << "TERMINAL encoding: " << encoding << endl;
	termEncoding = encoding;
	return true;
    }
    else
    {
	return false;
    }
}
