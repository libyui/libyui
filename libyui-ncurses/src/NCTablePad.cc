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

   File:       NCTablePad.cc

   Authors:    Michael Andres <ma@suse.de>
               Stefan Hundhammer <shundhammer@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCTablePad.h"



NCTablePad::NCTablePad( int lines, int cols, const NCWidget & p )
    : NCTablePadBase( lines, cols, p )
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

    prepareRedraw();

    if ( ! paging() )
        drawContentLines();
    // else
    //   item drawing requested via directDraw()

    drawHeader();

    dirty = false;

    return update();
}


void NCTablePad::directDraw( NCursesWindow & w, const wrect at, unsigned lineNo )
{
    if ( lineNo < Lines() )
    {
        _visibleItems[ lineNo ]->DrawAt( w,
                                         at,
                                         _itemStyle,
                                         ( (unsigned) currentLineNo() == lineNo) );
    }
    else
        yuiWarning() << "Illegal Line no " << lineNo << " (" << Lines() << ")" << std::endl;
}


bool NCTablePad::handleInput( wint_t key )
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


bool NCTablePad::setItemByKey( int key )
{
    if ( HotCol() >= Cols() )
	return false;

    if ( key < 0 || UCHAR_MAX < key )
	return false;

    unsigned hcol = HotCol();
    unsigned hkey = tolower( key );

    for ( unsigned i = 0; i < visibleLines(); ++i )
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
