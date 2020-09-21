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

   File:       NCTree.cc

   Authors:    Michael Andres <ma@suse.de>
               Stefan Hundhammer <shundhammer@suse.de>

/-*/

#define	 YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCTree.h"
#include "YNCursesUI.h"

#include <yui/TreeItem.h>
#include <yui/YSelectionWidget.h>

using std::string;
using std::endl;


NCTree::NCTree( YWidget *      parent,
                const string & nlabel,
                bool           multiselection,
                bool           recursiveselection )
    : YTree( parent, nlabel, multiselection, recursiveselection )
    , NCPadWidget( parent )
    , _multiSelect( multiselection )
    , _nextItemIndex( 0 )
{
    // yuiDebug() << endl;

    if ( multiselection && recursiveselection )
	yuiDebug() << "NCTree recursive multi selection ON" << endl;
    else if ( multiselection )
	yuiDebug() << "NCTree multi selection ON" << endl;

    setLabel( nlabel );
}


NCTree::~NCTree()
{

}


inline const NCTreeLine *
NCTree::getTreeLine( unsigned idx ) const
{
    if ( myPad() )
	return dynamic_cast<const NCTreeLine *>( myPad()->GetLine( idx ) );
    else
	return 0;
}


inline NCTreeLine *
NCTree::modifyTreeLine( unsigned idx )
{
    if ( myPad() )
	return dynamic_cast<NCTreeLine *>( myPad()->ModifyLine( idx ) );
    else
        return 0;
}


int NCTree::preferredWidth()
{
    wsze sze = wsze::max( defsze, wsze( 0, labelWidth() + 2 ) );
    return sze.W;
}


int NCTree::preferredHeight()
{
    wsze sze = wsze::max( defsze, wsze( 0, labelWidth() + 2 ) );
    return sze.H;
}


void NCTree::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YWidget::setEnabled( do_bv );
}


void NCTree::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


YTreeItem * NCTree::getCurrentItem() const
{
    YTreeItem * yitem = 0;

    if ( myPad() && myPad()->GetCurrentLine() )
    {
	const NCTreeLine * currentLine = dynamic_cast<const NCTreeLine *>( myPad()->GetCurrentLine() );

	if ( currentLine )
	    yitem = currentLine->YItem();
    }

    // yuiDebug() << "-> " << ( yitem ? yitem->label().c_str() : "noitem" ) << endl;

    return yitem;
}


void NCTree::deselectAllItems()
{
    if ( _multiSelect )
    {
	YItemCollection selectedItems = YTree::selectedItems();

	for ( YItemConstIterator it = selectedItems.begin(); it != selectedItems.end(); ++it )
	{
	    selectItem( *it, false );
	}
    }

    YTree::deselectAllItems();
}


void NCTree::selectItem( YItem * item, bool selected )
{
    if ( !myPad() )
	return;

    YTreeItem * treeItem =  dynamic_cast<YTreeItem *>( item );
    YUI_CHECK_PTR( treeItem );

    YTreeItem * currentItem = getCurrentItem();

    // retrieve position of the item
    int at = treeItem->index();

    NCTreeLine * currentLine = 0;
    NCTableCol * currentCol  = 0;

    if ( _multiSelect )
    {
	currentLine = modifyTreeLine( at );

	if ( currentLine )
	{
	    currentCol = currentLine->GetCol(0);
	}
    }

    if ( !selected )
    {
	if ( !_multiSelect && ( treeItem == currentItem ) )
	{
	    YTree::deselectAllItems();
	}
	else
	{
	    YTree::selectItem( treeItem, false );

	    if ( currentCol )
	    {
		currentCol->SetLabel( NCstring( string( currentLine->treeLevel() + 3, ' ' ) + "[ ] "
                                                + item->label() ) );
	    }
	}
    }
    else
    {
	YTree::selectItem( treeItem, selected );

	if ( _multiSelect && currentCol )
	{
	    currentCol->SetLabel( NCstring( string( currentLine->treeLevel() + 3, ' ' ) + "[x] "
                                            + item->label() ) );
	}

	// Highlight the selected item and possibly expand the tree if it is in
	// a currently hidden branch

	myPad()->ShowItem( getTreeLine( at ) );
    }
}


