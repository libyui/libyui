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

   File:       NCTable.cc

   Authors:    Michael Andres <ma@suse.de>
               Stefan Hundhammer <shundhammer@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCTable.h"
#include "NCPopupMenu.h"

#include <yui/YMenuButton.h>
#include <yui/YTypes.h>

using std::string;
using std::vector;
using std::endl;


/*
 * Some remarks about single/multi selection:
 *
 * A table in single selection mode has only one line/item selected which is
 * equal to the current item (means the highlighted line). Querying CurrentItem
 * in YCP looks for selectedItem()
 * (see YCPPropertyHandler::tryGetSelectionWidgetValue).
 *
 * In multi selection mode there can be several items selected (here is means
 * checked/marked with [x]) and the value is also got from selectedItem() when
 * asking for `SelectedItems
 * (see YCPPropertyHandler::tryGetSelectionWidgetValue).
 *
 * This means for multi selection mode: at the moment there isn't a possibility
 * to get the CurrentItem. To get the current item (which line of the list is
 * currently highlighted), a virtual function currentItem() like available for
 * the MultiSelectionBox has to be provided to allow NCTable to specify the
 * line number itself (getCurrentItem).
 */
NCTable::NCTable( YWidget *      parent,
                  YTableHeader * tableHeader,
                  bool           multiSelection )
    : YTable( parent, tableHeader, multiSelection )
    , NCPadWidget( parent )
    , _prefixCols( 0 )
    , _nestedItems( false )
    , _bigList( false )
    , _multiSelect( multiSelection )
    , _lastSortCol( 0 )
    , _sortReverse( false )
    , _sortStrategy( new NCTableSortDefault() )
{
    // yuiDebug() << endl;

    InitPad();
    rebuildHeaderLine();
}


NCTable::~NCTable()
{
    if ( _sortStrategy )
        delete _sortStrategy;
}


void NCTable::rebuildHeaderLine()
{
    _prefixCols = 0;

    if ( _multiSelect )
        ++_prefixCols;

    vector<NCstring> headers;
    headers.resize( _prefixCols + columns() );

    for ( int i = 0; i < columns(); i++ )
    {
        int col = i + _prefixCols;

        if ( hasColumn( i ) )
        {
            NCstring hdr( alignmentStr( i ) );
            hdr += header( i );
            // yuiDebug() << "hdr[" << col << "]: \"" << hdr << "\"" << endl;
            headers[ col ] = hdr;
        }
    }

    hasHeadline = myPad()->SetHeadline( headers );
}


NCstring NCTable::alignmentStr( int col )
{
    switch ( alignment( col ) )
    {
	case YAlignUnchanged:   return "L";
	case YAlignBegin:       return "L";
	case YAlignCenter:      return "C";
	case YAlignEnd:         return "R";

        // No 'default' branch: Let the compiler complain if there is an unhandled enum value.
    }

    return "L";
}


void NCTable::setCell( int index, int col, const string & newtext )
{
    NCTableLine * currentLine = myPad()->ModifyLine( index );

    if ( !currentLine )
    {
	yuiWarning() << "No such line: " << wpos( index, col ) << newtext << endl;
    }
    else
    {
	NCTableCol * currentCol = currentLine->GetCol( col );

	if ( !currentCol )
	{
	    yuiWarning() << "No such col: " << wpos( index, col ) << newtext << endl;
	}
	else
	{
	    // use NCtring to enforce recoding from UTF-8
	    currentCol->SetLabel( NCstring( newtext ) );
	    DrawPad();
	}
    }
}


void NCTable::cellChanged( const YTableCell * changedCell )
{
    YUI_CHECK_PTR( changedCell );

    YTableItem * ytableItem = changedCell->parent();
    YUI_CHECK_PTR( ytableItem );

    NCTableLine * tableLine = (NCTableLine *) ytableItem->data();
    YUI_CHECK_PTR( tableLine );

    NCTableCol * tableCol = tableLine->GetCol( changedCell->column() );

    if ( tableCol )
    {
        tableCol->SetLabel( changedCell->label() );
        DrawPad();
    }
    else
    {
        yuiError() << "No column #" << changedCell->column()
                   << " in item " << ytableItem
                   << endl;
    }
}


void NCTable::setHeader( const vector<string> & headers )
{
    YTableHeader * tableHeader = new YTableHeader();

    for ( unsigned i = 0; i < headers.size(); i++ )
    {
	tableHeader->addColumn( headers[ i ] );
    }

    YTable::setTableHeader( tableHeader );
    rebuildHeaderLine();
}


