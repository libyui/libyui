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
#include "YNCursesUI.h"

using std::string;
using std::vector;



NCItemSelectorBase::NCItemSelectorBase( YWidget * parent, bool enforceSingleSelection )
    : YItemSelector( parent, enforceSingleSelection )
    , NCPadWidget( parent )
    , _prefSize( 50, 5 ) // width, height
    , _prefSizeDirty( true )
    , _selectorWidth( string( "|[x] |" ).size() )
{
    // yuiDebug() << endl;
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
    // yuiDebug() << endl;
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
    // yuiDebug() << endl;
}


NCPad * NCItemSelectorBase::CreatePad()
{
    wsze psze( defPadSze() );
    NCTablePad * npad = new NCTablePad( psze.H, psze.W, *this );
    npad->bkgd( listStyle().item.plain );
    npad->SetSepChar( ' ' );
    npad->AssertMinCols( 2 );
    npad->SetHotCol( 1 );

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
    int lineNo = findItemLine( item );

    if ( lineNo >= 0 )
	myPad()->ScrlLine( lineNo );
}


void NCItemSelectorBase::addItem( YItem * item )
{
    if ( item )
    {
	YItemSelector::addItem( item );
	createItemWidget( item );
    }
}


void NCItemSelectorBase::createItemWidget( YItem * item )
{
    if ( item )
    {
	vector<NCTableCol*> cells( 2U, 0 );

	_prefSizeDirty = true;
	int lineNo = myPad()->Lines();

	if ( lineNo > 0 )
	{
	    // Add a blank line as a separator from the previous item
	    //
	    // ...but only if there is any previous item that had a description.
	    // If there are only items without description, we don't need separator lines.

	    cells[0] = new NCTableCol( "",   NCTableCol::SEPARATOR );
	    cells[1] = new NCTableCol( "",   NCTableCol::SEPARATOR );
	    myPad()->Append( cells );
	    ++lineNo;
	}

	// yuiDebug() << "Adding new item " << item->label() << " at line #" << lineNo << endl;

	// Add the item label with "[ ]" or "( )" for selection

	cells[0] = createTagCell( item );
	cells[1] = new NCTableCol( item->label() );

	cells[1]->stripHotkey();

	NCTableLine * tableLine = new NCTableLine( cells );
	myPad()->Append( tableLine );

	if ( enforceSingleSelection() && item->selected() )
	    myPad()->ScrlLine( lineNo );


	// Add the item description (possible multi-line)

	vector<string> lines = descriptionLines( item );

	for ( const string & line: lines )
	{
	    cells[0] = new NCTableCol( "",   NCTableCol::PLAIN );
	    cells[1] = new NCTableCol( line, NCTableCol::PLAIN );
	    myPad()->Append( cells );
	}
    }
}


NCTableTag * NCItemSelectorBase::tagCell( int index ) const
{
    NCTableLine * tableLine = myPad()->ModifyLine( index );

    if ( ! tableLine )
	return 0;

    return dynamic_cast<NCTableTag *> ( tableLine->GetCol( 0 ) );
}


