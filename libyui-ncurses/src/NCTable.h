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

   File:       NCTable.h

   Authors:    Michael Andres <ma@suse.de>
               Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef NCTable_h
#define NCTable_h

#include <yui/YTable.h>
#include "NCPadWidget.h"
#include "NCTablePad.h"
#include "NCTableSort.h"


/**
 * A table with rows and columns. Items may be nested.
 *
 * See also
 * https://github.com/libyui/libyui-ncurses/blob/master/doc/nctable-and-nctree.md
 */
class NCTable : public YTable, public NCPadWidget
{
    friend std::ostream & operator<<( std::ostream & str, const NCTable & obj );

public:

    NCTable( YWidget *      parent,
             YTableHeader * tableHeader,
             bool           multiSelection = false );

    virtual ~NCTable();

    /**
     * Set the table header (the first line inside the table) as strings.
     **/
    void setHeader( const std::vector<std::string>& head );

    /**
     * Get the table headers (the first line inside the table) as strings.
     * Alignment flags are removed.
     **/
    std::vector<std::string> getHeader() const;

    /**
     * Add items.
     *
     * Reimplemented from YSelectionWidget to optimize sorting.
     **/
    virtual void addItems( const YItemCollection & itemCollection );

    /**
     * Add one item.
     *
     * Implemented from YSelectionWidget.
     **/
    virtual void addItem( YItem *yitem )
        { addItem( yitem, NCTableLine::S_NORMAL ); }

    /**
     * Add one item with the specified state and redraw the table.
     *
     * This overloaded version is first defined here and also used in
     * NCPopupTable. Notice that this does not have a default value for the
     * 'state' parameter to avoid clashing with the addItem( YItem * ) version
     * inherited from YSelectionWidget which might be used from C++
     * applications using libyui.
     **/
    virtual void addItem( YItem *            yitem,
                          NCTableLine::STATE state );

    /**
     * Delete all items and clear the pad.
     *
     * Implemented from YSelectionWidget.
     **/
    virtual void deleteAllItems();

    /**
     * Get the index of the current item (the item under the cursor)
     * or -1 if there is none.
     **/
    virtual int getCurrentIndex() const;

    /**
     * Get the index of the current item (the item under the cursor)
     * or -1 if there is none, i.e. the table is empty.
     *
     * FIXME: This is a misnomer of epic proportions. This should be named
     * getCurrentIndex(). A method called getCurrentItem() should return the
     * item (i.e. a pointer or a reference), not an index! But since this is
     * all over the place in derived classes and in libyui-ncurses-pkg as well,
     * this is not a trivial thing to fix.
     **/
    virtual int getCurrentItem() const;

    /**
     * Return a pointer to the current item (the item under the cursor)
     * or 0 if there is none, i.e. the table is empty.
     *
     * FIXME: This is what getCurrentItem() should really be.
     **/
    YItem * getCurrentItemPointer();

    /**
     * Set the current item to the specified index.
     **/
    virtual void setCurrentItem( int index );

    /**
     * Scroll to the first item.
     **/
    virtual void scrollToFirstItem();

    /**
     * Select or deselect an item.
     *
     * Implemented from YSelectionWidget.
     **/
    virtual void selectItem( YItem *yitem, bool selected );

    /**
     * Select the current item (the item under the cursor).
     **/
    void selectCurrentItem();

    /**
     * Deselect all items.
     *
     * Implemented from YSelectionWidget.
     **/
    virtual void deselectAllItems();

    /**
     * Keyboard input handler.
     *
     * This is the starting point for handling key events. From here, key
     * events are propagated to the pad and to the items.
     *
     * Implemented from NCWidget.
     **/
    virtual NCursesEvent wHandleInput( wint_t key );

    /**
     * libyui geometry management:
     * Return the preferred width for this widget.
     *
     * Implemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * libyui geometry management:
     * Return the preferred height for this widget.
     *
     * Implemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * libyui geometry management:
     * Apply the width and height assigned from the parent layout widget.
     *
     * Implemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Set the label (the caption) above the table.
     *
     * YTable does not specify a label because a table has a whole row of
     * headers.
     **/
    virtual void setLabel( const std::string & nlabel );

    /**
     * Enable or disable this widget.
     *
     * Implemented from YWidget.
     **/
    virtual void setEnabled( bool do_bv );

    /**
     * Set the keyboard focus to this widget.
     *
     * Implemented from YWidget.
     **/
    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    /**
     * Select an item by its hotkey.
     * Used only in NCPopupTable::wHandleHotkey().
     **/
    bool setItemByKey( int key );

    /**
     * Set the column separator character.
     * Used only in NCPopupTable and in NCFileSelection.
     **/
    void SetSepChar( const chtype colSepchar )
        { myPad()->SetSepChar( colSepchar ); }

    /**
     * Set the separator width.
     * Used only in NCPopupTable.
     **/
    void SetSepWidth( const unsigned sepwidth )
        { myPad()->SetSepWidth( sepwidth ); }

    /**
     * Set the hotkey column (?).
     * Used only in NCPopupTable.
     **/
    void SetHotCol( int hcol )
        { myPad()->SetHotCol( hcol ); }

    /**
     * Flag: Is this a big list?
     **/
    bool bigList() const { return _bigList; }

    /**
     * Set the "big list" flag.
     **/
    void setBigList( bool big ) { _bigList = big; }

    /**
     * Remove all hotkeys from the pad.
     **/
    void stripHotkeys() { myPad()->stripHotkeys(); }

    /**
     * Set a sorting strategy. This class takes ownership.
     **/
    void setSortStrategy( NCTableSortStrategyBase * newStrategy );

