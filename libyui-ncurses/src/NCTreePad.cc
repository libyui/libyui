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

   File:       NCTreePad.cc

   Authors:    Michael Andres <ma@suse.de>
               Stefan Hundhammer <shundhammer@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCTreePad.h"



NCTreePad::NCTreePad( int lines, int cols, const NCWidget & p )
    : NCTablePadBase( lines, cols, p )
{
}


NCTreePad::~NCTreePad()
{
}


void NCTreePad::Destwin( NCursesWindow * dwin )
{
    NCPad::Destwin( dwin );

    if ( destwin )
    {
	maxspos.L = visibleLines() > (unsigned) srect.Sze.H ? visibleLines() - srect.Sze.H : 0;
    }
}


void NCTreePad::ShowItem( const NCTableLine * item )
{
    if ( !item )
	return;

    if ( const_cast<NCTableLine *>( item )->ChangeToVisible() || _dirtyFormat )
	UpdateFormat();

    for ( unsigned i = 0; i < visibleLines(); ++i )
    {
	if ( _visibleItems[i] == item )
	{
	    setpos( wpos( i, srect.Pos.C ) );
	    break;
	}
    }
}


int NCTreePad::DoRedraw()
{
    if ( !NCPad::Destwin() )
    {
	dirty = true;
	return OK;
    }

    prepareRedraw();
    drawContentLines();
    drawHeader();

    dirty = false;

    return update();
}



int NCTreePad::setpos( const wpos & newpos )
{
    if ( !visibleLines() )
    {
	if ( dirty )
	    return DoRedraw();

	return OK;
    }

    if ( _dirtyFormat )
	UpdateFormat();

    // save old values
    int oldLineNo = currentLineNo();
    int oldPos    = srect.Pos.C;

    // calc new values
    setCurrentLineNo( newpos.L < 0 ? 0 : newpos.L );

    if ( (unsigned) currentLineNo() >= visibleLines() )
	setCurrentLineNo( visibleLines() - 1 );

    srect.Pos = wpos( currentLineNo() - ( drect.Sze.H - 1 ) / 2, newpos.C ).between( 0, maxspos );

    if ( currentLineNo() != oldLineNo )
    {
	unsigned at  = 0;
	unsigned len = 0;

	if ( currentLineNo() >= 0 && _visibleItems[ currentLineNo() ] )
        {
	    len = _visibleItems[ currentLineNo() ]->Hotspot( at );
        }
	else
        {
	    return ERR;
        }

	if ( len )
	{
	    if ( (int) at < srect.Pos.C )
	    {
		srect.Pos.C = at;
	    }
	    else if ( (int) ( at + len - srect.Pos.C ) > drect.Sze.W )
	    {
		srect.Pos.C = (int) at < maxspos.C ? at : maxspos.C;
	    }
	}
    }

    if ( dirty )
	return DoRedraw();

    // adjust only

    if ( currentLineNo() != oldLineNo )
    {
	_visibleItems[ oldLineNo ]->DrawAt( *this,
                                            wrect( wpos( oldLineNo, 0 ),
                                                   wsze( 1, width() ) ),
                                            _itemStyle,
                                            false );
    }

    _visibleItems[ currentLineNo() ]->DrawAt( *this,
                                              wrect( wpos( currentLineNo(), 0 ),
                                                     wsze( 1, width() ) ),
                                              _itemStyle,
                                              true );

    if ( srect.Pos.C != oldPos )
	SendHead();

    return update();
}


bool NCTreePad::handleInput( wint_t key )
{
    bool handled = false;

    switch ( key )
    {
        // At this time, there are no more special keys to handle at this
        // level.
        //
        // Add 'case KEY_XXX' branches here if there should be any
        // and don't forget to set 'handled' to 'true'.

        default: // Call parent class input handler

            handled = NCTablePadBase::handleInput( key );
            break;
    }

    return handled;
}

