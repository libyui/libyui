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

using std::endl;

/*
 * Some remarks about single/multi selection:
 * A table in single selection mode has only one line/item selected which is equal to the
 * current item (means the highlighted line). Asking for `CurrentItem in YCP looks for
 * selectedItem() (see YCPPropertyHandler::tryGetSelectionWidgetValue).
 * In multi selection mode there can be several items selected (here is means checked/marked
 * with [x]) and the value is also got from selectedItem() when asking for `SelectedItems
 * (see YCPPropertyHandler::tryGetSelectionWidgetValue).
 * This means for multi selection mode: at the moment there isn't a possibility to get the
 * `CurrentItem. To get the current item (which line of the list is currently highlighted),
 * a virtual function currentItem() like available for the MultiSelectionBox has to be
 * provided to allow NCTable to specify the line number itself (getCurrentItem).
 *
 */
NCTable::NCTable( YWidget * parent, YTableHeader *tableHeader, bool multiSelection )
    : YTable( parent, tableHeader, multiSelection )
    , NCPadWidget( parent )
    , biglist( false )
    , multiselect( multiSelection )
{
    yuiDebug() << std::endl;

    InitPad();
    // !!! head is UTF8 encoded, thus should be std::vector<NCstring>
    if ( !multiselect )
    {
	_header.assign( tableHeader->columns(), NCstring( "" ) );
	for ( int col = 0; col < tableHeader->columns(); col++ )
	{
	    if ( hasColumn( col ) )
	    {
		// set alignment first
		setAlignment( col, alignment( col ) );
		// and then append header
		_header[ col ] +=  NCstring( tableHeader->header( col ) ) ;
	    }
	}
    }
    else
    {
	_header.assign( tableHeader->columns()+1, NCstring( "" ) );

	for ( int col = 1; col <= tableHeader->columns(); col++ )
	{
	    if ( hasColumn( col-1 ) )
	    {
		// set alignment first
		setAlignment( col, alignment( col-1 ) );
		// and then append header
		_header[ col ] +=  NCstring( tableHeader->header( col-1 ) ) ;
	    }
	}
    }

    hasHeadline = myPad()->SetHeadline( _header );

}




NCTable::~NCTable()
{
    yuiDebug() << std::endl;
}



// Change individual cell of a table line (to newtext)
//		      provided for backwards compatibility

void NCTable::cellChanged( int index, int colnum, const std::string & newtext )
{
    NCTableLine * cl = myPad()->ModifyLine( index );

    if ( !cl )
    {
	yuiWarning() << "No such line: " << wpos( index, colnum ) << newtext << std::endl;
    }
    else
    {
	NCTableCol * cc = cl->GetCol( colnum );

	if ( !cc )
	{
	    yuiWarning() << "No such colnum: " << wpos( index, colnum ) << newtext << std::endl;
	}
	else
	{
	    // use NCtring to enforce recoding from 'utf8'
	    cc->SetLabel( NCstring( newtext ) );
	    DrawPad();
	}
    }
}



// Change individual cell of a table line (to newtext)

void NCTable::cellChanged( const YTableCell *cell )
{

    cellChanged( cell->itemIndex(), cell->column(), cell->label() );

}



// Set all table headers all at once

void NCTable::setHeader( const std::vector<std::string>& head )
{
    _header.assign( head.size(), NCstring( "" ) );
    YTableHeader *th = new YTableHeader();

    for ( unsigned int i = 0; i < head.size(); i++ )
    {
	th->addColumn( head[ i ] );
	_header[ i ] +=  NCstring( head[ i ] ) ;
    }

    hasHeadline = myPad()->SetHeadline( _header );

    YTable::setTableHeader( th );
}

//
// Return table header as std::string std::vector (alignment removed)
//
std::vector<std::string> NCTable::getHeader( ) const
{
    std::vector<std::string> header;

    header.assign( _header.size(), "" );

    for ( unsigned int i = 0; i < _header.size(); i++ )
    {
	header[ i ] =  _header[i].Str().substr( 1 ); // remove alignment
    }

    return header;
}


// Set alignment of i-th table column (left, right, center).
// Create temp. header consisting of single letter;
// setHeader will append the rest.

