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

   File:       NCtext.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCtext.h"

#include <wchar.h>		// wcwidth

///////////////////////////////////////////////////////////////////

const NCstring NCtext::emptyStr;

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCtext::NCtext
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCtext::NCtext( const NCstring & nstr )
{
  lset( nstr );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCtext::~NCtext
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCtext::~NCtext()
{}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCtext::lset
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCtext::lset( const NCstring & ntext )
{
    // FIXME: rewrite this function so one understands it

    mtext.clear();
    mtext.push_back( "" );

    if ( ntext.str().empty() )
	return;

    const wstring & text( ntext.str() );
    wstring::size_type spos = 0;
    wstring::size_type cpos = wstring::npos;

    bool sawnl = false;		// saw new line

    while ( (cpos = text.find( L'\n', spos )) != wstring::npos )
    {
	if ( sawnl )
	    mtext.push_back( "" );

	mtext.back() = NCstring( mtext.back().str() + text.substr(spos, cpos-spos) );
	sawnl = true;
	spos = cpos + 1;
    }

    if ( spos < text.size() )
    {
	if ( sawnl )
	    mtext.push_back( "" );

	mtext.back() = NCstring( mtext.back().str() + text.substr( spos ) );
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCtext::Lines
//	METHOD TYPE : unsigned
//
//	DESCRIPTION :
//
unsigned NCtext::Lines() const
{
  return mtext.size();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCtext::append
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCtext::append( const NCstring &line )
{
    mtext.push_back( line );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCtext::Columns
//	METHOD TYPE : unsigned
//
//	DESCRIPTION :
//
size_t NCtext::Columns() const
{
  size_t llen = 0;		// longest line
  size_t tmp_len = 0;		// width of current line

  const_iterator line;		// iterator for list <NCstring> mtext
  std::wstring::const_iterator wstr_it;	// iterator for wstring

  for ( line = mtext.begin(); line != mtext.end(); ++line )
  {
      tmp_len = 0;
      for ( wstr_it = (*line).str().begin(); wstr_it != (*line).str().end() ; ++wstr_it )
      {
	  tmp_len += wcwidth( *wstr_it );
      }
      if ( tmp_len > llen )
	      llen = tmp_len;
  }

  return llen;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCtext::operator[]
//	METHOD TYPE : const NCstring &
//
//	DESCRIPTION :
//
const NCstring & NCtext::operator[]( wstring::size_type idx ) const
{
  if ( idx >= Lines() )
    return emptyStr;
  const_iterator line = mtext.begin();
  for ( ; idx; --idx, ++line )
    ;
  return *line;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const NCtext & OBJ )
{
  return STREAM << "[Text:" << OBJ.Lines() << ',' << OBJ.Columns() << ']';
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NClabel
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NClabel::stripHotkey
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NClabel::stripHotkey()
{
  hotline = wstring::npos;
  unsigned lineno = 0;
  for ( iterator line = mtext.begin(); line != mtext.end(); ++line, ++lineno ) {
    line->getHotkey();
    if ( line->hotpos() != wstring::npos ) {
      hotline = lineno;
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NClabel::drawAt
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NClabel::drawAt( NCursesWindow & w, chtype style, chtype hotstyle,
		      const wrect & dim,
		      const NC::ADJUST adjust,
		      bool fillup ) const
{
  wrect area( dim.intersectRelTo( w.area() ) );

  if ( area.Sze > 0 ) {
    unsigned maxlen = area.Sze.W;
    unsigned len    = ( width() < maxlen ) ? width() : maxlen;
    unsigned pre    = 0;
    unsigned post   = 0;

    if ( len < maxlen ) {
      unsigned dist = maxlen - len;
      if ( adjust & NC::LEFT )
	pre = 0;
      else if ( adjust & NC::RIGHT )
	pre = dist;
      else
	pre = dist/2;
      post = dist - pre;
    }

    int l           = area.Pos.L;
    int maxl        = area.Pos.L + area.Sze.H;
    unsigned lineno = 0;

    chtype obg = w.getbkgd();
    w.bkgdset( style );

    for ( NCtext::const_iterator line = begin();
	  line != end() && l < maxl;
	  ++line, ++l, ++lineno )
    {
      if ( pre && fillup ) {
	w.move( l, area.Pos.C );
	w.addwstr( wstring( pre, L' ' ).c_str() );
      } else {
	w.move( l, area.Pos.C + pre );
      }

      NCDBG << "TERMINAL: " << NCstring::terminalEncoding() << " CODESET: " << nl_langinfo( CODESET) << endl;
      if ( len )
      {
	  if ( NCstring::terminalEncoding() != "UTF-8" )
	  {
	      string out;
	      bool ok = NCstring::RecodeFromWchar( (*line).str(), NCstring::terminalEncoding(), &out );
	      if ( ok )
	      {
		 w.printw( "%-*.*s", len, (int)len, out.c_str() );
	      }
	  }
	  else
	  {
	      // TODO formatting (like above) needed ?
	      w.printw( "%ls", (*line).str().c_str() );
	  }
      }

      if ( post && fillup ) {
	w.addwstr( wstring( post, L' ' ).c_str() );
      }

      if ( lineno == hotline && hotstyle && pre + hotpos() < maxlen )
      {
	  w.bkgdset( hotstyle );

	  w.add_attr_char( l, area.Pos.C + pre + hotpos() );

	  w.bkgdset( style );
      }

    }
    if ( fillup ) {
      for ( ; l < maxl; ++l ) {
	w.printw( l, area.Pos.C, "%-*.*s", area.Sze.W, area.Sze.W, "" );
      }
    }
    w.bkgdset( obg );
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const NClabel & OBJ )
{
  STREAM << "[label" << OBJ.size() << ':' << OBJ[0].str();
  if ( OBJ.hasHotkey() )
    STREAM << ':' << OBJ.hotkey() << " at " << OBJ.hotpos();
  return STREAM  << ']';
}