int NCItemSelectorBase::findItemLine( YItem * wantedItem ) const
{
    for ( int lineNo = 0; lineNo < (int) myPad()->Lines(); ++lineNo )
    {
        NCTableTag * tag = tagCell( lineNo );

        if ( tag && tag->origItem() == wantedItem )
            return lineNo;
    }

    return -1;
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


void NCItemSelectorBase::selectItem( YItem *item, bool selected )
{
    if ( item )
    {
	YItemSelector::selectItem( item, selected );

	NCTableTag * tag = (NCTableTag *) item->data();
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
    YItem * item = 0;

    while ( currentLine() < linesCount() - 1 )
    {
	YItem * item = currentItem();

	if ( item )
	    return item;

        // yuiDebug() << "Scrolling down" << endl;
	myPad()->ScrlDown();
    }

    if ( ! item ) // That was the last one
        item = scrollUpToPreviousItem();

    return item;
}


YItem *
NCItemSelectorBase::scrollUpToPreviousItem()
{
    while ( true )
    {
	YItem * item = currentItem();

	if ( item )
	    return item;

        if ( currentLine() == 0 )
            return 0;

        // yuiDebug() << "Scrolling up" << endl;
        myPad()->ScrlUp();
    }

    /**NOTREACHED**/
    return 0;
}


NCursesEvent
NCItemSelectorBase::wHandleInput( wint_t key )
{
    NCursesEvent ret;
    YItem * changedItem = 0;
    YItem * curItem     = currentItem();

    switch ( key )
    {
        case KEY_SPACE:
        case KEY_RETURN:    // Cycle item status and stay on this item

            if ( ! curItem )
                curItem = scrollUpToPreviousItem();

            if ( curItem )
            {
                cycleCurrentItemStatus();
                changedItem = curItem;
            }

            break;


        case '+':   // Select this item and go to the next item

            if ( ! curItem )
                curItem = scrollUpToPreviousItem();

            if ( curItem &&
                 curItem->status() != 1 &&
                 statusChangeAllowed( curItem->status(), 1 ) )
            {
                setItemStatus( curItem, 1 );
                changedItem = curItem;
            }

            if ( ! enforceSingleSelection() )
            {
                myPad()->ScrlDown();
                curItem = scrollDownToNextItem();
            }

            break;


        case '-':   // Deselect this item and go to the next item

            if ( ! curItem )
                curItem = scrollUpToPreviousItem();

            if ( curItem &&
                 curItem->status() > 0 &&
                 statusChangeAllowed( curItem->status(), 0 ) )
            {
                setItemStatus( curItem, 0 );
                changedItem = curItem;
            }

            if ( ! enforceSingleSelection() )
            {
                myPad()->ScrlDown();
                curItem = scrollDownToNextItem();
            }

            break;

        // Those keys have different meanings in this widget:
        // Scroll up and down by item, not by line.

        case KEY_UP:    // Scroll up one item
            myPad()->ScrlUp();
            scrollUpToPreviousItem();
            break;

        case KEY_DOWN:  // Scroll down one item
            myPad()->ScrlDown();
            scrollDownToNextItem();
            break;

        case KEY_END:   // Scroll to the last item
            myPad()->ScrlToLastLine();
            // We want to be on the last item, not on the last line
            scrollUpToPreviousItem();
            break;

        // The Home key (scroll to the first item) is handled in the base
        // class: Since the first is on the first line, so we can simply let
        // the base class scroll to the first line.


        // We would have liked to use KEY_SHIFT_UP and KEY_SHIFT_DOWN for
        // scrolling up or down by line rather by item, but unfortunately
        // NCurses does not support that at all; there are no predefined keys
        // for any of that (but oddly enough KEY_SLEFT and KEY_SRIGHT for
        // shifted arrow left or right), and there is no way to query the
        // status of the modifier keys.
        //
        // See also /usr/include/ncurses/ncurses.h .
        //
        // There are lots of articles on StackOverflow etc. about this topic,
        // but there is not a single portable solution; not even portable
        // between the various terminal emulators (xterm, KDE konsole,
        // gnome-terminal, xfce4-terminal) or the Linux console, let alone all
        // the various other terminal types out there.
        //
        // So we have to resort to different keys. Not sure how many users will
        // even realize that, but maybe some users actually try to use
        // 'vi'-like keys like 'j' or 'k'.

        case 'j':               // For 'vi' fans: Scroll down one line
            myPad()->ScrlDown();
            break;

        case 'k':               // For 'vi' fans: Scroll up one line
            myPad()->ScrlUp();
            break;

	case KEY_HOTKEY:

	    changedItem = curItem;
	    curItem = findItemWithHotkey( _hotKey );

	    if ( curItem )
	    {
		setCurrentItem( curItem );

		if ( ! changedItem )
		    changedItem = curItem;

		cycleCurrentItemStatus();
	    }

	    break;

        default:
            handleInput( key ); // Call base class input handler
            break;
    }

    if ( notify() && changedItem )
        ret = valueChangedNotify( changedItem );

    return ret;
}


void NCItemSelectorBase::activateItem( YItem * item )
{
    if ( notify() )
    {
        NCursesEvent event = valueChangedNotify( item );
        event.selection = (YMenuItem *) item;
        event.widget = this;

        YNCursesUI::ui()->sendEvent( event );
    }
}


void NCItemSelectorBase::shortcutChanged()
{
    // Any of the items might have its keyboard shortcut changed, but we don't
    // know which one. So let's simply re-create the widgets again.

    myPad()->ClearTable();

    for ( YItemIterator it = itemsBegin(); it != itemsEnd(); ++it )
    {
	createItemWidget( *it );
    }

    DrawPad();
}


bool NCItemSelectorBase::HasHotkey( int key )
{
    if ( ! findItemWithHotkey( key ) )
	return false;

    _hotKey = key;

    return true;
}


// ----------------------------------------------------------------------



NCItemSelector::NCItemSelector( YWidget * parent, bool enforceSingleSelection )
    : NCItemSelectorBase( parent, enforceSingleSelection )
{
    // yuiDebug() << endl;
}


NCItemSelector::~NCItemSelector()
{
    // yuiDebug() << endl;
}


NCTableTag *
NCItemSelector::createTagCell( YItem * item )
{
    NCTableTag * tag = new NCTableTag( item, item->selected(), enforceSingleSelection() );
    YUI_CHECK_NEW( tag );

    return tag;
}


NCursesEvent
NCItemSelector::valueChangedNotify( YItem * item )
{
    if ( enforceSingleSelection() && item && item->selected() )
        deselectAllItemsExcept( item );

    yuiDebug() << "Sending ValueChanged event for " << (YItemSelector* ) this << endl;

    return NCursesEvent::ValueChanged;
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


bool NCItemSelector::statusChangeAllowed( int fromStatus, int toStatus )
{
    if ( fromStatus == toStatus ) // No use setting to the same status as before
        return false;

    if ( toStatus < 0 || toStatus > 1 )
        return false;

    if ( enforceSingleSelection() )
        return toStatus == 1; // Allow only setting 0 -> 1
    else
        return true;
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


YItem* NCItemSelectorBase::findItemWithHotkey( int key ) const
{
    for ( YItemConstIterator it = itemsBegin(); it != itemsEnd(); it++ )
    {
	NClabel label = NCstring( (*it)->label() );
	label.stripHotkey();

	if ( label.hasHotkey() && tolower( label.hotkey() ) == tolower( key ) )
	    return *it;
    }

    return nullptr;
}