void NCTable::setAlignment( int col, YAlignmentType al )
{
    std::string s;

    switch ( al )
    {
	case YAlignUnchanged:
	    s = 'L' ;
	    break;

	case YAlignBegin:
	    s = 'L' ;
	    break;

	case YAlignCenter:
	    s = 'C' ;
	    break;

	case YAlignEnd:
	    s = 'R' ;
	    break;
    }

    _header[ col ] = NCstring( s );
}

// Append  item (as pointed to by 'yitem')  in one-by-one
// fashion i.e. the whole table gets redrawn afterwards.
void NCTable::addItem( YItem *yitem)
{
    addItem(yitem, false); // add just this one
}

// Append item (as pointed to by 'yitem') to a table.
// This creates visual representation of new table line
// consisting of individual cells. Depending on the 2nd
// param, table is redrawn. If 'allAtOnce' is set to
// true, it is up to the caller to redraw the table.
void NCTable::addItem( YItem *yitem, bool allAtOnce )
{

    YTableItem *item = dynamic_cast<YTableItem *>( yitem );
    YUI_CHECK_PTR( item );
    YTable::addItem( item );
    unsigned int itemCount;

    if ( !multiselect )
	itemCount =  item->cellCount();
    else
	itemCount = item->cellCount()+1;

    std::vector<NCTableCol*> Items( itemCount );
    unsigned int i = 0;

    if ( !multiselect )
    {
	// Iterate over cells to create columns
	for ( YTableCellIterator it = item->cellsBegin();
	      it != item->cellsEnd();
	      ++it )
	{
	    Items[i] = new NCTableCol( NCstring(( *it )->label() ) );
	    i++;
	}
    }
    else
    {
	// Create the tag first
	Items[0] = new NCTableTag( yitem, yitem->selected() );
	i++;
	// and then iterate over cells
	for ( YTableCellIterator it = item->cellsBegin();
	      it != item->cellsEnd();
	      ++it )
	{
	    Items[i] = new NCTableCol( NCstring(( *it )->label() ) );
	    i++;
	}
    }

    //Insert @idx
    NCTableLine *newline = new NCTableLine( Items, item->index() );

    YUI_CHECK_PTR( newline );

    newline->setOrigItem( item );

    myPad()->Append( newline );

    if ( item->selected() )
    {
	setCurrentItem( item->index() ) ;
    }

    //in one-by-one mode, redraw the table (otherwise, leave it
    //up to the caller)
    if (!allAtOnce)
    {
	DrawPad();
    }
}

// reimplemented here to speed up item insertion
// (and prevent inefficient redrawing after every single addItem
// call)
void NCTable::addItems( const YItemCollection & itemCollection )
{

    for ( YItemConstIterator it = itemCollection.begin();
	  it != itemCollection.end();
	  ++it )
    {
	addItem( *it, true);
    }

    if ( !keepSorting() )
    {
	myPad()->sort();
	selectCurrentItem();
    }

    DrawPad();
}

// Clear the table (in terms of YTable and visually)

void NCTable::deleteAllItems()
{
    myPad()->ClearTable();
    DrawPad();
    YTable::deleteAllItems();
}



// Return index of currently selected table item

int NCTable::getCurrentItem() const
{
    if ( !myPad()->Lines() )
	return -1;

    return keepSorting() ? myPad()->GetLine( myPad()->CurPos().L )->getIndex()
	   : myPad()->CurPos().L;
}



// Return origin pointer of currently selected table item

YItem * NCTable::getCurrentItemPointer()
{
    const NCTableLine *cline = myPad()->GetLine( myPad()->CurPos().L );

    if ( cline )
	return cline->origItem();
    else
	return 0;
}



// Highlight item at 'index'

void NCTable::setCurrentItem( int index )
{
    myPad()->ScrlLine( index );
}



// Mark table item (as pointed to by 'yitem') as selected

void NCTable::selectItem( YItem *yitem, bool selected )
{
    if ( ! yitem )
	return;

    YTableItem *item = dynamic_cast<YTableItem *>( yitem );
    YUI_CHECK_PTR( item );

    NCTableLine *line = ( NCTableLine * )item->data();
    YUI_CHECK_PTR( line );

    const NCTableLine *current_line = myPad()->GetLine( myPad()->CurPos().L );
    YUI_CHECK_PTR( current_line );

    if ( !multiselect )
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

	yuiDebug() << item->label() << " is selected: " << (selected?"yes":"no") <<  endl;

	NCTableTag *tag =  static_cast<NCTableTag *>( line->GetCol( 0 ) );
	tag->SetSelected( selected );
    }

    // and redraw
    DrawPad();
}



