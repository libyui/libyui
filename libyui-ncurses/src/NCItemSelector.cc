/*
  Copyright (C) 2019 SUSE LLC
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

   File:       NCItemSelector.cc

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCItemSelector.h"


NCItemSelector::NCItemSelector( YWidget * parent, bool enforceSingleSelection )
    : YItemSelector( parent, enforceSingleSelection )
    , NCPadWidget( parent )
{
    yuiDebug() << std::endl;
    InitPad();
}


NCItemSelector::~NCItemSelector()
{
    yuiDebug() << std::endl;
}


int NCItemSelector::preferredWidth()
{
    wsze sze = wGetDefsze();

#if 0
    return sze.W > (int)( labelWidth() + 2 ) ? sze.W : ( labelWidth() + 2 );
#endif
    // FIXME: TO DO
    // FIXME: TO DO
    // FIXME: TO DO
    return 42;
}


int NCItemSelector::preferredHeight()
{
    return wGetDefsze().H;
}


void NCItemSelector::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YItemSelector::setEnabled( do_bv );
}


void NCItemSelector::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


YItem * NCItemSelector::currentItem()
{
    if ( !myPad()->Lines() )
	return 0;

    int index = myPad()->CurPos().L;

    return itemAt( index );
}


void NCItemSelector::setCurrentItem( YItem * item )
{
    if ( item )
	myPad()->ScrlLine( item->index() );
}


void NCItemSelector::addItem( YItem * item )
{
    std::vector<NCTableCol*> Items( 2U, 0 );

    if ( item )
    {
	YItemSelector::addItem( item );
	Items[0] = new NCTableTag( item, item->selected(), enforceSingleSelection() );

	// Do not set style to NCTableCol::PLAIN here, otherwise the current
	//item will not be highlighted if the cursor is not over the widget

	Items[1] = new NCTableCol( item->label() );
	myPad()->Append( Items );
	DrawPad();
    }
}


/**
 * Return pointer to current line tag
 * (holds state and yitem pointer)
 **/
NCTableTag * NCItemSelector::tagCell( int index )
{
    NCTableLine * cl = myPad()->ModifyLine( index );

    if ( !cl )
	return 0;

    return static_cast<NCTableTag *>( cl->GetCol( 0 ) );
}


const NCTableTag * NCItemSelector::tagCell( int index ) const
{
    const NCTableLine * cl = myPad()->GetLine( index );

    if ( !cl )
	return 0;

    return static_cast<const NCTableTag *>( cl->GetCol( 0 ) );
}


void NCItemSelector::deleteAllItems()
{
    YItemSelector::deleteAllItems();
    myPad()->ClearTable();
    DrawPad();
}


bool NCItemSelector::isItemSelected( YItem *item )
{
    if ( item )
	return item->selected();
    else
	return false;
}


void NCItemSelector::selectItem( YItem *yitem, bool selected )
{
    if ( yitem )
    {
	YItemSelector::selectItem( yitem, selected );

	// retrieve pointer to the line tag associated with this item
	NCTableTag * tag = ( NCTableTag * ) yitem->data();
	YUI_CHECK_PTR( tag );

	tag->SetSelected( selected );

	DrawPad();
    }
}


void NCItemSelector::deselectAllItems()
{
    YItemSelector::deselectAllItems();

    for ( unsigned int i = 0; i < getNumLines(); i++ )
    {
	NCTableTag * tag = tagCell( i );

        if ( tag )
            tag->SetSelected( false );
    }

    DrawPad();
}


void NCItemSelector::toggleCurrentItem()
{
    YItem *it = currentItem();
    if ( it )
	selectItem( it, !( it->selected() ) );
}


/**
 * Create empty MsB pad
 **/
NCPad * NCItemSelector::CreatePad()
{
    wsze psze( defPadSze() );
    NCTablePad * npad = new NCTablePad( psze.H, psze.W, *this );
    npad->bkgd( listStyle().item.plain );
    npad->SetSepChar( ' ' );
    return npad;
}


void NCItemSelector::wRecoded()
{
    NCPadWidget::wRecoded();
}


NCursesEvent NCItemSelector::wHandleInput( wint_t key )
{
    NCursesEvent ret;
    bool valueChanged = false;
    YItem *oldCurrentItem = currentItem();

    if ( ! handleInput( key ) )
    {
	YItem *citem = currentItem();

	switch ( key )
	{
	    case KEY_SPACE:

	    case KEY_RETURN:
		toggleCurrentItem();
		valueChanged = true;
		break;

	    case '+':

		if ( !isItemSelected( citem ) )
		{
		    selectItem( citem, true );
		    valueChanged = true;
		}

		myPad()->ScrlDown();

		break;

	    case '-':

		if ( isItemSelected( citem ) )
		{
		    selectItem( citem, false );
		    valueChanged = true;
		}

		myPad()->ScrlDown();

		break;
	}
    }

    if ( notify() )
    {
	if ( valueChanged )
	    ret = NCursesEvent::ValueChanged;
	else if ( oldCurrentItem != currentItem() )
	    ret = NCursesEvent::SelectionChanged;
    }

    return ret;
}
