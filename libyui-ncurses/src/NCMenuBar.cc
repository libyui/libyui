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
#include "YNCursesUI.h"

using std::string;
using std::vector;


NCMenuBar::NCMenuBar( YWidget * parent )
    : YMenuBar( parent )
    , NCPadWidget( parent )
{
    // yuiDebug() << endl;
    InitPad();
}


NCMenuBar::~NCMenuBar()
{
    // yuiDebug() << endl;
}


NCPad *
NCMenuBar::CreatePad()
{
    yuiDebug() << endl;

    wsze psze( defPadSze() );
    NCTablePad * npad = new NCTablePad( psze.H, psze.W, *this );
    npad->bkgd( listStyle().item.plain );
    npad->SetSepChar( ' ' );

    return npad;
}


void
NCMenuBar::rebuildMenuTree()
{
    vector<NCTableCol*> cells;

    NCTablePad * pad = myPad();
    YUI_CHECK_PTR( pad );
    pad->ClearTable();

    for ( YItemIterator it = itemsBegin(); it != itemsEnd(); ++it )
    {
	YMenuItem * item = dynamic_cast<YMenuItem *>( *it );
        YUI_CHECK_PTR( item );

        if ( ! item->isMenu() )
            YUI_THROW( YUIException( "NCMenuBar: Only menus allowed on toplevel." ) );

        yuiDebug() << "Adding " << item->label() << endl;
        NCTableCol * cell = new NCTableCol( item->label() );
        item->setUiItem( cell );
        cells.push_back( cell );
    }

    pad->Append( cells );
    // DrawPad();
    yuiDebug() << "Finish" << endl;
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
    yuiDebug() << endl;

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
            handleInput( key ); // Call base class input handler
            break;

    }
    return ret;
}


int
NCMenuBar::preferredWidth()
{
    wsze size = myPad()->tableSize() + 2;
    return std::max( size.W, 10 );
}


int
NCMenuBar::preferredHeight()
{
    return 3;
}


void
NCMenuBar::setSize( int newWidth, int newHeight )
{
    yuiDebug() << newWidth << ", " << newHeight << endl;
    wRelocate( wpos( 0 ), wsze( newHeight, newWidth ) );
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