vector<string>
NCTable::getHeader() const
{
    vector<string> headers;
    headers.resize( columns() );

    for ( int col = 0; col < columns(); col++ )
    {
        headers[ col ] = header( col );
    }

    return headers;
}


void NCTable::addItems( const YItemCollection & itemCollection )
{
    myPad()->ClearTable();
    YTable::addItems( itemCollection );

    if ( keepSorting() )
    {
        rebuildPadLines();
    }
    else
    {
        sortItems( _lastSortCol, _sortReverse );
        // this includes rebuildPadLInes()
    }

    if ( !_multiSelect )
        selectCurrentItem();

    DrawPad();
}


void NCTable::addItem( YItem *            yitem,
                       NCTableLine::STATE state )
{
    if ( ! yitem->parent() )            // Only for toplevel items:
        YTable::addItem( yitem );       // Notify the YTable base class

    addPadLine( 0,      // parentLine
                yitem,
                false,  // preventRedraw
                state );
}


void NCTable::addItem( YItem *            yitem,
                       bool               preventRedraw,
                       NCTableLine::STATE state )
{
    if ( ! yitem->parent() )            // Only for toplevel items:
        YTable::addItem( yitem );       // Notify the YTable base class

    addPadLine( 0,      // parentLine
                yitem,
                preventRedraw,
                state );
}


void NCTable::addPadLine( NCTableLine *      parentLine,
                          YItem *            yitem,
                          bool               preventRedraw,
                          NCTableLine::STATE state )
{
    YTableItem *item = dynamic_cast<YTableItem *>( yitem );
    YUI_CHECK_PTR( item );

    // Ideally, _nestedItems should be updated by iterating over ALL items
    // before the first NCTableLine is created (i.e. before the first
    // addPadLine() call) so they all get the proper prefix placeholder for
    // reserving some screen space for the tree line graphics.
    //
    // This additional check is just a second line of defence.

    if ( parentLine || item->hasChildren() )
        _nestedItems = true;

    vector<NCTableCol*> cells;

    if ( _multiSelect )
    {
        // Add a table tag to hold the "[ ]" / "[x]" marker.
        cells.push_back( new NCTableTag( yitem, yitem->selected() ) );
    }

    // Add all the cells
    for ( YTableCellIterator it = item->cellsBegin(); it != item->cellsEnd(); ++it )
        cells.push_back( new NCTableCol( NCstring(( *it )->label() ) ) );

    int index = myPad()->Lines();
    item->setIndex( index );

    // yuiMilestone() << "Adding pad line for " << item << " index: " << item->index() << endl;


    // Create the line itself
    NCTableLine *line = new NCTableLine( parentLine,
                                         item,
                                         cells,
                                         index,
                                         _nestedItems,
                                         state );
    YUI_CHECK_NEW( line );
    myPad()->Append( line );

    if ( item->selected() )
	setCurrentItem( item->index() ) ;

    // Recurse over children (if there are any)

    for ( YItemIterator it = item->childrenBegin(); it != item->childrenEnd(); ++it )
    {
        addPadLine( line, *it, preventRedraw, state );
    }

    if ( ! preventRedraw )
	DrawPad();
}


void NCTable::rebuildPadLines()
{
    myPad()->ClearTable();
    _nestedItems = hasNestedItems( itemsBegin(), itemsEnd() );

    for ( YItemConstIterator it = itemsBegin(); it != itemsEnd(); ++it )
    {
        addPadLine( 0,      // parentLine
                    *it,
                    true ); // preventRedraw
    }
}


bool NCTable::hasNestedItems( const YItemCollection & itemCollection ) const
{
    return hasNestedItems( itemCollection.begin(), itemCollection.end() );
}


bool NCTable::hasNestedItems( YItemConstIterator begin,
                              YItemConstIterator end ) const
{
    for ( YItemConstIterator it = begin; it != end; ++it )
    {
        if ( (*it)->hasChildren() )
            return true;
    }

    return false;
}


void NCTable::deleteAllItems()
{
    myPad()->ClearTable();
    YTable::deleteAllItems();
    DrawPad();

    _nestedItems   = false;
    _lastSortCol   = 0;
    _sortReverse   = false;
}


int NCTable::getCurrentItem() const
{
    if ( myPad()->empty() )
	return -1;

    // The intent of this condition is to return the original index, before
    // sorting. But the condition was accidentally inverted in 2007 and now it
    // always returns the index after sorting.
    // Should we fix it? Depends on whether the current users rely on the
    // current behavior.

    return keepSorting() ? getCurrentIndex() : myPad()->CurPos().L;
}


