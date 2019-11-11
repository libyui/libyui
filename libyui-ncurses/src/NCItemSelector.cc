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
using std::vector;



NCItemSelectorBase::NCItemSelectorBase( YWidget * parent, bool enforceSingleSelection )
    : YItemSelector( parent, enforceSingleSelection )
    , NCPadWidget( parent )
    , _prefSize( 50, 5 ) // width, height
    , _prefSizeDirty( true )
    , _selectorWidth( string( "|[x] |" ).size() )
{
    yuiDebug() << endl;
    InitPad();
}


NCItemSelectorBase::NCItemSelectorBase( YWidget *			parent,
					const YItemCustomStatusVector & customStates )
    : YItemSelector( parent, customStates )
    , NCPadWidget( parent )
    , _prefSize( 50, 5 ) // width, height
    , _prefSizeDirty( true )
    , _selectorWidth( 0 )
{
    yuiDebug() << endl;
    InitPad();

    // Find the longest text indicator
    
    for ( int i=0; i < customStatusCount(); ++i )
    {
	int len = customStatus( i ).textIndicator().size();

	if ( _selectorWidth < len )
	    _selectorWidth = len;
    }

    _selectorWidth += string( "| |" ).size();
}


NCItemSelectorBase::~NCItemSelectorBase()
{
    yuiDebug() << endl;
}


NCPad * NCItemSelectorBase::CreatePad()
{
    wsze psze( defPadSze() );
    NCTablePad * npad = new NCTablePad( psze.H, psze.W, *this );
    npad->bkgd( listStyle().item.plain );
    npad->SetSepChar( ' ' );
    return npad;
}


int NCItemSelectorBase::preferredWidth()
{
    return preferredSize().W;
}


int NCItemSelectorBase::preferredHeight()
{
    return preferredSize().H;
}


wsze NCItemSelectorBase::preferredSize()
{
    if ( _prefSizeDirty )
    {
	const int minHeight	= 5;	// 2 frame lines + 3 lines for content
	const int minWidth	= 20;
	int visibleItemsCount	= std::min( itemsCount(), visibleItems() );

	_prefSize.W = 0;
	_prefSize.H = 0;

	for ( int i=0; i < visibleItemsCount; ++i )
	{
	    if ( _prefSize.H > i )	// need a separator line?
		++_prefSize.H;		// for the separator line

	    ++_prefSize.H;		// For the item label

	    vector<string> lines = descriptionLines( itemAt( i ) );
	    _prefSize.H += lines.size();

	    for ( const string & line: lines )	// as wide as the longest line
		_prefSize.W = std::max( _prefSize.W, (int) line.size() + _selectorWidth );
	}

	_prefSize.H   += 2; // for the frame lines
	_prefSize.W    = std::max( _prefSize.W, minWidth  );
	_prefSize.H    = std::max( _prefSize.H, minHeight );
	_prefSizeDirty = false;
    }

    return _prefSize;
}


void NCItemSelectorBase::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


bool NCItemSelectorBase::setKeyboardFocus()
{
    if ( ! grabFocus() )
	return YWidget::setKeyboardFocus();

    return true;
}


void NCItemSelectorBase::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YItemSelector::setEnabled( do_bv );
}


void NCItemSelectorBase::setVisibleItems( int newVal )
{
    _prefSizeDirty = true;
    YItemSelector::setVisibleItems( newVal );
}


YItem * NCItemSelectorBase::currentItem() const
{
    if ( !myPad()->Lines() )
	return 0;

    NCTableTag * tag = tagCell( currentLine() );

    return tag ? tag->origItem() : 0;
}


void NCItemSelectorBase::setCurrentItem( YItem * item )
{
    if ( item )
	myPad()->ScrlLine( item->index() );
}


void NCItemSelectorBase::addItem( YItem * item )
{
    vector<NCTableCol*> cells( 2U, 0 );

    if ( item )
    {
	_prefSizeDirty = true;
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
	cells[0] = createTagCell( item );
	cells[1] = new NCTableCol( item->label() );

	NCTableLine * tableLine = new NCTableLine( cells );
	myPad()->Append( tableLine );


	// Add the item description (possible multi-line)

	vector<string> lines = descriptionLines( item );

	for ( const string & line: lines )
	{
	    cells[0] = new NCTableCol( "",   NCTableCol::PLAIN );
	    cells[1] = new NCTableCol( line, NCTableCol::PLAIN );
	    myPad()->Append( cells );
	}

	DrawPad();
    }
}


