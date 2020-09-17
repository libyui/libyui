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

   Author:     Michael Andres <ma@suse.de>

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


const NCTableLine * NCTreePad::GetCurrentLine() const
{
    if ( currentLineNo() >= 0 && (unsigned) currentLineNo() < visibleLines() )
	return _visibleItems[ currentLineNo() ];

    return 0;
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



wsze NCTreePad::UpdateFormat()
{
    dirty = true;
    _itemStyle.ResetToMinCols();

    for ( unsigned i = 0; i < Lines(); ++i )
	_items[i]->UpdateFormat( _itemStyle );

    _dirtyFormat = false;
    updateVisibleLines();

    maxspos.L = visibleLines() > (unsigned) srect.Sze.H ? visibleLines() - srect.Sze.H : 0;

    wsze size( visibleLines(), _itemStyle.TableWidth() );
    resize( size );

    return size;
}


void NCTreePad::updateVisibleLines()
{
    _visibleItems.clear();

    for ( unsigned i = 0; i < Lines(); ++i )
    {
	if ( _items[ i ]->isVisible() )
	    _visibleItems.push_back( _items[ i ] );
    }
}


int NCTreePad::DoRedraw()
{
    if ( !NCPad::Destwin() )
    {
	dirty = true;
	return OK;
    }

    if ( _dirtyFormat )
	UpdateFormat();

    // Set background and clear
    // (fill the window with the background color)

    bkgdset( _itemStyle.getBG() );
    clear();


    //
    // Draw content lines
    //

    wsze lineSize( 1, width() );

    for ( unsigned lineNo = 0; lineNo < visibleLines(); ++lineNo )
    {
	_visibleItems[ lineNo ]->DrawAt( *this,
                                         wrect( wpos( lineNo, 0 ), lineSize ),
                                         _itemStyle,
                                         ( lineNo == (unsigned) currentLineNo() ) );
    }

    if ( _headpad.width() != width() )
	_headpad.resize( 1, width() );

    //
    // Draw header
    //

    _headpad.clear();

    _itemStyle.Headline().DrawAt( _headpad,
                                  wrect( wpos( 0, 0 ), lineSize ),
                                  _itemStyle,
                                  false );
    SendHead();
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
    bool handled = true;

    if ( !GetCurrentLine() )
	return false;

    switch ( key )
    {
        //
        // Handle those keys in the parent NCPad class
        //

	case KEY_UP:
	case KEY_PPAGE:
	case KEY_DOWN:
	case KEY_NPAGE:
	    // handle these in compatible way with other widgets (#251180)
	    // jump to the first/last item

	case KEY_HOME:
	case KEY_END:
	    // scroll horizontally

	case KEY_RIGHT:
	case KEY_LEFT:
	    handled = NCPad::handleInput( key );
	    break;

        //
	// Handle those keys in the current item
        //

	case '+':
	case '-':
	case KEY_IC:    // "Insert" key ("Insert Character")
	case KEY_DC:    // "Delete" key ("Delete Character")
	case KEY_SPACE:
        // case KEY_RETURN: see bsc#67350

	    if ( _visibleItems[ currentLineNo() ]->handleInput( key ) )
	    {
		UpdateFormat();
		setpos( wpos( currentLineNo(), srect.Pos.C ) );
	    }

	    break;

	default:
	    handled = false;
    }

    return handled;
}