YItem * NCTable::getCurrentItemPointer()
{
    const NCTableLine * currentLine = myPad()->GetCurrentLine();

    if ( currentLine )
	return currentLine->origItem();
    else
	return 0;
}


int NCTable::getCurrentIndex() const
{
    const NCTableLine * currentLine = myPad()->GetCurrentLine();

    return currentLine ? currentLine->index() : -1;
}


void NCTable::scrollToFirstItem()
{
    if ( myPad()->empty() )
        myPad()->ScrlLine( 0 );
}


void NCTable::setCurrentItem( int index )
{
    if ( myPad()->empty() )
        return;

    int pos = myPad()->findIndex( index );

    if ( pos >= 0 && (unsigned) pos < myPad()->visibleLines() )
        myPad()->ScrlLine( index );
    else
        yuiWarning() << "Can't find line with index " << index << endl;
}


void NCTable::selectItem( YItem *yitem, bool selected )
{
    if ( ! yitem )
	return;

    YTableItem *item = dynamic_cast<YTableItem *>( yitem );
    YUI_CHECK_PTR( item );

    NCTableLine *line = (NCTableLine *) item->data();
    YUI_CHECK_PTR( line );

    const NCTableLine *current_line = myPad()->GetLine( myPad()->CurPos().L );
    YUI_CHECK_PTR( current_line );

    if ( !_multiSelect )
    {
	if ( !selected && ( line == current_line ) )
	{
	    deselectAllItems();
	}
	else
	{
	    // first highlight only, then select
	    setCurrentItem( line->index() );
	    YTable::selectItem( item, selected );
	}
    }
    else // multiSelect
    {
	YTable::selectItem( item, selected );

	// yuiDebug() << item->label() << " is selected: " << std::boolalpha << selected <<  endl;

        // The NCTableTag holds the "[ ]" / "[x]" selection marker
	NCTableTag * tagCell =  line->tagCell();

        if ( tagCell )
            tagCell->SetSelected( selected );
    }

    DrawPad();
}



/**
 * Mark the currently highlighted table item as selected.
 *
 * Yes, it is really already highlighted, so no need to selectItem() and
 * setCurrentItem() here again. (bsc#493884)
 **/
void NCTable::selectCurrentItem()
{
    const NCTableLine * currentLine = myPad()->GetCurrentLine();

    if ( currentLine )
	YTable::selectItem( currentLine->origItem(), true );
}


void NCTable::deselectAllItems()
{
    if ( !_multiSelect )
    {
        setCurrentItem( -1 );
        YTable::deselectAllItems();
    }
    else
    {
        YItemCollection itemCollection = YTable::selectedItems();
        // This will return nested selected items as well

        for ( YItemConstIterator it = itemCollection.begin();
              it != itemCollection.end();
              ++it )
        {
            // Clear the item's internal selected status flag
            // and update the "[x]" marker on the screen to "[ ]"
            // in the corresponding NCTableTag

            selectItem( *it, false );
        }
    }

    DrawPad();
}


int NCTable::preferredWidth()
{
    wsze sze = _bigList ? myPad()->tableSize() + 2 : wGetDefsze();
    return sze.W;
}


int NCTable::preferredHeight()
{
    wsze sze = _bigList ? myPad()->tableSize() + 2 : wGetDefsze();
    return sze.H;
}


void NCTable::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


void NCTable::setLabel( const string & nlabel )
{
    // not implemented: YTable::setLabel( nlabel );
    NCPadWidget::setLabel( NCstring( nlabel ) );
}


void NCTable::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YTable::setEnabled( do_bv );
}


bool NCTable::setItemByKey( int key )
{
    return myPad()->setItemByKey( key );
}


NCPad * NCTable::CreatePad()
{
    wsze    psze( defPadSze() );
    NCPad * npad = new NCTablePad( psze.H, psze.W, *this );
    npad->bkgd( listStyle().item.plain );

    return npad;
}


/**
 * NCurses widget keyboard handler.
 *
 * This is the starting point for handling key events. From here, key events
 * are propagated to the pad and to the items.
 **/