NCTableTag * NCItemSelectorBase::tagCell( int index ) const
{
    NCTableLine * tableLine = myPad()->ModifyLine( index );

    if ( ! tableLine )
	return 0;

    return dynamic_cast<NCTableTag *> ( tableLine->GetCol( 0 ) );
}


string NCItemSelectorBase::description( YItem * item ) const
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


vector<string>
NCItemSelectorBase::descriptionLines( YItem * item ) const
{
    vector<string> lines;

    // This temporary variable is only needed to work around a bug in older boost versions:
    // https://github.com/boostorg/algorithm/commit/c6f784cb

    string desc = description( item );
    boost::split( lines, desc, boost::is_any_of( "\n" ) );

    return lines;
}


void NCItemSelectorBase::deleteAllItems()
{
    YItemSelector::deleteAllItems();
    myPad()->ClearTable();
    DrawPad();
}


void NCItemSelectorBase::selectItem( YItem *yitem, bool selected )
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


void NCItemSelectorBase::deselectAllItems()
{
    YItemSelector::deselectAllItems();

    for ( int i = 0; i < linesCount(); i++ )
    {
	NCTableTag * tag = tagCell( i );

	if ( tag )
	    tag->SetSelected( false );
    }

    DrawPad();
}


YItem *
NCItemSelectorBase::scrollDownToNextItem()
{
    while ( currentLine() < linesCount() - 1 )
    {
	YItem * item = currentItem();

	if ( item )
	    return item;

	myPad()->ScrlDown();
	yuiMilestone() << "Scrolling one line down" << endl;
    }

    return 0;
}


YItem *
NCItemSelectorBase::scrollUpToPreviousItem()
{
    while ( currentLine() >= 0 )
    {
	YItem * item = currentItem();

	if ( item )
	    return item;

	myPad()->ScrlUp();
	yuiMilestone() << "Scrolling one line up" << endl;
    }

    return 0;
}


// ----------------------------------------------------------------------



NCItemSelector::NCItemSelector( YWidget * parent, bool enforceSingleSelection )
    : NCItemSelectorBase( parent, enforceSingleSelection )
{
    yuiDebug() << endl;
}


NCItemSelector::~NCItemSelector()
{
    yuiDebug() << endl;
}


NCTableTag *
NCItemSelector::createTagCell( YItem * item )
{
    NCTableTag * tag = new NCTableTag( item, item->selected(), enforceSingleSelection() );
    YUI_CHECK_NEW( tag );

    return tag;
}


void NCItemSelector::cycleCurrentItemStatus()
{
    YItem *item = currentItem();

    if ( item )
    {
	if ( enforceSingleSelection() )
	{
	    selectItem( item, true );
	    deselectAllItemsExcept( item );
	}
	else // Multi-selection
	{
	    selectItem( item, !( item->selected() ) );
	}
    }
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


NCursesEvent
NCItemSelector::wHandleInput( wint_t key )
{
    NCursesEvent ret;
    bool valueChanged = false;
    YItem *oldCurrentItem = currentItem();

    if ( ! handleInput( key ) )
    {
	YItem *curItem = currentItem();

	switch ( key )
	{
	    case KEY_SPACE:
	    case KEY_RETURN:

                if ( ! curItem )
                    curItem = scrollUpToPreviousItem();

                if ( curItem )
                {
                    cycleCurrentItemStatus();
                    valueChanged = true;
                }
		break;


	    case '+':

                if ( ! curItem )
                    curItem = scrollUpToPreviousItem();

		if ( curItem && ! curItem->selected() )
		{
		    selectItem( curItem, true );

                    if ( enforceSingleSelection() )
                        deselectAllItemsExcept( curItem );

		    valueChanged = true;
		}

		myPad()->ScrlDown();
                curItem = scrollDownToNextItem();

                if ( ! curItem ) // That was the last one
                    curItem = scrollUpToPreviousItem();

		break;


	    case '-':

                if ( ! enforceSingleSelection() )
                {
                    if ( ! curItem )
                        curItem = scrollUpToPreviousItem();

                    if ( curItem )
                    {
                        if ( curItem->selected() )
                        {
                            selectItem( curItem, false );
                            valueChanged = true;
                        }

                        myPad()->ScrlDown();
                        curItem = scrollDownToNextItem();

                        if ( ! curItem ) // That was the last one
                            curItem = scrollUpToPreviousItem();
                    }
                }

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