void NCTree::selectItem( int index )
{
    YItem * item = YTree::itemAt( index );

    if ( item )
	selectItem( item, true );
    else
	YUI_THROW( YUIException( "Can't find selected item" ) );
}


void NCTree::setLabel( const string & nlabel )
{
    YTree::setLabel( nlabel );
    NCPadWidget::setLabel( NCstring( nlabel ) );
}


void NCTree::rebuildTree()
{
    DelPad();
    Redraw();
}


NCPad * NCTree::CreatePad()
{
    wsze    psze( defPadSze() );
    NCPad * npad = new NCTreePad( psze.H, psze.W, *this );
    npad->bkgd( listStyle().item.plain );

    return npad;
}


void NCTree::CreateTreeLines( NCTreeLine * parentLine,
                              NCTreePad  * pad,
                              YItem      * item )
{
    // Set the item index explicitely: It is set to -1 by default which makes
    // selecting items painful.

    item->setIndex( _nextItemIndex++ );

    YTreeItem * treeItem = dynamic_cast<YTreeItem *>( item );
    YUI_CHECK_PTR( treeItem );

    NCTreeLine * line = new NCTreeLine( parentLine, treeItem, _multiSelect );
    pad->Append( line );

    if ( item->selected() )
    {
        // Retrieve the position of the item
        int at = treeItem->index();

        NCTreeLine * currentLine = 0;
        NCTableCol * currentCol = 0;

        if ( _multiSelect )
        {
            currentLine = modifyTreeLine( at );

            if ( currentLine )
                currentCol = currentLine->GetCol(0);

            if ( currentCol )
            {
                currentCol->SetLabel( NCstring( string( currentLine->treeLevel() + 3, ' ' ) + "[x] "
                                                + item->label() ) );
            }
        }

        // Highlight the selected item and expand the tree if it is in a
        // currently hidden branch

        pad->ShowItem( getTreeLine( at ) );
    }

    // Recursively create TreeLines for the children of this item

    for ( YItemIterator it = item->childrenBegin();  it < item->childrenEnd(); ++it )
    {
	CreateTreeLines( line, pad, *it );
    }
}


YTreeItem * NCTree::currentItem()
{
    return getCurrentItem();
}


void NCTree::DrawPad()
{
    if ( !myPad() )
    {
	yuiWarning() << "PadWidget not yet created" << endl;
	return;
    }

    _nextItemIndex = 0;

    // Iterate over the toplevel items

    for ( YItemIterator it = itemsBegin(); it < itemsEnd(); ++it )
    {
        // Create a TreeLine for this item.
        // This will recurse into children if there are any.

	CreateTreeLines( 0, myPad(), *it );
    }

    NCPadWidget::DrawPad();
}


