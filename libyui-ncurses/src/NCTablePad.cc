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


int NCTablePad::findIndexById( int id ) const
{
    auto begin = Items.begin();
    auto end = Items.end();
    auto found = find_if( begin, end,
                          [id](NCTableLine * line) {
        return line->getIndex() == id;
    });

    if ( found == end )
	return -1;
    else
	return found - begin;
}


wsze NCTablePad::UpdateFormat()
{
    dirty = true;
    dirtyFormat = false;
    ItemStyle.ResetToMinCols();

    for ( unsigned l = 0; l < Lines(); ++l )
    {
	Items[l]->UpdateFormat( ItemStyle );
    }

    resize( wsze( Lines(), ItemStyle.TableWidth() ) );

    return wsze( Lines(), ItemStyle.TableWidth() );
}


int NCTablePad::DoRedraw()
{
    if ( !Destwin() )
    {
	dirty = true;
	return OK;
    }

    if ( dirtyFormat )
	UpdateFormat();

    bkgdset( ItemStyle.getBG() );

    clear();

    wsze lSze( 1, width() );

    if ( ! pageing() )
    {
        for ( unsigned l = 0; l < Lines(); ++l )
        {
            Items[l]->DrawAt( *this, wrect( wpos( l, 0 ), lSze ),
                              ItemStyle, (( unsigned )citem.L == l ) );
        }
    }
    // else: item drawing requested via directDraw

    if ( Headpad.width() != width() )
	Headpad.resize( 1, width() );

    Headpad.clear();

    ItemStyle.Headline().DrawAt( Headpad, wrect( wpos( 0, 0 ), lSze ),
				 ItemStyle, false );

    SendHead();

    dirty = false;

    return update();
}



void NCTablePad::directDraw( NCursesWindow & w, const wrect at, unsigned lineno )
{
    if ( lineno < Lines() )
        Items[lineno]->DrawAt( w, at, ItemStyle, ((unsigned)citem.L == lineno) );
    else
        yuiWarning() << "Illegal Line no " << lineno << " (" << Lines() << ")" << std::endl;
}



int NCTablePad::setpos( const wpos & newpos )
{
    if ( !Lines() )
    {
	if ( dirty || dirtyFormat )
	    return DoRedraw();

	return OK;
    }

#if 0
    yuiDebug() << newpos << " : l " << Lines() << " : cl " << citem.L
               << " : d " << dirty << " : df " << dirtyFormat << std::endl;
#endif

    if ( dirtyFormat )
	UpdateFormat();

    // save old values
    int oitem = citem.L;

    int opos  = srect.Pos.C;

    // calc new values
    citem.L = newpos.L < 0 ? 0 : newpos.L;

    if (( unsigned )citem.L >= Lines() )
	citem.L = Lines() - 1;

    srect.Pos = wpos( citem.L - ( drect.Sze.H - 1 ) / 2, newpos.C ).between( 0, maxspos );

    if ( dirty )
    {
	return DoRedraw();
    }

    if ( ! pageing() )
    {
        // adjust only
        if ( citem.L != oitem )
        {
            Items[oitem]->DrawAt( *this, wrect( wpos( oitem, 0 ), wsze( 1, width() ) ),
                                  ItemStyle, false );
        }

        Items[citem.L]->DrawAt( *this, wrect( wpos( citem.L, 0 ), wsze( 1, width() ) ),
                                ItemStyle, true );
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

    for ( unsigned l = 0; l < Lines(); ++l )
    {
	if ( Items[l]->GetCol( hcol )->hasHotkey()
	     && (unsigned) tolower( Items[l]->GetCol( hcol )->hotkey() ) == hkey )
	{
	    ScrlLine( l );
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

    sortStrategy->sort( Items.begin(), Items.end() );

    dirty = true;
    update();
}


void NCTablePad::stripHotkeys()
{
    for ( unsigned i = 0; i < Lines(); ++i )
    {
	if ( Items[i] )
	{
	    Items[i]->stripHotkeys();
	}
    }
}


