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

#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <vector>

#define	 YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCItemSelector.h"

using std::string;



NCItemSelector::NCItemSelector( YWidget * parent, bool enforceSingleSelection )
    : YItemSelector( parent, enforceSingleSelection )
    , NCPadWidget( parent )
    , prefSize( 50, 5 ) // width, height
    , prefSizeDirty( true )
{
    yuiDebug() << endl;
    InitPad();
}


NCItemSelector::~NCItemSelector()
{
    yuiDebug() << endl;
}


int NCItemSelector::preferredWidth()
{
    return preferredSize().W;
}


int NCItemSelector::preferredHeight()
{
    return preferredSize().H;
}


wsze NCItemSelector::preferredSize()
{
    if ( prefSizeDirty )
    {
	const int minHeight	= 5;	// 2 frame lines + 3 lines for content
	const int minWidth	= 20;
	const int selectorWidth = string( "|[x] |" ).size();
	int visibleItemsCount	= std::min( itemsCount(), visibleItems() );

	prefSize.W = 0;
	prefSize.H = 0;

	for ( int i=0; i < visibleItemsCount; ++i )
	{
	    if ( prefSize.H > i )	// need a separator line?
		++prefSize.H;		// for the separator line

	    ++prefSize.H;		// For the item label

	    std::vector<string> lines = descriptionLines( itemAt( i ) );
            prefSize.H += lines.size();

	    for ( const string & line: lines )	// as wide as the longest line
		prefSize.W = std::max( prefSize.W, (int) line.size() + selectorWidth );
	}

	prefSize.H   += 2; // for the frame lines
	prefSize.W    = std::max( prefSize.W, minWidth	);
	prefSize.H    = std::max( prefSize.H, minHeight );
	prefSizeDirty = false;
    }

    return prefSize;
}


void NCItemSelector::setVisibleItems( int newVal )
{
    prefSizeDirty = true;
    YItemSelector::setVisibleItems( newVal );
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

    NCTableTag * tag = tagCell( myPad()->CurPos().L );

    return tag ? tag->origItem() : 0;
}


void NCItemSelector::setCurrentItem( YItem * item )
{
    if ( item )
	myPad()->ScrlLine( item->index() );
}


void NCItemSelector::addItem( YItem * item )
{
    std::vector<NCTableCol*> cells( 2U, 0 );

    if ( item )
    {
	prefSizeDirty = true;
	int lineNo = myPad()->Lines();

	if ( lineNo > itemsCount() )
	{
	    // Add a blank line as a separator from the previous item
	    //
	    // ...but only if there is any previous item that had a description.
	    // If there are only items without description, we don't need separator lines.

	    cells[0] = new NCTableCol( "",   NCTableCol::SEPARATOR );
	    cells[1] = new NCTableCol( "",   NCTableCol::SEPARATOR );
	    myPad()->Append( cells );
	}

	yuiDebug() << "Adding new item " << item->label() << " at line #" << lineNo << endl;

	// Add the item label with "[ ]" or "( )" for selection

	YItemSelector::addItem( item );
	cells[0] = new NCTableTag( item, item->selected(), enforceSingleSelection() );
	cells[1] = new NCTableCol( item->label() );

	NCTableLine * tableLine = new NCTableLine( cells );
	myPad()->Append( tableLine );


	// Add the item description (possible multi-line)

	std::vector<string> lines = descriptionLines( item );

	for ( const string & line: lines )
	{
	    cells[0] = new NCTableCol( "",   NCTableCol::PLAIN );
	    cells[1] = new NCTableCol( line, NCTableCol::PLAIN );
	    myPad()->Append( cells );
	}

	DrawPad();
    }
}


string NCItemSelector::description( YItem * item ) const
{
    string desc;

    if ( item )
    {
	YDescribedItem * descItem = dynamic_cast<YDescribedItem *>( item );

	if ( descItem )
	    desc = descItem->description();
    }

    return desc;
}


std::vector<std::string>
NCItemSelector::descriptionLines( YItem * item ) const
{
    std::vector<std::string> lines;

    // This temporary variable is only needed to work around a bug in older boost versions:
    // https://github.com/boostorg/algorithm/commit/c6f784cb

    string desc = description( item );
    boost::split( lines, desc, boost::is_any_of( "\n" ) );

    return lines;
}


/**
 * Return pointer to current line tag
 * (holds state and YItem pointer)
 **/
NCTableTag * NCItemSelector::tagCell( int index )
{
    NCTableLine * tableLine = myPad()->ModifyLine( index );

    if ( ! tableLine )
	return 0;

    return dynamic_cast<NCTableTag *> ( tableLine->GetCol( 0 ) );
}


const NCTableTag * NCItemSelector::tagCell( int index ) const
{
    const NCTableLine * tableLine = myPad()->GetLine( index );

    if ( ! tableLine )
	return 0;

    return dynamic_cast<const NCTableTag *>( tableLine->GetCol( 0 ) );
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

	NCTableTag * tag = (NCTableTag *) yitem->data();
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


void NCItemSelector::deselectAllItemsExcept( YItem * exceptItem )
{
    for ( YItemIterator it = itemsBegin(); it != itemsEnd(); ++it )
    {
	if ( *it != exceptItem )
	{
	    (*it)->setSelected( false );
	    NCTableTag * tag = (NCTableTag *) (*it)->data();

	    if ( tag )
		tag->SetSelected( false );
	}
    }

    DrawPad();
}


void NCItemSelector::toggleCurrentItem()
{
    YItem *yItem = currentItem();

    if ( yItem )
    {
	if ( enforceSingleSelection() )
	{
	    selectItem( yItem, true );
	    deselectAllItemsExcept( yItem );
	}
	else // Multi-selection
	{
	    selectItem( yItem, !( yItem->selected() ) );
	}
    }
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
