/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCTable.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCTable.h"
#include "NCPopupMenu.h"
#include <yui/YMenuButton.h>
#include "YTypes.h"


NCTable::NCTable( YWidget * parent, YTableHeader *tableHeader, bool multiSelection )
    : YTable( parent, tableHeader, multiSelection )
    , NCPadWidget( parent )
    , biglist( false )
    , multiselect( multiSelection )
{
    yuiDebug() << endl;

    InitPad();
    // !!! head is UTF8 encoded, thus should be vector<NCstring>
    if ( !multiselect )
    {
	_header.assign( tableHeader->columns(), NCstring( "" ) );
	for ( int col = 0; col < tableHeader->columns(); col++ )
	{
	    if ( hasColumn( col ) )
	    {
		// set aligmant first
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
		// set aligmant first
		setAlignment( col, alignment( col ) );
		// and then append header
		_header[ col ] +=  NCstring( tableHeader->header( col-1 ) ) ;
	    }
	}
    }

    hasHeadline = myPad()->SetHeadline( _header );

}




NCTable::~NCTable()
{
    yuiDebug() << endl;
}



// Change individual cell of a table line (to newtext)
//		      provided for backwards compatibility

void NCTable::cellChanged( int index, int colnum, const string & newtext )
{
    NCTableLine * cl = myPad()->ModifyLine( index );

    if ( !cl )
    {
	yuiWarning() << "No such line: " << wpos( index, colnum ) << newtext << endl;
    }
    else
    {
	NCTableCol * cc = cl->GetCol( colnum );

	if ( !cc )
	{
	    yuiWarning() << "No such colnum: " << wpos( index, colnum ) << newtext << endl;
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

void NCTable::setHeader( vector<string> head )
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



// Set alignment of i-th table column (left, right, center).
// Create temp. header consisting of single letter;
// setHeader will append the rest.

void NCTable::setAlignment( int col, YAlignmentType al )
{
    string s;

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



// Append item (as pointed to by 'yitem') to a table
// (create new table line consisting of individual cells)

void NCTable::addItem( YItem *yitem )
{

    YTableItem *item = dynamic_cast<YTableItem *>( yitem );
    YUI_CHECK_PTR( item );
    YTable::addItem( item );
    unsigned int itemCount;
    
    if ( !multiselect )
	itemCount =  item->cellCount();
    else
	itemCount = item->cellCount()+1;
    
    vector<NCTableCol*> Items( itemCount );
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

    //Do not forget to redraw whole stuff at the end
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

int NCTable::getCurrentItem()
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
	    //first highlight only, then select
	    setCurrentItem( line->getIndex() );
	    YTable::selectItem( item, selected );
	}
    }
    else
    {
	setCurrentItem( line->getIndex() );
	YTable::selectItem( item, selected );

	yuiMilestone() << item->label() << " is selected: " << (selected?"yes":"no") <<  endl;

	NCTableTag *tag =  static_cast<const NCTableTag *>( line->GetCol( 0 ) );
	tag->SetSelected( selected );
    }

    //and redraw
    DrawPad();
}



// Mark currently highlighted table item as selected

void NCTable::selectCurrentItem()
{
    const NCTableLine *cline = myPad()->GetLine( myPad()->CurPos().L );

    if ( cline )
	selectItem( cline->origItem(), true );
}



// Mark all items as deselected

void NCTable::deselectAllItems()
{
    setCurrentItem( -1 );
    YTable::deselectAllItems();
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




void NCTable::setLabel( const string & nlabel )
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

    if ( ! handleInput( key ) )
    {
	switch ( key )
	{
	    case CTRL( 'o' ):
		{
		    // get the column
		    wpos at( ScreenPos() + wpos( win->height() / 2, 1 ) );

		    YItemCollection ic;
		    ic.reserve( _header.size() );
		    unsigned int i = 0;

		    for ( vector<NCstring>::const_iterator it = _header.begin();
			  it != _header.end() ; it++, i++ )
		    {
			// strip the align mark
			string col = ( *it ).Str();
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
			myPad()->setOrder( column );

		    //remove the popup
		    YDialog::deleteTopmostDialog();

		    return NCursesEvent::none;
		}

	    case KEY_SPACE:
	    case KEY_RETURN:
		if ( !multiselect )
		{
		    if ( notify() && citem != -1 )
			return NCursesEvent::Activated;
		}
		else
		{
		    toggleCurrentItem();
		}
		break;

	}
    }


    if ( notify() && immediateMode() && citem != getCurrentItem() )
    {
	ret = NCursesEvent::SelectionChanged;
    }

    if ( !multiselect )
	selectCurrentItem();

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