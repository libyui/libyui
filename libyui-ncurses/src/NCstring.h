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

   File:       NCstring.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCstring_h
#define NCstring_h

#include <iosfwd>

#include <string>
using namespace std;

class YCPString;

///////////////////////////////////////////////////////////////////

extern int Recode( const string & str, const string & from,
		   const string & to, string & outstr );

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCstring
//
//	DESCRIPTION :
//
class NCstring {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCstring & OBJ );

  private:

    static const string utf8Encoding;
    static string       defaultEncoding;

  private:

    enum Flags {
      CLEAN  = 0x00,
      HOTKEY = 0x01,
      NOUTF8 = 0x10,
      DIRTY  = 0x20,
      //////////////
      M_LOCAL  = HOTKEY  // remains set in opeartor=()
    };

    mutable unsigned flags;
    mutable unsigned char hotk;
    mutable string::size_type hotp;
    mutable string   mutf8;
    mutable string   mstr;
    mutable string   mencoding;

    unsigned f_local() const { return flags & M_LOCAL; }

    void genUtf8() const;
    void checkEncoding( bool needutf8 = false ) const;

  public:

    NCstring()
      : flags    ( CLEAN )
      , hotk     ( 0 )
      , hotp     ( string::npos )
      , mutf8    ( "" )
      , mstr     ( "" )
      , mencoding( defaultEncoding )
      {}
    NCstring( const char * cstr )
      : flags    ( NOUTF8 )
      , hotk     ( 0 )
      , hotp     ( string::npos )
      , mutf8    ( "" )
      , mstr     ( cstr ? cstr : "" )
      , mencoding( defaultEncoding )
      {}
    NCstring( const string & str )
      : flags    ( NOUTF8 )
      , hotk     ( 0 )
      , hotp     ( string::npos )
      , mutf8    ( "" )
      , mstr     ( str )
      , mencoding( defaultEncoding )
      {}
    NCstring( const NCstring & nstr )
      : flags    ( nstr.flags )
      , hotk     ( nstr.hotk )
      , hotp     ( nstr.hotp )
      , mutf8    ( nstr.mutf8 )
      , mstr     ( nstr.mstr )
      , mencoding( nstr.mencoding )
      {}

    ~NCstring() {}

  public: // filter from/to YCPString(utf8)

    NCstring( const YCPString & ystr );

    NCstring & operator=( const YCPString & ystr );
    NCstring & assignUtf8( const string & nstr );

    YCPString YCPstr() const;

  public:

    NCstring & operator=( const NCstring & nstr ) {
      if ( &nstr != this ) {
	flags     = nstr.flags | f_local();
	hotk      = nstr.hotk;
	hotp      = nstr.hotp;
	if ( (flags & HOTKEY) && !(nstr.flags & HOTKEY) ) {
	  // nstr does not pass a hotkey, so force
	  // next checkEncoding() to look for it.
	  flags |= DIRTY;
	}
	mutf8     = nstr.mutf8;
	mstr      = nstr.mstr;
	mencoding = nstr.mencoding;
      }
      return *this;
    }

    NCstring & operator+=( const NCstring & nstr ) {
	mstr = this->mstr + nstr.mstr;
	mutf8 = this->mutf8 + nstr.mstr;

	return *this;
    }	

    const string & str()      const { checkEncoding(); return mstr; }
    const string & utf8str()  const { checkEncoding( true ); return mutf8; }

  private:

    friend class NClabel;
    unsigned char     hotkey() const { checkEncoding(); return hotk; }
    string::size_type hotpos() const { checkEncoding(); return hotp; }

    void stripHotkey() {
      if ( !(flags & HOTKEY) ) {
	flags |= DIRTY | HOTKEY;
      }
    }

  public:

    static const string & DefaultEncoding();
    static bool setDefaultEncoding( const string & encoding = "" );
};

///////////////////////////////////////////////////////////////////

#endif // NCstring_h
