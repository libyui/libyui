/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/

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

   File:       NCPad.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCPad.h"

///////////////////////////////////////////////////////////////////

// PAD_PAGESIZE needs to be large enough to feed any destwin. We
// get in throuble here if the terminal has more than 1024 lines.
#define PAD_PAGESIZE 1024

// Maximum height of the NCursesPad (e.g. in case it can't hold more
// than 32768 lines). Larger pads need to page.
//#define MAX_PAD_HEIGHT 100
#define MAX_PAD_HEIGHT NCursesWindow::maxcoord()


NCPad::NCPad( int lines, int cols, const NCWidget & p )
  : NCursesPad( lines > MAX_PAD_HEIGHT ? PAD_PAGESIZE : lines, cols )
  , _vheight( lines > MAX_PAD_HEIGHT ? lines : 0 )
  , parw( p )
  , destwin ( 0 )
  , maxdpos ( 0 )
  , maxspos ( 0 )
  , dclear  ( false )
  , dirty   ( false )
{}


void NCPad::Destwin( NCursesWindow * dwin )
{
    if ( dwin != destwin )
    {
	destwin = dwin;

	if ( destwin )
	{
	    wsze mysze( vheight(), width() );

	    drect = wrect( 0, wsze( destwin->height(), destwin->width() ) );
	    srect = wrect( 0, wsze::min( mysze, drect.Sze ) );
	    maxdpos = drect.Pos + srect.Sze - 1;
	    maxspos = mysze - srect.Sze;

	    dclear = ( drect.Sze != srect.Sze );
	    setpos( CurPos() );
	}
	else
	{
	    drect = srect = wrect();
	    maxdpos = maxspos = 0;
	}
    }
}


void NCPad::resize( wsze nsze )
{
    SetPadSize( nsze ); // might be enlarged by NCPadWidget if redirected

    if ( nsze.H != vheight()
	 || nsze.W != width() )
    {
	NCursesWindow * odest = Destwin();

	if ( odest )
	    Destwin( 0 );

        if ( nsze.H > MAX_PAD_HEIGHT )
        {
          yuiDebug() << "TRUCNATE PAD: " << nsze.H << " > " << MAX_PAD_HEIGHT << endl;
          NCursesPad::resize( PAD_PAGESIZE, nsze.W );
          _vheight = nsze.H;
        }
        else
        {
	NCursesPad::resize( nsze.H, nsze.W );
          _vheight = 0;
        }

        yuiDebug() << "Pageing ?: " << pageing() << endl;

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
    if ( destwin )
    {
	if ( dirty )
	{
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

    switch ( key )
    {
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
