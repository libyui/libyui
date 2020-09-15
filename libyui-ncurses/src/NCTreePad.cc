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
    if ( citem.L >= 0 && (unsigned) citem.L < visibleLines() )
	return visibleItems[citem.L];

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

    if ( const_cast<NCTableLine *>( item )->ChangeToVisible() || dirtyFormat )
	UpdateFormat();

    for ( unsigned l = 0; l < visibleLines(); ++l )
    {
	if ( visibleItems[l] == item )
	{
	    setpos( wpos( l, srect.Pos.C ) );
	    break;
	}
    }
}



wsze NCTreePad::UpdateFormat()
{
    dirty = true;
    dirtyFormat = false;
    visibleItems.clear();
    ItemStyle.ResetToMinCols();

    for ( unsigned l = 0; l < Lines(); ++l )
    {
	Items[l]->UpdateFormat( ItemStyle );

	if ( Items[l]->isVisible() )
	    visibleItems.push_back( Items[l] );
    }

    maxspos.L = visibleLines() > (unsigned) srect.Sze.H ? visibleLines() - srect.Sze.H : 0;

    resize( wsze( visibleLines(), ItemStyle.TableWidth() ) );
    return wsze( visibleLines(), ItemStyle.TableWidth() );
}



int NCTreePad::DoRedraw()
{
    if ( !NCPad::Destwin() )
    {
	dirty = true;
	return OK;
    }

    if ( dirtyFormat )
	UpdateFormat();

    bkgdset( ItemStyle.getBG() );

    clear();

    wsze lSze( 1, width() );

    for ( unsigned l = 0; l < visibleLines(); ++l )
    {
	visibleItems[l]->DrawAt( *this, wrect( wpos( l, 0 ), lSze ),
			     ItemStyle, ( l == (unsigned) citem.L ) );
    }

    if ( Headpad.width() != width() )
	Headpad.resize( 1, width() );

    Headpad.clear();

    ItemStyle.Headline().DrawAt( Headpad, wrect( wpos( 0, 0 ), lSze ),
				 ItemStyle, false );

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

    if ( dirtyFormat )
	UpdateFormat();

    // save old values
    int oitem = citem.L;

    int opos  = srect.Pos.C;

    // calc new values
    citem.L = newpos.L < 0 ? 0 : newpos.L;

    if ( (unsigned) citem.L >= visibleLines() )
	citem.L = visibleLines() - 1;

    srect.Pos = wpos( citem.L - ( drect.Sze.H - 1 ) / 2, newpos.C ).between( 0, maxspos );

    if ( citem.L != oitem )
    {
	unsigned at  = 0;
	unsigned len = 0;

	if ( citem.L >= 0 && visibleItems[citem.L] )
	    len = visibleItems[citem.L]->Hotspot( at );
	else
	    return ERR;

	if ( len )
	{
	    if ((int) at < srect.Pos.C )
	    {
		srect.Pos.C = at;
	    }
	    else if ((int) ( at + len - srect.Pos.C ) > drect.Sze.W )
	    {
		srect.Pos.C = (int) at < maxspos.C ? at : maxspos.C;
	    }
	}
    }

    if ( dirty )
	return DoRedraw();

    // adjust only
    if ( citem.L != oitem )
    {
	visibleItems[oitem]->DrawAt( *this, wrect( wpos( oitem, 0 ), wsze( 1, width() ) ),
				 ItemStyle, false );
    }

    visibleItems[citem.L]->DrawAt( *this, wrect( wpos( citem.L, 0 ), wsze( 1, width() ) ),
			       ItemStyle, true );

    if ( srect.Pos.C != opos )
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
	case KEY_UP:
	case KEY_PPAGE:
	case KEY_DOWN:
	case KEY_NPAGE:
	    //handle these in compatible way with other widgets (#251180)
	    //jump to the first/last item

	case KEY_HOME:
	case KEY_END:
	    //scroll horizontally

	case KEY_RIGHT:
	case KEY_LEFT:
	    handled = NCPad::handleInput( key );
	    break;

	    //use these for toggling pack/unpack the tree

	case '+':
	case '-':
	case KEY_IC:
	case KEY_DC:
	case KEY_SPACE:
        //  case KEY_RETURN: - see bug 67350

	    if ( visibleItems[citem.L]->handleInput( key ) )
	    {
		UpdateFormat();
		setpos( wpos( citem.L, srect.Pos.C ) );
	    }

	    break;

	default:
	    handled = false;
    }

    return handled;
}