// Mark currently highlighted table item as selected
// Yeah, it is really already highlighted, so no need to
// selectItem() and setCurrentItem() here again - #493884

void NCTable::selectCurrentItem()
{
    const NCTableLine *cline = myPad()->GetLine( myPad()->CurPos().L );

    if ( cline )
	YTable::selectItem( cline->origItem(), true );
}



// Mark all items as deselected

void NCTable::deselectAllItems()
{
    if ( !multiselect )
    {
        setCurrentItem( -1 );
        YTable::deselectAllItems();
    }
    else
    {
        YItemCollection itemCollection = YTable::selectedItems();
        for ( YItemConstIterator it = itemCollection.begin();
              it != itemCollection.end(); ++it )
        {
            selectItem( *it, false );   // YTable::selectItem(item,false)
        }
    }

    DrawPad();
}



// return preferred size

int NCTable::preferredWidth()
{
    wsze sze = ( biglist ) ? myPad()->tableSize() + 2 : wGetDefsze();
    return sze.W;
}



// return preferred size

int NCTable::preferredHeight()
{
    wsze sze = ( biglist ) ? myPad()->tableSize() + 2 : wGetDefsze();
    return sze.H;
}



// Set new size of the widget

void NCTable::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}




void NCTable::setLabel( const std::string & nlabel )
{
    // not implemented: YTable::setLabel( nlabel );
    NCPadWidget::setLabel( NCstring( nlabel ) );
}



// Set widget state (enabled vs. disabled)

void NCTable::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YTable::setEnabled( do_bv );
}




bool NCTable::setItemByKey( int key )
{
    return myPad()->setItemByKey( key );
}





// Create new NCTablePad, set its background
NCPad * NCTable::CreatePad()
{
    wsze    psze( defPadSze() );
    NCPad * npad = new NCTablePad( psze.H, psze.W, *this );
    npad->bkgd( listStyle().item.plain );

    return npad;
}



// Handle 'special' keys i.e those not handled by parent NCPad class
// (space, return). Set items to selected, if appropriate.

NCursesEvent NCTable::wHandleInput( wint_t key )
{
    NCursesEvent ret;
    int citem  = getCurrentItem();
    bool sendEvent = false;

    if ( ! handleInput( key ) )
    {
	switch ( key )
	{
	    case CTRL( 'o' ):
		{
		    if ( ! keepSorting() )
		    {
			// get the column - show popup in upper left corner
			wpos at( ScreenPos() + wpos( 2, 1 ) );

			YItemCollection ic;
			ic.reserve( _header.size() );
			unsigned int i = 0;

			for ( std::vector<NCstring>::const_iterator it = _header.begin();
			      it != _header.end() ; it++, i++ )
			{
			    // strip the align mark
			    std::string col = ( *it ).Str();
			    col.erase( 0, 1 );

			    YMenuItem *item = new YMenuItem( col ) ;
			    //need to set index explicitly, MenuItem inherits from TreeItem
			    //and these don't have indexes set
			    item->setIndex( i );
			    ic.push_back( item );
			}

			NCPopupMenu *dialog = new NCPopupMenu( at, ic.begin(), ic.end() );

			int column = dialog->post();

			if ( column != -1 )
			{
			    myPad()->setOrder( column, true );	//enable sorting in reverse order
			    selectCurrentItem();
			}

			//remove the popup
			YDialog::deleteTopmostDialog();

			return NCursesEvent::none;
		    }
		}

	    case KEY_RETURN:
                sendEvent = true;
	    case KEY_SPACE:
		if ( !multiselect )
		{
		    if ( notify() && citem != -1 )
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


    if (  citem != getCurrentItem() )
    {
	if ( notify() && immediateMode() )
	    ret = NCursesEvent::SelectionChanged;

	if ( !multiselect )
	    selectCurrentItem();
    }

    return ret;
}

/**
 * Toggle item from selected -> deselected and vice versa
 **/
void NCTable::toggleCurrentItem()
{
    YTableItem *it =  dynamic_cast<YTableItem *>( getCurrentItemPointer() );
    if ( it )
    {
	selectItem( it, !( it->selected() ) );
    }
}
