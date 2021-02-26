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

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCPad.h"



void NCPad::Destwin( NCursesWindow * dwin )
{
  if ( dwin != destwin ) {
    destwin = dwin;

    if ( destwin ) {
      wsze mysze( maxy()+1, maxx()+1 );

      drect = wrect( 0, wsze(destwin->maxy()+1, destwin->maxx()+1) );
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



void NCPad::resize( wsze nsze )
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



void NCPad::updateScrollHint()
{
  NCScrollHint::VSet( srect.Sze.H + maxspos.L, srect.Sze.H, srect.Pos.L );
  NCScrollHint::HSet( srect.Sze.W + maxspos.C, srect.Sze.W, srect.Pos.C );
}



int NCPad::update()
{
  if ( destwin ) {
    if ( dirty ) {
      return dirtyPad();
    }

    if ( dclear )
      destwin->clear();

    updateScrollHint();

    return copywin( *destwin,
		    srect.Pos.L, srect.Pos.C,
		    drect.Pos.L, drect.Pos.C,
		    maxdpos.L,   maxdpos.C,
		    false );
  }
  return OK;
}



int NCPad::setpos( const wpos & newpos )
{
  srect.Pos = newpos.between( 0, maxspos );
  return update();
}



void NCPad::wRecoded()
{
  yuiDebug() << "NCPad::wRecoded" << endl;
}



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
    ScrlUp( maxy() );
    break;

  case KEY_DOWN:
    ScrlDown();
    break;
  case KEY_NPAGE:
    ScrlDown( destwin->maxy() );
    break;
  case KEY_END:
    ScrlDown( maxy() );
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
