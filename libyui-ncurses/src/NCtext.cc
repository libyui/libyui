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
    : sawnl( false )
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
  mtext.clear();
  mtext.push_back( "" );
  sawnl = false;

  if ( ntext.str().empty() )
    return;

  const string & text( ntext.utf8str() );
  string::size_type spos = 0;
  string::size_type cpos = string::npos;

  while ( (cpos = text.find( '\n', spos )) != string::npos ) {
    if ( sawnl )
      mtext.push_back( "" );
    mtext.back().assignUtf8( mtext.back().utf8str() + text.substr( spos, cpos-spos ) );
    sawnl = true;
    spos = cpos + 1;
  }

  if ( spos < text.size() ) {
    if ( sawnl )
      mtext.push_back( "" );
    mtext.back().assignUtf8( mtext.back().utf8str() + text.substr( spos ) );
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
//	METHOD NAME : NCtext::Columns
//	METHOD TYPE : unsigned
//
//	DESCRIPTION :
//
unsigned NCtext::Columns() const
{
  unsigned llen = 0;

  const_iterator line;
  for ( line = mtext.begin(); line != mtext.end(); ++line ) {
    if ( (*line).str().length() > llen )
      llen = (*line).str().length();
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
const NCstring & NCtext::operator[]( unsigned idx ) const
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
  hotline = string::npos;
  unsigned lineno = 0;
  for ( iterator line = mtext.begin(); line != mtext.end(); ++line, ++lineno ) {
    line->stripHotkey();
    if ( line->hotpos() != string::npos ) {
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
	  ++line, ++l, ++lineno ) {

      if ( pre && fillup ) {
	w.move( l, area.Pos.C );
	w.addstr( string( pre, ' ' ).c_str() );
      } else {
	w.move( l, area.Pos.C + pre );
      }

      if ( len ) {
	w.printw( "%-*.*s", len, (int)len, (*line).str().c_str() );
      }

      if ( post && fillup ) {
	w.addstr( string( post, ' ' ).c_str() );
      }

      if ( lineno == hotline && hotstyle && pre + hotpos() < maxlen ) {
	w.bkgdset( hotstyle );
	w.addch( w.inchar( l, area.Pos.C + pre + hotpos() ) );
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

