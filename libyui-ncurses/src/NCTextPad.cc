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

   File:       NCTextPad.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCTextPad.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::NCTextPad
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCTextPad::NCTextPad( int l, int c, const NCWidget & p )
    : NCPad( l, c, p )
    , lines( 1U, 0 )
    , cline( lines.begin() )
    , curson( false )
{
  bkgd( p.widgetStyle().data );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::~NCTextPad
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCTextPad::~NCTextPad()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::resize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextPad::resize( wsze nsze )
{
  SetPadSize( nsze ); // might be enlarged by NCPadWidget if redirected
  if (    nsze.H != height()
       || nsze.W != width() ) {
    NCursesWindow * odest = Destwin();
    if ( odest )
      Destwin( 0 );
    NCursesPad::resize( nsze.H, nsze.W );
    if ( odest )
      Destwin( odest );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::assertSze
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextPad::assertSze( wsze minsze )
{
  if (    minsze.W > width()
       || minsze.H > height() )
    resize( minsze );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::assertWidht
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextPad::assertWidht( unsigned minw )
{
  if ( minw >= (unsigned)width() ) // == for the '\n'
    resize( wsze( height(), minw + 10 ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::assertHeight
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextPad::assertHeight( unsigned minh )
{
  if ( minh > (unsigned)height() )
    resize( wsze( minh + 10, width() ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::CurPos
//	METHOD TYPE : wpos
//
//	DESCRIPTION :
//
wpos NCTextPad::CurPos() const
{
  return curs;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::cursor
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextPad::cursor( bool on )
{
  if ( on != curson ) {
    if ( (curson = on) ) {
      bkgdset( parw.wStyle().cursor );
      addch( inchar( curs.L, curs.C ) );
      bkgdset( parw.widgetStyle().data );
    }
    else
      addch( inchar( curs.L, curs.C ) );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::setpos
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCTextPad::setpos()
{
  bkgd( parw.widgetStyle().data );
  cursor( parw.GetState() == NC::WSactive );
  return setpos( CurPos() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::setpos
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCTextPad::setpos( const wpos & newpos )
{
  wpos npos( newpos.between( 0, wpos( maxy(), maxx() ) ) );

  if ( (unsigned)npos.L >= lines.size() ) {
    npos.L = lines.size() - 1;
    cline = lines.end();
    --cline;
  } else if ( npos.L != curs.L ) {
    advance( cline, npos.L - curs.L );
  }

  if ( (unsigned)npos.C > *cline ) {
    npos.C = *cline;
  }

  bool ocurs = curson;
  if ( ocurs ) cursorOff();
  curs = npos;
  if ( ocurs ) cursorOn();

  wpos padpos( curs );
  if ( drect.Sze > wsze( 0 ) ) {
    padpos = (padpos/drect.Sze) * drect.Sze;
  }

  return NCPad::setpos( padpos );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::handleInput
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCTextPad::handleInput( int key )
{
  bool handled = true;
  bool beep    = false;
  bool update  = true;

  cursorOff();
  switch ( key ) {

  case KEY_LEFT:
    if ( curs.C ) {
      --curs.C;
    } else if ( curs.L ) {
      --cline;
      --curs.L;
      curs.C = (*cline);
    } else {
      beep = true;
      update = false;
    }
    break;

  case KEY_UP:
    if ( curs.L ) {
      --cline;
      --curs.L;
    } else {
      beep = true;
      update = false;
    }
    break;

  case KEY_RIGHT:
    if ( (unsigned)curs.C < (*cline) ) {
      ++curs.C;
    } else if ( (unsigned)curs.L + 1 < lines.size() ) {
      ++cline;
      ++curs.L;
      curs.C = 0;
    } else {
      beep = true;
      update = false;
    }
    break;

  case KEY_DOWN:
    if ( (unsigned)curs.L + 1 < lines.size() ) {
      ++cline;
      ++curs.L;
    } else {
      beep = true;
      update = false;
    }
    break;

  case KEY_PPAGE:
    if ( curs.L ) {
      setpos( wpos( curs.L-3, curs.C ) );
    } else {
      beep = true;
      update = false;
    }
    break;

  case KEY_NPAGE:
    if ( (unsigned)curs.L + 1 < lines.size() ) {
      setpos( wpos( curs.L+3, curs.C ) );
    } else {
      beep = true;
      update = false;
    }
    break;

  case KEY_HOME:
    if ( curs.C ) {
      curs.C = 0;
    }
    break;

  case KEY_END:
    if ( (unsigned)curs.C < (*cline) ) {
      curs.C = (*cline);
    }
    break;

  case KEY_BACKSPACE:
    beep = !delch( true );
    break;

  case KEY_DC:
    beep = !delch();
    break;

  default:
    beep = !insert( key );
    break;
  }
  cursorOn();

  if ( beep )
    ::beep();

  if ( update )
    setpos( curs );

  return handled;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::insert
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCTextPad::insert( int key )
{
  if ( key == 10 ) {
    return openLine();
  }
  if ( key < 32 || ( key >= 127 && key < 160 ) || UCHAR_MAX < key ) {
    return false;
  }

  assertWidht( ++(*cline) );
  insch( curs.L, curs.C++, key );
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::openLine
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCTextPad::openLine()
{
  assertHeight( lines.size() + 1 );
  list<unsigned>::iterator newl( cline );
  newl = lines.insert( ++newl, 0 );

  if ( curs.C == 0 ) {
    // eazy at line begin: new empty line above
    insertln();

    (*newl) = (*cline);
    (*cline)= 0;
  } else {
    // new empty line below
    move( curs.L+1, 0 );
    insertln();

    if ( (unsigned)curs.C < (*cline) ) {
      // copy down rest of line
      (*newl) = (*cline) - curs.C;
      (*cline)= curs.C;

      move( curs.L, curs.C );
      copywin( *this, curs.L, curs.C, curs.L+1, 0, curs.L+1, (*newl), false );
      clrtoeol();
    }
  }

  cline = newl;

  ++curs.L;
  curs.C = 0;

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::delch
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCTextPad::delch( bool previous )
{
  if ( previous ) {
    if ( curs.C )
      --curs.C;
    else if ( curs.L ) {
      --cline;
      --curs.L;
      curs.C = (*cline);
    } else
      return false;
  }

  if ( (unsigned)curs.C < *cline ) {
    // eazy not at line end
    --(*cline);

    NCPad::delch( curs.L, curs.C );
  } else if ( (unsigned)curs.L + 1 < lines.size() ) {
    // at line end: join with next line
    list<unsigned>::iterator nextl( cline );
    ++nextl;
    (*cline) += (*nextl);
    lines.erase( nextl );

    assertWidht( (*cline) );
    copywin( *this, curs.L+1, 0, curs.L, curs.C, curs.L, (*cline), false );

    move( curs.L+1, 0 );
    deleteln();
  } else
    return false;

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::setText
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextPad::setText( const NCtext & ntext )
{
  bkgd( parw.widgetStyle().data );
  bool ocurs = curson;
  if ( ocurs ) cursorOff();
  clear();
  assertSze( wsze( ntext.Lines(), ntext.Columns() + 1 ) );
  curs = 0;

  lines.clear();
  unsigned cl = 0;
  for ( NCtext::const_iterator line = ntext.begin(); line != ntext.end(); ++line ) {
    lines.push_back( (*line).str().length() );

    move( cl++, 0 );
    addstr( (*line).str().c_str() );
  }
  if ( lines.empty() )
    lines.push_back( 0U );
  cline = lines.begin();

  if ( ocurs ) cursorOn();
  setpos( curs );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextPad::getText
//	METHOD TYPE : NCtext
//
//	DESCRIPTION :
//
string NCTextPad::getText() const
{
  // just for inch(x,y) call, which isn't const due to
  // hw cursor movement, but that's of no interest here.
  NCTextPad * myself = const_cast<NCTextPad *>( this );

  string ret;
  unsigned l = 0;
  for ( list<unsigned>::const_iterator cgetl( lines.begin() ); cgetl != lines.end(); ++cgetl ) {
    for ( unsigned c = 0; c < (*cgetl); ++c ) {
      ret += (char)(myself->inch( l, c )&A_CHARTEXT);
    }
    ret += "\n";
    ++l;
  }
  return ret;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const NCTextPad & OBJ )
{
  STREAM << "at " << OBJ.CurPos() << " on " << wsze( OBJ.height(), OBJ.width() )
    << " lines " << OBJ.lines.size() << " (" << *OBJ.cline << ")";
  return STREAM;
}

