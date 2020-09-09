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
    // yuiDebug() << std::endl;

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
    // yuiDebug() << std::endl;
}


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


std::vector<std::string>
NCTable::getHeader() const
{
    std::vector<std::string> header;

    header.assign( _header.size(), "" );

    for ( unsigned int i = 0; i < _header.size(); i++ )
    {
	header[ i ] =  _header[i].Str().substr( 1 ); // remove alignment
    }

    return header;
}


/**
 * Set the alignment for the specified table column (left, right, center).
 *
 * Create a temporary header consisting of single letter; setHeader will append
 * the rest.
 **/
void NCTable::setAlignment( int col, YAlignmentType alignment )
{
    std::string txt;

    switch ( alignment )
    {
	case YAlignUnchanged:
	    txt = 'L' ;
	    break;

	case YAlignBegin:
	    txt = 'L' ;
	    break;

	case YAlignCenter:
	    txt = 'C' ;
	    break;

	case YAlignEnd:
	    txt = 'R' ;
	    break;
    }

    _header[ col ] = NCstring( txt );
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
    YUI_CHECK_PTR( item );
    YTable::addItem( item );
    unsigned int itemCount;

    if ( !multiselect )
	itemCount = item->cellCount();
    else
	itemCount = item->cellCount() + 1;

    std::vector<NCTableCol*> Items( itemCount );
    unsigned int i = 0;

    if ( !multiselect )
    {
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
	Items[0] = new NCTableTag( yitem, yitem->selected() );
	i++;

	for ( YTableCellIterator it = item->cellsBegin();
	      it != item->cellsEnd();
	      ++it )
	{
	    Items[i] = new NCTableCol( NCstring(( *it )->label() ) );
	    i++;
	}
    }

    NCTableLine *newline = new NCTableLine( Items, item->index() );
    YUI_CHECK_PTR( newline );

    newline->setOrigItem( item );
    newline->SetState(state);
    myPad()->Append( newline );

    if ( item->selected() )
	setCurrentItem( item->index() ) ;

    if ( ! preventRedraw )
	DrawPad();
}


/**
 * Reimplemented here to speed up item insertion (and prevent inefficient
 * redrawing after every single addItem call)
 **/
void NCTable::addItems( const YItemCollection & itemCollection )
{

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

	if ( !multiselect )
	    selectCurrentItem();
    }

    DrawPad();
}


void NCTable::deleteAllItems()
{
    myPad()->ClearTable();
    DrawPad();
    YTable::deleteAllItems();
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
    const NCTableLine *cline = myPad()->GetLine( myPad()->CurPos().L );

    if ( cline )
	return cline->origItem();
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

	// yuiDebug() << item->label() << " is selected: " << (selected?"yes":"no") <<  endl;

	NCTableTag *tag =  static_cast<NCTableTag *>( line->GetCol( 0 ) );
	tag->SetSelected( selected );
    }

    // and redraw
    DrawPad();
}



/**
 * Mark currently highlighted table item as selected.
 *
 * Yes, it is really already highlighted, so no need to selectItem() and
 * setCurrentItem() here again. (bsc#493884)
 **/
void NCTable::selectCurrentItem()
{
    const NCTableLine *cline = myPad()->GetLine( myPad()->CurPos().L );

    if ( cline )
	YTable::selectItem( cline->origItem(), true );
}


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


int NCTable::preferredWidth()
{
    wsze sze = biglist ? myPad()->tableSize() + 2 : wGetDefsze();
    return sze.W;
}


int NCTable::preferredHeight()
{
    wsze sze = biglist ? myPad()->tableSize() + 2 : wGetDefsze();
    return sze.H;
}


void NCTable::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


void NCTable::setLabel( const std::string & nlabel )
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

			    if (!multiselect)
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

	if ( !multiselect )
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
