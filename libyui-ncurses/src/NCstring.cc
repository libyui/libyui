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

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include <ycp/YCPString.h>

#include "Y2Log.h"
#include "NCstring.h"

///////////////////////////////////////////////////////////////////

const string NCstring::utf8Encoding   ( "UTF-8" );
string       NCstring::defaultEncoding( "ISO-8859-1" );

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
    : flags    ( CLEAN )
    , hotk     ( 0 )
    , hotp     ( string::npos )
    , mutf8    ( ystr->value() )
    , mstr     ( ystr->value() )
    , mencoding( utf8Encoding )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::operator=
//	METHOD TYPE : NCstring &
//
//	DESCRIPTION :
//
NCstring & NCstring::operator=( const YCPString & ystr ) {
  return assignUtf8( ystr->value() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::assignutf8
//	METHOD TYPE : NCstring &
//
//	DESCRIPTION :
//
NCstring & NCstring::assignUtf8( const string & nstr )
{
  // No need to set DIRTY if HOTKEY is set. Next checkEncoding()
  // will recode and strip because utf8Encoding is never defaultEncoding.
  flags     = CLEAN | f_local();
  hotk      = 0;
  hotp      = string::npos;
  mutf8     = nstr;
  mstr      = nstr;
  mencoding = utf8Encoding;
  return *this;
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
  genUtf8();
  return mutf8;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::genUtf8
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCstring::genUtf8() const
{
  if ( flags & NOUTF8 ) {
    Recode( mstr, mencoding, utf8Encoding, mutf8 );
    flags &= ~NOUTF8;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::checkEncoding
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCstring::checkEncoding( bool needutf8 ) const
{
  if ( (flags & NOUTF8) && ( needutf8 || (flags & HOTKEY) ) ) {
    flags |= DIRTY; // we need utf8 for hotkey
  }

  if ( flags & DIRTY || mencoding != defaultEncoding ) {
    genUtf8();
    Recode( mutf8, utf8Encoding, defaultEncoding, mstr );
    mencoding = defaultEncoding;
    flags &= ~DIRTY;
    hotk = 0;
    hotp = string::npos;
    if ( flags & HOTKEY ) {
      // strip hotkey from mstr
      string::size_type tpos = mstr.find_first_of( '&' );
      if ( tpos != string::npos && tpos != mstr.size()-1 ) {
	mstr.erase( tpos, 1 );
	hotk = mstr[tpos];
	hotp = tpos;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstring::setDefaultEncoding
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCstring::setDefaultEncoding( const string & encoding )
{
  string newencoding( encoding.length() ? encoding : "ISO-8859-1" );

  if ( defaultEncoding == newencoding )
    return false;

  NCMIL << form( "DefaultEncoding canged from %s to %s\n",
		defaultEncoding.c_str(),
		newencoding.c_str() );
  defaultEncoding = newencoding;
  return true;
}

