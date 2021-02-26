/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

   File:       ncursesp.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

/****************************************************************************
 * Copyright (c) 1998 Free Software Foundation, Inc.			    *
 *									    *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the	    *
 * "Software"), to deal in the Software without restriction, including	    *
 * without limitation the rights to use, copy, modify, merge, publish,	    *
 * distribute, distribute with modifications, sublicense, and/or sell	    *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:		    *
 *									    *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.		    *
 *									    *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF		    *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.				    *
 *									    *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written	    *
 * authorization.							    *
 ****************************************************************************/

/****************************************************************************
 *   Author: Juergen Pfeifer <Juergen.Pfeifer@T-Online.de> 1993,1997	    *
 ****************************************************************************/

#include <string.h>
#include <iostream>

#include "ncursesp.h"


NCursesPanel* NCursesPanel::dummy = ( NCursesPanel* )0;

void NCursesPanel::init()
{
    p = ::new_panel( w );

    if ( !p )
	OnError( ERR );

    UserHook* hook = new UserHook;

    hook->m_user  = NULL;

    hook->m_back  = this;

    hook->m_owner = p;

    ::set_panel_userptr( p, ( void * )hook );
}

NCursesPanel::~NCursesPanel()
{
    UserHook* hook = ( UserHook* )::panel_userptr( p );
    assert( hook && hook->m_back == this && hook->m_owner == p );
    delete hook;
    ::del_panel( p );
    ::update_panels();
    ::doupdate();
}

void
NCursesPanel::redraw()
{
    PANEL *pan;

    pan = ::panel_above( NULL );

    while ( pan )
    {
	::touchwin( panel_window( pan ) );
	pan = ::panel_above( pan );
    }

    ::update_panels();

    ::doupdate();
}

int
NCursesPanel::refresh()
{
    ::update_panels();
    return ::doupdate();
}

int
NCursesPanel::noutrefresh()
{
    ::update_panels();
    return OK;
}

void
NCursesPanel::boldframe( const char *title, const char* btitle )
{
    standout();
    frame( title, btitle );
    standend();
}

void
NCursesPanel::frame( const char *title, const char *btitle )
{
    int err = OK;

    if ( !title && !btitle )
    {
	err = box();
    }
    else
    {
	err = box();

	if ( err == OK )
	    label( title, btitle );
    }

    OnError( err );
}

void
NCursesPanel::label( const char *tLabel, const char *bLabel )
{
    if ( tLabel )
	centertext( 0, tLabel );

    if ( bLabel )
	centertext( maxy(), bLabel );
}

void
NCursesPanel::centertext( int row, const char *label )
{
    if ( label )
    {
	int x = ( maxx() - strlen( label ) ) / 2;

	if ( x < 0 )
	    x = 0;

	OnError( addstr( row, x, label, width() ) );
    }
}



int NCursesPanel::transparent( int y, int x )
{
    if ( hidden()
	 || y < 0 || maxy() < y
	 || x < 0 || maxx() < x )
    {
	return ERR;
    }

    int ay = begy() + y;

    int ax = begx() + x;

    for ( PANEL * sp = ::panel_below( p ); 1; sp = ::panel_below( sp ) )
    {
	WINDOW * sw = ( sp ? ::panel_window( sp ) : ::stdscr );

	if ( sw )
	{
	    int dy = ay - sw->_begy;

	    if ( 0 <= dy && dy <= sw->_maxy )
	    {
		int dx = ax - sw->_begx;

		if ( 0 <= dx && dx <= sw->_maxx )
		{
		    return addch( y, x, ::mvwinch( sw, dy, dx ) );
		}
	    }
	}

	if ( !sp )
	    break;
    }

    return ERR;
}


std::ostream & operator<<( std::ostream & Stream, const NCursesPanel * Obj_Cv )
{
    if ( Obj_Cv )
	return Stream << *Obj_Cv;

    return Stream << "(NoNCPan)";
}


std::ostream & operator<<( std::ostream & Stream, const NCursesPanel & Obj_Cv )
{
    return Stream << "NCPan(" << Obj_Cv.p << ')';
}