NCursesEvent NCTree::wHandleInput( wint_t key )
{
    NCursesEvent ret = NCursesEvent::none;
    YTreeItem * oldCurrentItem = getCurrentItem();

    // Call the pad's input handler via NCPadWidget::handleInput()
    // which may call its base pad class's input handler
    // which may call the current item's input handler.
    //
    // Notice that most keys are handled on the level of the pad or the item,
    // not here. See
    // - NCTreePad::handleInput()
    // - NCTablePadBase::handleInput()
    // - NCTreeLine::handleInput()
    // - NCTableLine::handleInput()

    bool handled = handleInput( key ); // NCTreePad::handleInput()

    const YItem * currentItem = getCurrentItem();

    if ( !currentItem )
	return ret;

    if ( _multiSelect )
    {
	if ( ! handled )
	{
	    switch ( key )
	    {
		// KEY_SPACE is handled in NCTreeLine::handleInput()

		case KEY_RETURN:

		    if ( currentItem->selected() )
			selectItem( const_cast<YItem *>(currentItem), false );
		    else
			selectItem( const_cast<YItem *>(currentItem), true );

		    if ( notify() )
			return NCursesEvent::ValueChanged;

		    break;
	    }
	}
    }
    else
    {
	if ( ! handled )
	{
	    switch ( key )
	    {
		// KEY_SPACE is handled in NCTreeLine::handleInput
		case KEY_RETURN:

		    if ( notify() )
			return NCursesEvent::Activated;

		    break;
	    }
	}

	YTree::selectItem( const_cast<YItem *>( currentItem ), true );
    }

    if ( notify() && immediateMode() && ( oldCurrentItem != currentItem ) )
	    ret = NCursesEvent::SelectionChanged;

    // yuiDebug() << "Notify: " << ( notify() ? "true" : "false" )
    // << " Return event: " << ret.reason << endl;

    return ret;
}


void NCTree::activate()
{
    // Send an activation event for this widget

    NCursesEvent event = NCursesEvent::Activated;
    event.widget = this;
    YNCursesUI::ui()->sendEvent(event);
}


void NCTree::deleteAllItems()
{
    YTree::deleteAllItems();
    myPad()->ClearTable();
}


//
// ----------------------------------------------------------------------
//


NCTreeLine::NCTreeLine( NCTreeLine * parentLine,
                        YTreeItem  * item,
                        bool         multiSelection )
    : NCTableLine( parentLine,
                   item,
                   0,           // cols
                   -1,          // idx
                   true,        // nested
                   S_NORMAL )   // lineState
    , _multiSelect( multiSelection )
{
    if ( _multiSelect )
        _prefixPlaceholder += "[ ] ";

    string contentStr = prefixPlaceholder() + _yitem->label();
    Append( new NCTableCol( NCstring( contentStr ) ) );
}


NCTreeLine::~NCTreeLine()
{

}


bool NCTreeLine::ChangeToVisible()
{
    if ( isVisible() )
        return false;   // no status change

    if ( parent() )
    {
        // Make sure the parent is visible.
        // This recurses upwards until the toplevel.

        parent()->ChangeToVisible();


        // Make sure this line and all siblings on this level are visible

        for ( NCTreeLine * sibling = parent()->firstChild();
              sibling;
              sibling = sibling->nextSibling() )
        {
            sibling->ClearState( S_HIDDEN );
            sibling->YItem()->setOpen( true );
        }
    }
    else // No parent (i.e. this is a toplevel item)
    {
        // Make sure this item is visible

        ClearState( S_HIDDEN );
        YItem()->setOpen( true );
    }

    return true; // status change (the line was invisible before)
}


unsigned NCTreeLine::Hotspot( unsigned & at ) const
{
    return 6;
}


bool NCTreeLine::handleInput( wint_t key )
{
    bool handled = false;

    switch ( key )
    {
        // At this time, there are no more special keys to handle on this
        // level. This method is a stub for future extension if any more keys
        // need to be handled.
        //
        // Add 'case KEY_XXX' branches here if there should be any
        // and don't forget to set 'handled' to 'true'.
#if 0
        case KEY_SOMETHING:     // Sample
            doSomething();
            handled = true;
            break;
#endif

        default: // Call parent class input handler
            handled = NCTableLine::handleInput( key );
            break;
    }

    return handled;
}


void NCTreeLine::DrawAt( NCursesWindow & w,
                         const wrect     at,
                         NCTableStyle  & tableStyle,
                         bool            active ) const
{
    // Call the base class function to draw the normal content
    // without the tree hierarchy line graphics

    NCTableLine::DrawAt( w, at, tableStyle, active );

    if ( !isSpecial() )
        w.bkgdset( tableStyle.getBG( _vstate, NCTableCol::SEPARATOR ) );

    drawPrefix( w, at, tableStyle );
}

