/*
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

   File:       NCTablePadBase.h

   Authors:    Michael Andres <ma@suse.de>
               Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef NCTablePadBase_h
#define NCTablePadBase_h

#include <vector>
#include "NCPad.h"
#include "NCTableItem.h"

class NCTableCol;


/**
 * An NCPad for an NCTable or an NCTree.
 *
 * This is an abstract base class for NCTablePad and NCTreePad.
 *
 * See also
 * https://github.com/libyui/libyui-ncurses/blob/master/doc/nctable-and-nctree.md
 *
 * Notice that this class is heavily used in other selection widgets like
 *
 * - NCFileSelection
 * - NCItemSelector
 * - NCMultiSelectionBox
 * - NCSelectionBox
 *
 * And the table classes from libyui-ncurses-pkg:
 * - NCPkgTable
 * - NCPkgLocaleTable
 * - NCPkgRepoTable
 *
 * So if you change anything here, make sure to test all those classes.
 **/
class NCTablePadBase : public NCPad
{
    friend std::ostream & operator<<( std::ostream & str, const NCTablePadBase & obj );

protected:
    /**
     * Constructor. This is protected because this is an abstract base class,
     * not meant to be instantiated on this level.
     **/
    NCTablePadBase( int lines, int cols, const NCWidget & p );

public:

    virtual ~NCTablePadBase();

    /**
     * Clear all content.
     **/
    void ClearTable();

    virtual void wRecoded();

    /// CurPos().L is the index of the selected item
    virtual wpos CurPos() const;

    wsze tableSize();

    /**
     * Return the number of lines that are currently visible.
     * This is updated in UpdateFormat().
     **/
    unsigned visibleLines() const { return _visibleItems.size(); }

    bool SetHeadline( const std::vector<NCstring> & head );

    virtual void SendHead()
    {
	SetHead( _headpad, srect.Pos.C );
	_dirtyHead = false;
    }

    void SetSepChar( const chtype colSepchar )
    {
	_itemStyle.SetSepChar( colSepchar );
    }

    void SetSepWidth( const unsigned sepwidth )
    {
	_itemStyle.SetSepWidth( sepwidth );
    }

    void SetHotCol( int hcol )
    {
	_itemStyle.SetHotCol( hcol );
    }

    /**
     * Return the number of table columns (logical, not screen)
     */
    unsigned Cols()   const { return _itemStyle.Cols(); }

    /**
     * Return the number of table lines (logical, not screen)
     **/
    unsigned Lines()  const { return _items.size(); }

    bool empty() const { return _items.empty(); }

    unsigned HotCol() const { return _itemStyle.HotCol(); }

    /**
     * Expand or shrink to have exactly *count* logical lines
     **/
    void SetLines( unsigned count );

    void SetLines( std::vector<NCTableLine*> & newItems );

    /**
     * Add *item* at position *idx*, expanding if needed
     * @param item we take ownership
     *
     * @deprecated Used only by Append; undefined behaviour if used after
     * sorting
     **/
    void AddLine( unsigned idx, NCTableLine * item );

    /**
     * Add one item to the end of _items.
     **/
    void Append( NCTableLine * item ) { AddLine( Lines(), item ); }

    /**
     * Create a new item from 'cells' and add it to the end of _items.
     **/
    void Append( std::vector<NCTableCol*> & cells, int index )
        { AddLine( Lines(), new NCTableLine( cells, index ) ); }

    /**
     * Return the line at *idx* for read-only operations.
     **/
    const NCTableLine * GetLine( unsigned idx ) const;

    /**
     * Return line at *idx* for read-write operations and mark it as modified.
     **/
    NCTableLine * ModifyLine( unsigned idx );

    /**
     * Find the item with index 'idx' in the items and return its position.
     * Return -1 if not found.
     **/
    int findIndex( unsigned idx ) const;

    /**
     * Return the current line (the line at the cursor position) or 0 if there
     * is none.
     **/
    NCTableLine * GetCurrentLine() const;

    /**
     * Handle a keyboard input event. Return 'true' if the event is now
     * handled, 'false' if it should be propagated to the parent widget.
     *
     * Most of the keys are now handled in the individual items' handlers
     * (NCTreeLine, NCTableLine). This method is mostly here as a stub for
     * future extensions.
     *
     * Reimplemented from NCPad.
     **/
    virtual bool handleInput( wint_t key );


private:

    // Disable unwanted assignment operator and copy constructor

    NCTablePadBase & operator=( const NCTablePadBase & );
    NCTablePadBase( const NCTablePadBase & );

    /**
     * Return the line with index 'idx' or 0 if not found.
     **/
    NCTableLine * getLineWithIndex( unsigned idx ) const;


protected:

    virtual wsze UpdateFormat();

    /**
     * Call the current item's handleInput() method and return 'true' if the
     * event is now handled, 'false' if not.
     **/
    virtual bool currentItemHandleInput( wint_t key );

    /**
     * Update the internal _visibleItems vector with the items that are
     * currently visible: Clear the old contents of the vector, iterate over
     * all lines and check which ones are currently visible.
     *
     * This does NOT do a screen update of the visible items!
     **/
    void updateVisibleItems();

    void setFormatDirty() { dirty = _dirtyFormat = true; }

    virtual int dirtyPad() { return setpos( CurPos() ); }

    /**
     * Redraw the pad.
     *
     * Reimplemented from NCPad.
     **/
    virtual int DoRedraw();

    /**
     * Prepare a redraw: Update the format if needed, set the background, clear
     * the old content.
     **/
    virtual void prepareRedraw();

    /**
     * Redraw the (visible) content lines one by one.
     **/
    virtual void drawContentLines();

    /**
     * Redraw the table header.
     **/
    virtual void drawHeader();

    /**
     * Base function for scrolling: Move the cursor position to 'newPos' and
     * redraw the old and the new current item with suitable attributes: The
     * new current item is highlighted, the old one is not.
     *
     * Reimplemented from NCPad.
     **/
    virtual int setpos( const wpos & newPos );

    /**
     * Return the current line number (the cursor position).
     **/
    int currentLineNo() const { return _citem.L; }

    /**
     * Set the current line number (the cursor position).
     **/
    void setCurrentLineNo( int newVal ) { _citem.L = newVal; }

    /**
     * Return the current column number (the cursor position).
     **/
    int currentColNo() const { return _citem.C; }

    /**
     * Set the current column number (the cursor position).
     **/
    void setCurrentColNo( int newVal ) { _citem.C = newVal; }

    /**
     * Ensure that a line with the specified index exists.
     * Enlarge or shrink if necessary.
     **/
    void assertLine( unsigned index );


    //
    // Data members
    //

    std::vector<NCTableLine*> _items;        ///< (owned)
    std::vector<NCTableLine*> _visibleItems; ///< not owned
    NCursesPad	              _headpad;
    bool	              _dirtyHead;
    bool	              _dirtyFormat;  ///< does table format (size) need recalculating?
    NCTableStyle	      _itemStyle;
    wpos		      _citem;        ///< current/cursor position
};


#endif  // NCTablePadBase_h
