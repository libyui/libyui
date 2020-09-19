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

   File:       NCTable.h

   Authors:    Michael Andres <ma@suse.de>
               Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef NCTable_h
#define NCTable_h

#include <iosfwd>

#include <yui/YTable.h>
#include "NCPadWidget.h"
#include "NCTablePad.h"

/**
 * A table with rows and columns.
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
     * or -1 if there is none, i.e. the table is empty.
     **/
    virtual int getCurrentIndex() const { return getCurrentItem(); }

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
     * Set a sorting strategy.
     **/
    void setSortStrategy( NCTableSortStrategyBase * newStrategy )
        { myPad()->setSortStrategy( newStrategy ); }


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
     * Internal overloaded version of addItem() that recurses into child items
     * if there are any. This is called by the previous version.
     **/
    virtual void addItem( NCTableLine *      parentLine,
                          YItem *            yitem,
                          bool               preventRedraw,
                          NCTableLine::STATE state = NCTableLine::S_NORMAL );

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
     * Change individual cell of a table line (to newtext) provided for
     *		      backwards compatibility
     **/
    void cellChanged( int index, int colnum, const std::string & newtext );
    void cellChanged( const YTableCell *cell );

    /**
     * Interactive sorting by a user-selected column:
     *
     * Open a popup with the (non-empty) column headers and let the user choose
     * one for sorting.
     **/
    void interactiveSort();

private:

    // Disable unwanted assignment opearator and copy constructor

    NCTable & operator=( const NCTable & );
    NCTable( const NCTable & );


    //
    // Data members
    //

    // Number of non-data prefix columns for things like the multi-selection
    // indicator [x] or the tree structure indicator.
    int                   _prefixCols;

    bool                  _nestedItems;
    bool	          _bigList;
    bool 	          _multiSelect;
};


#endif // NCTable_h
