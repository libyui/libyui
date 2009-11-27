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

   File:       NCPad.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPad.h"

#if 0
#undef  DBG_CLASS
#define DBG_CLASS "_NCPad_"
#endif

///////////////////////////////////////////////////////////////////

// PAD_PAGESIZE needs to be large enough to feed any destwin. We
// get in throuble here if the terminal has more than 1024 lines.
#define PAD_PAGESIZE 1024

// Maximum height of the NCursesPad (e.g. in case it can't hold more
// than 32768 lines). Larger pads need to page.
//#define MAX_PAD_HEIGHT 100
#define MAX_PAD_HEIGHT NCursesWindow::maxcoord()

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPad::NCPad
//	METHOD TYPE : Constructor
//
NCPad::NCPad( int lines, int cols, const NCWidget & p )
  : NCursesPad( lines > MAX_PAD_HEIGHT ? PAD_PAGESIZE : lines, cols )
  , _vheight( lines > MAX_PAD_HEIGHT ? lines : 0 )
  , parw( p )
  , destwin ( 0 )
  , maxdpos ( 0 )
  , maxspos ( 0 )
  , dclear  ( false )
  , dirty   ( false )
{
  IDBG << maxcoord() << endl;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPad::Destwin
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPad::Destwin( NCursesWindow * dwin )
{
  if ( dwin != destwin ) {
    destwin = dwin;

    if ( destwin ) {
      wsze mysze( vheight(), width() );

      drect = wrect( 0, wsze(destwin->height(), destwin->width()) );
      srect = wrect( 0, wsze::min( mysze, drect.Sze ) );
      maxdpos = drect.Pos + srect.Sze - 1;
      maxspos = mysze - srect.Sze;

      dclear =( drect.Sze != srect.Sze );
      setpos( CurPos() );
    }
    else {
      drect = srect = wrect();
      maxdpos = maxspos = 0;
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPad::resize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPad::resize( wsze nsze )
{
  SetPadSize( nsze ); // might be enlarged by NCPadWidget if redirected

  if (    nsze.H != vheight()
       || nsze.W != width() ) {
    NCursesWindow * odest = Destwin();
    if ( odest )
      Destwin( 0 );

    if ( nsze.H > MAX_PAD_HEIGHT )
    {
      IDBG << "TRUCNATE PAD: " << nsze.H << " > " << MAX_PAD_HEIGHT << endl;
      NCursesPad::resize( PAD_PAGESIZE, nsze.W );
      _vheight = nsze.H;
    }
    else
    {
    NCursesPad::resize( nsze.H, nsze.W );
      _vheight = 0;
    }

    IDBG << "Pageing ?: " << pageing() << endl;

    if ( odest )
      Destwin( odest );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPad::updateScrollHint
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPad::updateScrollHint()
{
  NCScrollHint::VSet( srect.Sze.H + maxspos.L, srect.Sze.H, srect.Pos.L );
  NCScrollHint::HSet( srect.Sze.W + maxspos.C, srect.Sze.W, srect.Pos.C );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPad::update
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCPad::update()
{
  if ( destwin ) {
    if ( dirty ) {
      return dirtyPad();
    }

    if ( dclear )
      destwin->clear();

    updateScrollHint();

    if ( ! pageing() )
    {
    return copywin( *destwin,
		    srect.Pos.L, srect.Pos.C,
		    drect.Pos.L, drect.Pos.C,
		    maxdpos.L,   maxdpos.C,
		    false );
  }

    // Here: Table is pageing, so we must prepare the visible lines
    // on the Pad before we're copying them to the destwin:
    wsze lSze( 1, width() );
    for ( unsigned i = 0; i <= maxdpos.L; ++i )
    {
      directDraw( *this, wrect( wpos( i, 0 ), lSze ), srect.Pos.L+i );
    }
    return copywin( *destwin,
                    0, srect.Pos.C,
                    drect.Pos.L, drect.Pos.C,
                    maxdpos.L,   maxdpos.C,
                    false );
  }
  return OK;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPad::setpos
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCPad::setpos( const wpos & newpos )
{
  srect.Pos = newpos.between( 0, maxspos );
  return update();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPad::wRecoded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPad::wRecoded()
{
  SDBG << "NCPad::wRecoded" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPad::handleInput
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPad::handleInput( wint_t key )
{
  bool handled = true;
  switch ( key ) {

  case KEY_UP:
    ScrlUp();
    break;
  case KEY_PPAGE:
    ScrlUp( destwin->maxy() );
    break;
  case KEY_HOME:
    ScrlUp( vheight() );
    break;

  case KEY_DOWN:
    ScrlDown();
    break;
  case KEY_NPAGE:
    ScrlDown( destwin->maxy() );
    break;
  case KEY_END:
    ScrlDown( vheight() );
    break;

  case KEY_LEFT:
  case KEY_SLEFT:
      ScrlLeft();
    break;
  case KEY_RIGHT:
  case KEY_SRIGHT:
    ScrlRight();
    break;

  default:
    handled = false;
    break;
  }
  return handled;
}
