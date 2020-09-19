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

   File:       NCTablePad.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCTablePad.h"
#include "NCPopupMenu.h"

#include <limits.h>
#include <string>


NCTablePad::NCTablePad( int lines, int cols, const NCWidget & p )
    : NCTablePadBase( lines, cols, p )
    , sortStrategy ( new NCTableSortDefault )
{
}


NCTablePad::~NCTablePad()
{
}


int NCTablePad::DoRedraw()
{
    if ( !Destwin() )
    {
	dirty = true;
	return OK;
    }

    if ( _dirtyFormat )
	UpdateFormat();

    bkgdset( _itemStyle.getBG() );
    clear();

    wsze lineSize( 1, width() );

    if ( ! pageing() )
    {
        for ( unsigned i = 0; i < Lines(); ++i )
        {
            _items[i]->DrawAt( *this,
                               wrect( wpos( i, 0 ), lineSize ),
                               _itemStyle,
                               ( (unsigned) currentLineNo() == i ) );
        }
    }
    // else: item drawing requested via directDraw

    if ( _headpad.width() != width() )
	_headpad.resize( 1, width() );

    _headpad.clear();
    _itemStyle.Headline().DrawAt( _headpad,
                                  wrect( wpos( 0, 0 ), lineSize ),
                                  _itemStyle,
                                  false );
    SendHead();
    dirty = false;

    return update();
}



void NCTablePad::directDraw( NCursesWindow & w, const wrect at, unsigned lineNo )
{
    if ( lineNo < Lines() )
    {
        _items[ lineNo ]->DrawAt( w,
                                  at,
                                  _itemStyle,
                                  ( (unsigned) currentLineNo() == lineNo) );
    }
    else
        yuiWarning() << "Illegal Line no " << lineNo << " (" << Lines() << ")" << std::endl;
}



int NCTablePad::setpos( const wpos & newpos )
{
    if ( !Lines() )
    {
	if ( dirty || _dirtyFormat )
	    return DoRedraw();

	return OK;
    }

#if 0
    yuiDebug() << newpos << " : l " << Lines() << " : cl " << currentLineNo()
               << " : d " << dirty << " : df " << _dirtyFormat << std::endl;
#endif

    if ( _dirtyFormat )
	UpdateFormat();

    // save old values
    int oldLineNo = currentLineNo();

    int opos  = srect.Pos.C;

    // calc new values
    setCurrentLineNo( newpos.L < 0 ? 0 : newpos.L );

    if ( (unsigned) currentLineNo() >= Lines() )
	setCurrentLineNo( Lines() - 1 );

    srect.Pos = wpos( currentLineNo() - ( drect.Sze.H - 1 ) / 2, newpos.C ).between( 0, maxspos );

    if ( dirty )
	return DoRedraw();

    if ( ! pageing() )
    {
        // adjust only
        if ( currentLineNo() != oldLineNo )
        {
            _items[ oldLineNo ]->DrawAt( *this,
                                         wrect( wpos( oldLineNo, 0 ),
                                                wsze( 1, width() ) ),
                                         _itemStyle,
                                         false );
        }

        _items[ currentLineNo() ]->DrawAt( *this,
                                           wrect( wpos( currentLineNo(), 0 ),
                                                  wsze( 1, width() ) ),
                                           _itemStyle,
                                           true );
    }
    // else: item drawing requested via directDraw

    if ( srect.Pos.C != opos )
	SendHead();

    return update();
}


bool NCTablePad::setItemByKey( int key )
{
    if ( HotCol() >= Cols() )
	return false;

    if ( key < 0 || UCHAR_MAX < key )
	return false;

    unsigned hcol = HotCol();
    unsigned hkey = tolower( key );

    for ( unsigned i = 0; i < Lines(); ++i )
    {
	if ( _items[i]->GetCol( hcol )->hasHotkey()
	     && (unsigned) tolower( _items[i]->GetCol( hcol )->hotkey() ) == hkey )
	{
	    ScrlLine( i );
	    return true;
	}
    }

    return false;
}


/**
 * This sorts the table according to the given column by calling the sort
 * strategy. Sorting in reverse order is done if 'do_reverse' is set to 'true'.
 **/
void NCTablePad::setOrder( int col, bool do_reverse )
{
    if ( col < 0 )
	return;

    if ( sortStrategy->getColumn() != col )
    {
	sortStrategy->setColumn( col );
	sortStrategy->setReverse( false );
    }
    else if ( do_reverse )
    {
	sortStrategy->setReverse( !sortStrategy->isReverse() );
    }

    // libyui-ncurses-pkg relies on the fact that this function always
    // sorts

    sort();
}


void NCTablePad::sort()
{
    if (sortStrategy->getColumn() < 0)
	return;

    sortStrategy->sort( _items.begin(), _items.end() );

    dirty = true;
    update();
}


void NCTablePad::stripHotkeys()
{
    for ( unsigned i = 0; i < Lines(); ++i )
    {
	if ( _items[i] )
	{
	    _items[i]->stripHotkeys();
	}
    }
}


int NCTablePad::findIndexById( int id ) const
{
    // FIXME: "auto" variables are ugly and dangerous.
    // This throws all type checking right out of the window.

    auto begin = _items.begin();
    auto end   = _items.end();
    auto found = find_if( begin, end,
                          [id](NCTableLine * line)
                          {
                              return line->index() == id;
                          });

    if ( found == end )
	return -1;
    else
	return found - begin;
}
