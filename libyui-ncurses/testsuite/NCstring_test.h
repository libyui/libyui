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

   File:       NCstring_test.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCstring_test_h
#define NCstring_test_h

#include <iosfwd>

#include <string>

using std::string;
using std::wstring;


using namespace std;

class YCPString;


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCstring_test
//
//	DESCRIPTION :
//
class NCstring_test {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCstring_test & OBJ );

  private:

    mutable wchar_t hotk;
    mutable wstring::size_type hotp;
    mutable wstring   mstr;

public:

    NCstring_test()
      : hotk     ( 0 )
      , hotp     ( wstring::npos )
      , mstr     ( L"" )
      {
      }

    NCstring_test( const NCstring_test & nstr )
      : hotk     ( nstr.hotk )
      , hotp     ( nstr.hotp )
      , mstr     ( nstr.mstr )
      {}

    ~NCstring_test() {}

  public: // filter from/to YCPString(utf8)

    NCstring_test( const YCPString & ystr );

    NCstring_test & operator=( const YCPString & ystr );
    
    YCPString YCPstr() const;

  public:

    NCstring_test( const string & str );

    NCstring_test( const char * cstr );
    
    NCstring_test & operator=( const NCstring_test & nstr ) {
      if ( &nstr != this ) {
	hotk      = nstr.hotk;
	hotp      = nstr.hotp;
	mstr      = nstr.mstr;
      }
      return *this;
    }

    NCstring_test & operator+=( const NCstring_test & nstr ) {
	mstr = this->mstr + nstr.mstr;

	return *this;
    }	

    const wstring & str()      const { return mstr; }

  private:

    wchar_t 	      hotkey() const { return hotk; }
    string::size_type hotpos() const { return hotp; }

  public:

    static bool RecodeToWchar ( const string& in, const string & from_encoding, wstring* out);
    static bool RecodeFromWchar( const wstring & in, const string & to_encoding, string* out);
    
    void getHotkey() const;
};

///////////////////////////////////////////////////////////////////

#endif // NCstring_test_h