    /**
     * Return the current sorting strategy.
     **/
    NCTableSortStrategyBase * sortStrategy() const { return _sortStrategy; }


protected:

    /**
     * Code location for logging.
     *
     * Implemented from NCWidget.
     **/
    virtual const char * location() const { return "NCTable"; }

    /**
     * Create an empty pad and set its background.
     **/
    virtual NCPad * CreatePad();

    /**
     * Return the TreePad that belongs to this widget.
     *
     * Overloaded from NCPadWidget to narrow the type to the actual one used in
     * this widget.
     **/
    virtual NCTablePad * myPad() const
	{ return dynamic_cast<NCTablePad*>( NCPadWidget::myPad() ); }

    /**
     * Internal overloaded version of addItem().
     *
     * This creates a visual representation of the new table line consisting of
     * individual cells. If 'preventRedraw' is 'false', the table is redrawn;
     * otherwise, it is up to the caller to redraw the table.
     *
     * This is used in addItem( yitem ) and addItems( itemCollection ) in this
     * class, but also in the derived NCFileSelection and NCPkgTable classes.
     **/
    virtual void addItem( YItem *            yitem,
                          bool               preventRedraw,
                          NCTableLine::STATE state = NCTableLine::S_NORMAL );

    /**
     * Add a pad line (an NCTableLine) for 'yitem' and recurse into any of its
     * children (and grandchildren etc.).
     *
     * Make sure to update _nestedItems (by iterating over all the YItems)
     * before the first one is added so they will reserve some screen space for
     * the tree hierarchy line graphics (using the prefix placeholder).
     *
     * This can realistically only be done when bulk-adding all YItems at
     * once. If they are added one by one, the first few may not have any
     * children, so their corresponding pad lines will not reserve screen space
     * for the prefix, leading to ugly results.
     *
     * If there is no item nesting at all, this does not matter, of course.
     **/
    virtual void addPadLine( NCTableLine *      parentLine,
                             YItem *            yitem,
                             bool               preventRedraw,
                             NCTableLine::STATE state = NCTableLine::S_NORMAL );


    /**
     * Build or rebuild the pad lines: Clear the pad, iterate over all YItems
     * and add a corresponding NCTableLine to the pad. This recurses into any
     * child YItems.
     *
     * This does not redraw the pad. Do that from the outside.
     **/
    void rebuildPadLines();

    /**
     * Rebuild the table header line.
     **/
    void rebuildHeaderLine();

    /**
     * Return the NCurses alignment string for the alignment of the
     * specified column: One of "L", "C", "R" (Left, Center, Right).
     **/
    NCstring alignmentStr( int col );

    /**
     * Return 'true' if any item in the item collection has any children,
     * 'false' otherwise.
     **/
    bool hasNestedItems( const YItemCollection & itemCollection ) const;

    bool hasNestedItems( YItemConstIterator begin,
                         YItemConstIterator end ) const;

    /**
     * Optimization for NCurses from libyui:
     * Notification that multiple changes are about to come.
     *
     * Implemented from YWidget.
     **/
    virtual void startMultipleChanges() { startMultidraw(); }

    /**
     * Optimization for NCurses from libyui:
     * Notification that multiple changes are now finished.
     *
     * Implemented from YWidget.
     **/
    virtual void doneMultipleChanges()	{ stopMultidraw(); }

    /**
     * Toggle the current item between selected and not selected.
     **/
    void toggleCurrentItem();

    /**
     * Notification that a cell has now changed content:
     * Set that cell's content also in the corresponding table line.
     **/
    void cellChanged( const YTableCell * cell );

    /**
     * Change the cell with item index 'index' and column no. 'col' to 'newText'.
     **/
    void setCell( int index, int col, const std::string & newText );

    /**
     * Recursively iterate over items and assign each one a unique item index.
     *
     * As long as the items don't have any child items, each one simply gets
     * its initial position in the item collection. When there are children,
     * however, there will be gaps between the index of one toplevel item and
     * the next.
     *
     * It is generally unsafe to make assumptions about the indices except that
     * they are unique within one table, and an item will keep its initial
     * index, no matter how the table is sorted.
     *
     * The indices restart from 0 after the table has been cleared, i.e. after
     * deleteAllItems() or at the start of setItems().
     **/
    void assignIndex( YItemConstIterator begin,
                      YItemConstIterator end );

    /**
     * Assign an item a unique index.
     **/
    void assignIndex( YItem * item );

    /**
     * Interactive sorting by a user-selected column:
     *
     * Open a popup with the (non-empty) column headers and let the user choose
     * one for sorting.
     **/
    void interactiveSort();

    /**
     * Sort the items by column no. 'sortCol' with the current sort strategy.
     *
     * This sorts the YItems and recreates all NCTableLines.
     * All YItem pointers remain valid, but the NCTableLines do not.
     **/
    void sortItems( int sortCol, bool reverse = false );

    /**
     * Sort the YItems between 'begin' and 'end' using the current sort
     * strategy.
     **/
    void sortYItems( YItemIterator begin,
                     YItemIterator end   );

private:

    // Disable unwanted assignment opearator and copy constructor

    NCTable & operator=( const NCTable & );
    NCTable( const NCTable & );


    //
    // Data members
    //

    // Number of non-data prefix columns for things like the multi-selection
    // indicator ("[ ]" / "[x]")
    int  _prefixCols;

    bool _nestedItems;
    bool _bigList;
    bool _multiSelect;

    int  _lastSortCol;
    bool _sortReverse;
    NCTableSortStrategyBase * _sortStrategy;    //< owned
};


std::ostream & operator<<( std::ostream & stream, const YItem * item );


#endif // NCTable_h
