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
 * This is an abstract base class for NCTablePad and NCTreePad.
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

    unsigned HotCol() const { return _itemStyle.HotCol(); }

    /// Expand or shrink to have exactly *count* logical lines
    void SetLines( unsigned count );

    void SetLines( std::vector<NCTableLine*> & newItems );

    void ClearTable() { SetLines( 0 ); }

    void Append( NCTableLine * item ) { AddLine( Lines(), item ); }

    void Append( std::vector<NCTableCol*> & newItems, int index = -1 );

    /**
     * Add *item* at position *idx*, expanding if needed
     * @param item we take ownership
     *
     * @deprecated Used only by Append; undefiend behaviour if used after
     * sorting
     **/
    void AddLine( unsigned idx, NCTableLine * item );

    /**
     * Return the line at *idx* for read-only operations.
     **/
    const NCTableLine * GetLine( unsigned idx ) const;

    /**
     * Return line at *idx* for read-write operations and mark it as modified.
     **/
    NCTableLine * ModifyLine( unsigned idx );

    /**
     * Return the current line (the line at the cursor position) or 0 if there
     * is none.
     **/
    NCTableLine * GetCurrentLine() const;

    /**
     * Handle a keyboard input event. Return 'true' if the event is now
     * handled, 'false' if it should be propagated to the parent widget.
     **/
    virtual bool handleInput( wint_t key );


private:

    // Disable unwanted assignment operator and copy constructor

    NCTablePadBase & operator=( const NCTablePadBase & );
    NCTablePadBase( const NCTablePadBase & );


protected:

    virtual wsze UpdateFormat();

    /**
     * Call the current item's handleInput() method and return 'true' if the
     * event is now handled, 'false' if not.
     **/
    virtual bool currentItemHandleInput( wint_t key );

    /**
     * Update _visibleItems: Clear the old contents, iterate over all lines and
     * check which ones are currently visible.
     **/
    void updateVisibleItems();

    void setFormatDirty() { dirty = _dirtyFormat = true; }

    virtual int  dirtyPad() { return setpos( CurPos() ); }

    /**
     * Redraw the pad.
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


    /// ensure that a line with the specified index exists
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



/**
 * A column (one cell) used as a selection marker:
 * `[ ]`/`[x]` or `( )`/`(x)`.
 **/
class NCTableTag : public NCTableCol
{
public:

    /**
     * Constructor.
     *
     * @param item (must not be nullptr, not owned)
     * @param sel currently selected, draw an `x` inside
     * @param singleSel if true  draw this in a radio-button style `(x)`;
     *                  if false draw this in a checkbox style     `[x]`
     **/
    NCTableTag( YItem *item, bool sel = false, bool singleSel = false )
        : NCTableCol( NCstring( singleSel ? "( )" : "[ ]" ), SEPARATOR )
        , _yitem( item )
        , _selected( sel )
        , _singleSelection( singleSel )
    {
	// store pointer to this tag in Yitem data
	_yitem->setData( this );
    }

    virtual ~NCTableTag() {}

    virtual void SetLabel( const NClabel & ) { /*NOOP*/; }

    virtual void DrawAt( NCursesWindow &    w,
                         const wrect        at,
			 NCTableStyle &     tableStyle,
			 NCTableLine::STATE linestate,
			 unsigned           colidx ) const
    {
	NCTableCol::DrawAt( w, at, tableStyle, linestate, colidx );

	if ( _selected )
	{
	    setBkgd( w, tableStyle, linestate, DATA );
	    w.addch( at.Pos.L, at.Pos.C + 1, 'x' );
	}
    }

    virtual bool Selected() const        { return _selected; }

    virtual void SetSelected( bool sel ) { _selected = sel; }

    virtual bool SingleSelection() const { return _singleSelection; }

    YItem *origItem() const              { return _yitem; }

private:

    YItem * _yitem; ///< (not owned, never nullptr, should have been a reference)
    bool    _selected;
    bool    _singleSelection;
};


#endif  // NCTablePadBase_h
