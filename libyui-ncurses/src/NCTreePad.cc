/*
  Copyright (C) 2000-2012 Novell, Inc
  Copyright (C) 2020 SUSE LLC
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


bool NCTreePad::handleInput( wint_t key )
{
    bool handled = false;

    switch ( key )
    {
        // At this time, there are no more special keys to handle on this
        // level. This method is a stub for future extension if any more keys
        // need to be handled.
        //
        // Add 'case KEY_XXX' branches here if there should be any
        // and don't forget to set 'handled' to 'true'.
#if 0
        case KEY_SOMETHING:     // Sample
            doSomething();
            handled = true;
            break;
#endif

        default: // Call parent class input handler

            // This also calls currentItemHandleInput() as the first thing it
            // does to forward key presses to the item at the current cursor
            // position. Many operations such as opening, closing or selecting
            // an item are done on that level.
            handled = NCTablePadBase::handleInput( key );
            break;
    }

    return handled;
}

