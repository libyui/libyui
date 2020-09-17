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

   File:       NCTable.cc

   Author:     Michael Andres <ma@suse.de>

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
{
    // yuiDebug() << endl;

    InitPad();
    rebuildHeaderLine();
}


NCTable::~NCTable()
{
    // yuiDebug() << endl;
}


void NCTable::rebuildHeaderLine()
{
    _prefixCols = 0;

    if ( _multiSelect || _nestedItems )
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


void NCTable::cellChanged( int index, int colnum, const string & newtext )
{
    NCTableLine * currentLine = myPad()->ModifyLine( index );

    if ( !currentLine )
    {
	yuiWarning() << "No such line: " << wpos( index, colnum ) << newtext << endl;
    }
    else
    {
	NCTableCol * currentCol = currentLine->GetCol( colnum );

	if ( !currentCol )
	{
	    yuiWarning() << "No such colnum: " << wpos( index, colnum ) << newtext << endl;
	}
	else
	{
	    // use NCtring to enforce recoding from UTF-8
	    currentCol->SetLabel( NCstring( newtext ) );
	    DrawPad();
	}
    }
}


void NCTable::cellChanged( const YTableCell *cell )
{
    int id = cell->itemIndex(); // index at insertion time, before sorting
    int index = myPad()->findIndexById(id); // convert to index after sorting

    if (index == -1)
    {
	// should not happen
	return;
    }

    cellChanged( index, cell->column(), cell->label() );
}


void NCTable::setHeader( const vector<string> & headers )
{
    YTableHeader *tableHeader = new YTableHeader();

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


void NCTable::addItem( YItem *            yitem,
                       NCTableLine::STATE state )
{
    addItem( yitem,
             false,     // preventRedraw
             state );
}


void NCTable::addItem( YItem *            yitem,
                       bool               preventRedraw,
                       NCTableLine::STATE state )
{

    YTableItem *item = dynamic_cast<YTableItem *>( yitem );
    YUI_CHECK_NEW( item );
    YTable::addItem( item );

    vector<NCTableCol*> cells;

    if ( _multiSelect )
        cells.push_back( new NCTableTag( yitem, yitem->selected() ) );

    for ( YTableCellIterator it = item->cellsBegin(); it != item->cellsEnd(); ++it )
    {
        cells.push_back( new NCTableCol( NCstring(( *it )->label() ) ) );
    }

    NCTableLine *line = new NCTableLine( cells, item->index(), _nestedItems );
    YUI_CHECK_NEW( line );

    line->setOrigItem( item );
    line->SetState( state );
    myPad()->Append( line );

    if ( item->selected() )
	setCurrentItem( item->index() ) ;

    // Recurse over children (if there are any)

    for ( YItemIterator it = item->childrenBegin(); it != item->childrenEnd(); ++it )
    {
        _nestedItems = true;

        // TO DO: Specify parent item
        addItem( *it, preventRedraw, state );
    }

    if ( ! preventRedraw )
	DrawPad();
}


/**
 * Reimplemented here to speed up item insertion (and prevent inefficient
 * redrawing after every single addItem call)
 **/
void NCTable::addItems( const YItemCollection & itemCollection )
{
    _nestedItems = hasNestedItems( itemCollection );

    for ( YItemConstIterator it = itemCollection.begin();
	  it != itemCollection.end();
	  ++it )
    {
	addItem( *it,
                 true,  // preventRedraw
                 NCTableLine::S_NORMAL );
    }

    if ( !keepSorting() )
    {
	myPad()->sort();

	if ( !_multiSelect )
	    selectCurrentItem();
    }

    DrawPad();
}


bool NCTable::hasNestedItems( const YItemCollection & itemCollection ) const
{
    for ( YItemConstIterator it = itemCollection.begin();
          it != itemCollection.end();
          ++it )
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
    _nestedItems = false;
}


int NCTable::getCurrentItem() const
{
    if ( !myPad()->Lines() )
	return -1;

    // The intent of this condition is to return the original index, before
    // sorting. But the condition was accidentally inverted in 2007 and now it
    // always returns the index after sorting.
    // Should we fix it? Depends on whether the current users rely on the
    // current behavior.
    return keepSorting() ? myPad()->GetLine( myPad()->CurPos().L )->getIndex()
	   : myPad()->CurPos().L;
}


YItem * NCTable::getCurrentItemPointer()
{
    const NCTableLine *currentLine = myPad()->GetLine( myPad()->CurPos().L );

    if ( currentLine )
	return currentLine->origItem();
    else
	return 0;
}


void NCTable::setCurrentItem( int index )
{
    myPad()->ScrlLine( index );
}


void NCTable::selectItem( YItem *yitem, bool selected )
{
    if ( ! yitem )
	return;

    YTableItem *item = dynamic_cast<YTableItem *>( yitem );
    YUI_CHECK_NEW( item );

    NCTableLine *line = ( NCTableLine * )item->data();
    YUI_CHECK_NEW( line );

    const NCTableLine *current_line = myPad()->GetLine( myPad()->CurPos().L );
    YUI_CHECK_NEW( current_line );

    if ( !_multiSelect )
    {
	if ( !selected && ( line == current_line ) )
	{
	    deselectAllItems();
	}
	else
	{
	    // first highlight only, then select
	    setCurrentItem( line->getIndex() );
	    YTable::selectItem( item, selected );
	}
    }
    else
    {
	YTable::selectItem( item, selected );

	// yuiDebug() << item->label() << " is selected: " << (selected?"yes":"no") <<  endl;

	NCTableTag *tag =  static_cast<NCTableTag *>( line->GetCol( 0 ) );
	tag->SetSelected( selected );
    }

    // and redraw
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
    const NCTableLine *currentLine = myPad()->GetLine( myPad()->CurPos().L );

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

        for ( YItemConstIterator it = itemCollection.begin();
              it != itemCollection.end();
              ++it )
        {
            selectItem( *it, false );   // YTable::selectItem(item,false)
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
 * Handle 'special' keys i.e those not handled by parent NCPad class
 * (space, return). Set items to selected if appropriate.
 **/
NCursesEvent NCTable::wHandleInput( wint_t key )
{
    NCursesEvent ret;
    int currentIndex  = getCurrentItem();
    bool sendEvent = false;

    if ( ! handleInput( key ) )
    {
	switch ( key )
	{
	    case CTRL( 'o' ):
                if ( ! keepSorting() )
                {
                    interactiveSort();
                    return NCursesEvent::none;
                }
                break;

	    case KEY_RETURN:
                sendEvent = true;
	    case KEY_SPACE:
		if ( !_multiSelect )
		{
		    if ( notify() && currentIndex != -1 )
			return NCursesEvent::Activated;
		}
		else
		{
		    toggleCurrentItem();

                    // send ValueChanged on Return (like done for NCTree multiSelection)

                    if ( notify() && sendEvent )
                    {
                        return NCursesEvent::ValueChanged;
                    }
		}
		break;

	}
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

        // Get the column - show popup in upper left corner
        wpos pos( ScreenPos() + wpos( 2, 1 ) );

        NCPopupMenu *dialog = new NCPopupMenu( pos, menuItems.begin(), menuItems.end() );
        int column = dialog->post();

        // close the popup
        YDialog::deleteTopmostDialog();

        if ( column != -1 && hasColumn( column ) )
        {
            yuiDebug() << "Manually sorting by column #"
                       << column << ": " << header( column )
                       << endl;

            myPad()->setOrder( column, true ); // enable sorting in reverse order

            if ( !_multiSelect )
                selectCurrentItem();
        }
    }
}
