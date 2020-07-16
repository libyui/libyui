/*
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

   File:       NCMenuBar.cc

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <algorithm>    // std::max()

#define	 YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCMenuBar.h"
#include "NCPulldownMenu.h"
#include "YNCursesUI.h"


// Margin to the left of the first toplevel item
#define LEFT_MARGIN     0

// Spacing between toplevel items
#define SPACING         0

using std::string;


NCMenuBar::NCMenuBar( YWidget * parent )
    : YMenuBar( parent )
    , NCWidget( parent )
{
    // yuiDebug() << endl;
    wstate = NC::WSdumb;
}


NCMenuBar::~NCMenuBar()
{
    // yuiDebug() << endl;

    clear();
}


void
NCMenuBar::clear()
{
    for ( unsigned i=0; i < _menus.size(); ++i )
        delete _menus[i];

    _menus.clear();
    defsze = wsze( 1, 10 );
}


void
NCMenuBar::rebuildMenuTree()
{
    clear();
    int width = 0;

    for ( YItemIterator it = itemsBegin(); it != itemsEnd(); ++it )
    {
	YMenuItem * item = dynamic_cast<YMenuItem *>( *it );
        YUI_CHECK_PTR( item );

        if ( ! item->isMenu() )
            YUI_THROW( YUIException( "NCMenuBar: Only menus allowed on toplevel." ) );

        yuiDebug() << "Adding " << item->label() << endl;
        NCPulldownMenu * menu = new NCPulldownMenu( this, item->label() );
        item->setUiItem( menu );
        _menus.push_back( menu );

        if ( width > 0 )
            width += SPACING;

        width += menu->preferredWidth();
    }

    width += LEFT_MARGIN;
    defsze = wsze( 1, width );

    // yuiDebug() << "Finish" << endl;
}


NCursesEvent
NCMenuBar::postMenu()
{
    NCursesEvent ret;
    // TO DO

    return ret;
}


void
NCMenuBar::setItemEnabled( YMenuItem * item, bool enabled )
{
    // TO DO
}


void
NCMenuBar::activateItem( YMenuItem * item )
{
    // TO DO
}


NCursesEvent
NCMenuBar::wHandleInput( wint_t key )
{
    NCursesEvent ret;

    switch ( key )
    {
        case KEY_LEFT:
            // TO DO
            break;

        case KEY_RIGHT:
            // TO DO
            break;

        case KEY_DOWN:  // Open the current menu
            // TO DO
            break;

        default:
            wHandleInput( key ); // Call base class input handler
            break;

    }
    return ret;
}


int
NCMenuBar::preferredWidth()
{
    return defsze.W;
}


int
NCMenuBar::preferredHeight()
{
    return 1;
}


void
NCMenuBar::setSize( int newWidth, int newHeight )
{
    wRelocate( wpos( 0 ), wsze( newHeight, newWidth ) );
    layoutChildren( newWidth, newHeight );
}


void
NCMenuBar::layoutChildren( int newWidth, int newHeight )
{
    (void) newHeight;   // unused
    int pos = LEFT_MARGIN;

    for ( unsigned i=0; i < _menus.size(); ++i )
    {
        NCPulldownMenu * menu = _menus[i];
        int width = menu->preferredWidth();

        if ( pos + width < newWidth )
        {
            menu->resizeToContent();
            wMoveChildTo( *menu, wpos( 0, pos ) );
        }

        pos += width;

        if ( i > 0 )
            pos += SPACING;
    }
}


void
NCMenuBar::setEnabled( bool enabled )
{
    NCWidget::setEnabled( enabled );
    YMenuBar::setEnabled( enabled );
}


bool
NCMenuBar::setKeyboardFocus()
{
    // TO DO
    return true;
}