NCursesEvent NCTable::wHandleInput( wint_t key )
{
    NCursesEvent ret  = NCursesEvent::none;
    bool sendEvent    = false;
    int  currentIndex = getCurrentItem();

    // Call the pad's input handler via NCPadWidget::handleInput()
    // which calls its pad class's input handler
    // which may call the current item's input handler.
    //
    // Notice that most keys are handled on the level of the pad or the item,
    // not here. See
    //
    // - NCTablePad::handleInput()
    // - NCTablePadBase::handleInput()
    // - NCTableLine::handleInput()

    bool handled = handleInput( key ); // NCTablePad::handleInput()

    switch ( key )
    {
        case CTRL( 'o' ):       // Table sorting (Ordering)
            if ( ! handled )
            {
                if ( ! keepSorting() )
                {
                    interactiveSort();
                    return NCursesEvent::none;
                }
            }
            break;


        // Even if the event was already handled:
        // Take care about sending UI events to the caller.

        case KEY_RETURN:

            sendEvent = true;

            if ( _multiSelect)
            {
                toggleCurrentItem();

                // Send ValueChanged on Return (like done for NCTree multiSelection)

                if ( notify() && sendEvent )
                    return NCursesEvent::ValueChanged;
            }
            // FALLTHRU

        case KEY_SPACE:

            if ( !_multiSelect )
            {
                if ( notify() && currentIndex != -1 )
                    return NCursesEvent::Activated;
            }
            break;
    }

    if (  currentIndex != getCurrentItem() )
    {
	if ( notify() && immediateMode() )
	    ret = NCursesEvent::SelectionChanged;

	if ( !_multiSelect )
	    selectCurrentItem();
    }

    return ret;
}


void NCTable::toggleCurrentItem()
{
    YTableItem * item =  dynamic_cast<YTableItem *>( getCurrentItemPointer() );

    if ( item )
	selectItem( item, !( item->selected() ) );
}


void NCTable::interactiveSort()
{
    //
    // Collect the non-empty column headers
    //

    YItemCollection menuItems;
    menuItems.reserve( columns() );

    for ( int col = 0; col < columns(); col++ )
    {
        string hdr = header( col );

        if ( ! hdr.empty() )
        {
            YMenuItem *item = new YMenuItem( header( col ) ) ;

            // need to set the index explicitly, YMenuItem inherits from YTreeItem
            // and these don't have indexes set
            item->setIndex( col );
            menuItems.push_back( item );
        }
    }

    if ( ! menuItems.empty() )
    {
        //
        // Post a popup with the column headers
        //

        // Get the column; show the popup in the table's upper left corner
        wpos pos( ScreenPos() + wpos( 2, 1 ) );

        NCPopupMenu *dialog = new NCPopupMenu( pos, menuItems.begin(), menuItems.end() );
        int sortCol = dialog->post();

        // close the popup
        YDialog::deleteTopmostDialog();

        if ( sortCol != -1 && hasColumn( sortCol ) )
        {
            //
            // Do the sorting
            //

            yuiDebug() << "Manually sorting by column #"
                       << sortCol << ": " << header( sortCol )
                       << endl;

            _sortReverse = sortCol == _lastSortCol ?
                ! _sortReverse : false;

            sortItems( sortCol, _sortReverse );

            if ( !_multiSelect )
                selectCurrentItem();

            DrawPad();
        }
    }
}


void NCTable::sortItems( int sortCol, bool reverse )
{
    myPad()->ClearTable();

    // Sort the YItems.
    //
    // This may feel a little weird since those YItems are owned by the
    // YSelectionWidget parent class. But we are only changing their sort
    // order, not invalidating any item pointers; and the internal sort order
    // is not anything that any calling application code may rely on.
    //
    // Since the NCTable now supports nested items, we can no longer simply
    // sort the NCTableLines to keep this whole sorting localized: In the pad,
    // they are just a flat list, and the hierarchy is not that easy to find
    // out.  But we need the hierarchy to sort each tree level separately in
    // each branch.
    //
    // It is much simpler and less error-prone to just clear the pad (and thus
    // get rid of any existing NCTableLines), sort the YItems and rebuild all
    // the NCTableLines from the newly sorted YItems.

    _sortStrategy->setSortCol( sortCol );
    _sortStrategy->setReverse( reverse );
    _lastSortCol = sortCol;

    sortYItems( itemsBegin(), itemsEnd() );

    rebuildPadLines();
}


void NCTable::sortYItems( YItemIterator begin,
                          YItemIterator end )
{
    // Sort the children first as long as the iterators are
    // guaranteed to be valid

    for ( YItemIterator it = begin; it != end; ++it )
    {
        if ( (*it)->hasChildren() )
            sortYItems( (*it)->childrenBegin(), (*it)->childrenEnd() );
    }

    // Sort this level. This may make the iterators invalid.
    _sortStrategy->sort( begin, end );
}


void NCTable::setSortStrategy( NCTableSortStrategyBase * newStrategy )
{
    if ( _sortStrategy )
        delete _sortStrategy;

    _sortStrategy = newStrategy;
}

