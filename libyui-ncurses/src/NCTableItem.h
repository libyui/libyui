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

   File:       NCTableItem.h

   Authors:    Michael Andres <ma@suse.de>
               Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef NCTableItem_h
#define NCTableItem_h

#include <iosfwd>
#include <vector>

#include "position.h"
#include "NCWidget.h"
#include <yui/YTableItem.h>

class NCTableCol;
class NCTableStyle;
class NCTableTag;


/**
 * One line in a NCTable with multiple cells and an optional tree hierarchy.
 * Each line corresponds to a YItem subclass (YTableItem or YTreeItem).
 *
 * This class is also the base class for NCTreeLine; it provides most its
 * functionlity.
 *
 * Notice that on the libyui level, the inheritance hierarchy is
 *
 *     YTableItem < YTreeItem < YItem
 *
 * whereas on the libyui-ncurses level, it is
 *
 *     NCTreeLine < NCTableLine
 *
 * i.e. it's just the other way round. This is why it is safer to do dynamic
 * casts of the internal _yitem to YTreeItem rather than to YTableItem: If
 * used from an NCTree (i.e. YTree) widget, the items will all be YTreeItems; a
 * dynamic cast to YTableItem will fail.
 *
 * NOTE: "col", "column", here refer to only one cell, not the entire table column.
 *
 * See also
 * https://github.com/libyui/libyui-ncurses/blob/master/doc/nctable-and-nctree.md
 **/
class NCTableLine
{
public:

    enum STATE
    {
	S_NORMAL    = 0x00,
	S_ACTIVE    = 0x01,
	S_DISABLED  = 0x10,
	S_HIDDEN    = 0x20,
	S_HEADLINE  = 0x40
    };


    /**
     * Constructor: Create an NCTableLine and fill it with 'cells'. This object
     * takes over ownership of those cells and will delete it when appropriate.
     *
     * 'index' is a unique number with which to identify this line.
     *
     * 'nested' specifies whether any item in the table has any child items,
     * i.e. whether line graphics to visualize the tree structure should be drawn.
     *
     * 'state' is an OR'ed combination of the STATE enum.
     **/
    NCTableLine( std::vector<NCTableCol*> & cells,
                 int                        index  = -1,
                 bool                       nested = false,
                 unsigned                   state  = S_NORMAL );

    NCTableLine( NCTableLine *              parentLine,
                 YItem *                    yitem,
                 std::vector<NCTableCol*> & cells,
                 int                        index  = -1,
                 bool                       nested = false,
                 unsigned                   state  = S_NORMAL );

    /**
     * Constructor with a number of empty cells.
     **/
    NCTableLine( unsigned      colCount,
                 int           index     = -1,
                 bool          nested    = false,
                 unsigned      state     = S_NORMAL );

    NCTableLine( NCTableLine * parentLine,
                 YItem *       yitem,
                 unsigned      colCount,
                 int           index     = -1,
                 bool          nested    = false,
                 unsigned      state     = S_NORMAL );

    /**
     * Destructor.
     **/
    virtual ~NCTableLine();

    /**
     * Return the YItem this line corresponds to.
     **/
    YTableItem * origItem() const { return dynamic_cast<YTableItem *>( _yitem ); }

    /**
     * Set the YItem this line corresponds to.
     **/
    void setOrigItem( YTableItem *yitem );

    /**
     * Return the unique index by which this line can be identified.
     **/
    int index() const { return _index; }

    /**
     * Return the number of columns (cells) in this line.
     **/
    unsigned Cols() const { return _cells.size(); }

    /**
     * Set a number of (empty) columns (cells).
     **/
    void SetCols( unsigned idx );

    /**
     * Set the columns (cells).
     **/
    void SetCols( std::vector<NCTableCol*> & newCells );

    /**
     * Delete all content.
     **/
    void ClearLine()  { SetCols( 0 ); }

    /**
     * Return all columns (cells).
     * Ownership of the cells remains with this line; do not delete them!
     **/
    std::vector<NCTableCol*> GetItems() const { return _cells; }

    /**
     * Append one cell. Ownership is transferred to this line.
     **/
    void Append( NCTableCol * cell ) { AddCol( Cols(), cell ); }

    void AddCol( unsigned idx, NCTableCol * item );
    void DelCol( unsigned idx );

    /**
     * Return a non-const pointer for read/write operations to the column (the
     * cell) with the specified index or 0 if there is no such cell.
     *
     * This is the table cell counterpart to NCTablePad::ModifyLine(). This
     * does not set any 'dirty' flag.
     **/
    NCTableCol * GetCol( unsigned idx );

    /**
     * Return a const pointer for read-only operatoins to the column (the cell)
     * with the specified index or 0 if there is no such cell.
     *
     * This is the table cell counterpart to NCTablePad::GetLine().
     **/
    const NCTableCol * GetCol( unsigned idx ) const
    {
	return const_cast<NCTableLine*>( this )->GetCol( idx );
    }

    void  SetState  ( const STATE s ) { _state |= s; }
    void  ClearState( const STATE s ) { _state &= ~s; }

    bool  isHidden() const    { return ( _state & S_HIDDEN ); }
    bool  isDisabled() const  { return ( _state & S_DISABLED ); }
    bool  isSpecial() const   { return ( _state & ( S_HIDDEN | S_DISABLED ) ); }
    bool  isActive() const    { return ( _state & S_ACTIVE ); }

    virtual bool isVisible() const;

    virtual bool isEnabled() const { return isVisible() && !isDisabled(); }

    /**
     * Return 'true' if this should be displayed as nested items, i.e.
     * with line graphics connecting tree items and their children.
     * This needs to be set from the outside.
     **/
    virtual bool isNested() const { return _nested; }

    /**
     * Set the 'nested' status.
     **/
    virtual void setNested( bool val ) { _nested = val; }

    /**
     * Open this tree branch
     **/
    void openBranch();

    /**
     * Close this tree branch
     **/
    void closeBranch();

    /**
     * Toggle the open/closed state of this branch
     **/
    void toggleOpenClosedState();

    /**
     * Handle keyboard input. Return 'true' if the key event is handled,
     * 'false' to propagate it up to the pad.
     **/
    virtual bool handleInput( wint_t key );

    /**
     * Change a line that may have been invisible until now to be visible.
     *
     * Return 'true' if there was a status change, i.e. if it was invisible
     * before, 'false' otherwise.
     *
     * This default implementation does nothing and always returns 'false'.
     * Derived classes that can handle invisible items may want to overwrite
     * this.
     **/
    virtual bool ChangeToVisible() { return false; }

    virtual unsigned Hotspot( unsigned & at ) const { at = 0; return 0; }

    /**
     * Update TableStyle so that this line fits in
     **/
    virtual void UpdateFormat( NCTableStyle & tableStyle );

    /**
     * Create the real tree hierarchy line graphics prefix and store it in
     * _prefix
     **/
    virtual void updatePrefix();


    /// @param active is the table cursor here
    virtual void DrawAt( NCursesWindow & w,
                         const wrect     at,
			 NCTableStyle &  tableStyle,
			 bool            active ) const;

    void stripHotkeys();

    //
    // Tree operations
    //

    virtual NCTableLine * parent()      const { return _parent;      }
    virtual NCTableLine * firstChild()  const { return _firstChild;  }
    virtual NCTableLine * nextSibling() const { return _nextSibling; }

    void setParent     ( NCTableLine * newVal ) { _parent      = newVal; }
    void setFirstChild ( NCTableLine * newVal ) { _firstChild  = newVal; }
    void setNextSibling( NCTableLine * newVal ) { _nextSibling = newVal; }

    /**
     * Return the nesting level in the tree (toplevel is 0).
     **/
    int treeLevel() const { return _treeLevel; }

    /**
     * Set the tree nesting level.
     **/
    void setTreeLevel( int newVal ) { _treeLevel = newVal; }

    /**
     * Return the length of the prefix for tree hierarchy line graphics.
     **/
    int prefixLen() const { return _nested ? treeLevel() + 3 : 0; }

    /**
     * Return the tag cell or 0 if there is none.
     **/
    NCTableTag * tagCell() const;

    /**
     * Return a string of a number of blanks suitable for the indentation of
     * this tree level.
     **/
    std::string indentationStr() const;

protected:

    /**
     * Common init for tree-related things in the constructors that have a
     * 'parentLine' and a 'yitem' parameter.
     **/
    void treeInit( NCTableLine * parentLine, YItem * yitem );

    /**
     * Initialize _prefixPlaceholder, the placeholder for tree hierarchy line graphics.
     **/
    void initPrefixPlaceholder();

    /**
     * Add this line to the parent's tree hierarchy.
     **/
    void addToTree( NCTableLine * parent );

    /**
     * Return 'true' if yitem inherits YTreeItem or YTableItem and has its
     * 'open' flag set to 'true'.
     **/
    bool isOpen( YItem * yitem ) const;

    /**
     * Return the YItem this line corresponds to as its base class.
     **/
    YItem * yitem() const { return _yitem; }

    /**
     * Set the YItem this line corresponds to.
     **/
    void setYItem( YItem * yitem );

    virtual void DrawItems( NCursesWindow & w,
                            const wrect     at,
			    NCTableStyle &  tableStyle,
			    bool            active ) const;

    void assertCol( unsigned idx );

    /**
     * Return a placeholder for the prefix string for this line consisting of
     * enough blanks for the tree hierarchy line graphics.
     *
     * The real line graphics will be drawn over this in DrawAt().
     **/
    const std::string & prefixPlaceholder() const { return _prefixPlaceholder; }

    /**
     * Draw the tree hierarchy line graphics prefix in _prefix in window 'w'
     * into rectangle 'at' with style 'tableStyle'.
     **/
    void drawPrefix( NCursesWindow & w,
                     const wrect     at,
                     NCTableStyle  & tableStyle ) const;

private:

    friend std::ostream & operator<<( std::ostream & str, const NCTableLine & obj );

    // Disable unwanted assignment operator and copy constructor

    NCTableLine & operator=( const NCTableLine & );
    NCTableLine( const NCTableLine & );


    //
    // Data members
    //

protected:

    std::vector<NCTableCol*> _cells; ///< owned

    unsigned      _state;        ///< Or'ed STATE flags
    int           _index;        ///< unique index to identify this line
    YItem *       _yitem;        ///< not owned
    bool          _nested;       ///< using nested (tree-like) items?

    // Tree-related

    int           _treeLevel;
    NCTableLine * _parent;
    NCTableLine * _nextSibling;
    NCTableLine * _firstChild;

    // This should have been an argument for DrawItems.
    //
    // It needs to be mutable because some methods that change it promise to be
    // const, but they break that promise with this variable.
    mutable STATE _vstate;

    // Tree hierarchy line graphics for this line.
    //
    // chtype is a very basic NCurses type to store one character with its
    // attributes (bg/fg color).
    chtype *         _prefix;
    std::string      _prefixPlaceholder;
};


/**
 * One cell in an NCTableLine with a label and a cell-specific style.
 *
 * 'Col' in this context means just this one cell, not the entire column in the
 * table.
 *
 * The style (NCTableCol::STYLE) is just color information,
 * don't confuse with table sizing+alignment info, NCTableStyle.
 **/
class NCTableCol
{

    friend std::ostream & operator<<( std::ostream & str, const NCTableCol & obj );

public:

    enum STYLE
    {
	NONE = 0,    // use current bg
	PLAIN,	     // plain text
	DATA,	     // data style
	ACTIVEDATA,  // data style if line active, else plain
	HINT,	     // hint
	SEPARATOR    // separator
    };


    NCTableCol( const NCstring & label = "", STYLE st = ACTIVEDATA );

    virtual ~NCTableCol();

    const NClabel & Label() const { return _label; }
    virtual void SetLabel( const NClabel & newVal ) { _label = newVal; }
    virtual void SetLabel( const std::string & newVal ) { _label = NCstring( newVal ); }

    /**
     * Return the prefix that is drawn (without delimiter) before the label.
     * This can be used for an empty placeholder for tree hierarchy graphics.
     **/
    const NClabel & prefix() const { return _prefix; }

    virtual void setPrefix( const NClabel & newVal ) { _prefix = newVal; }
    virtual void setPrefix( const std::string & newVal ) { _prefix = NCstring( newVal); }
    int prefixWidth() const { return _prefix.width(); }

    /**
     * Return a wrect that is adjusted for the size of the prefix, i.e. a
     * little to the right and a little narrower.
     **/
    wrect prefixAdjusted( const wrect origRect ) const;

    virtual wsze Size() const { return wsze( 1, _prefix.width() + _label.width() ); }

    virtual void DrawAt( NCursesWindow &    w,
                         const wrect        at,
			 NCTableStyle &     tableStyle,
			 NCTableLine::STATE linestate,
			 unsigned           colidx ) const;

    void stripHotkey() { _label.stripHotkey(); }

    bool	  hasHotkey() const { return _label.hasHotkey(); }
    unsigned char hotkey()    const { return _label.hotkey(); }

protected:

    chtype setBkgd( NCursesWindow &    w,
		    NCTableStyle &     tableStyle,
		    NCTableLine::STATE linestate,
		    STYLE              colstyle ) const ;

private:

    NClabel _prefix;
    NClabel _label;
    STYLE   _style;
};


/**
 * The header line of an NCTable.
 **/
class NCTableHead : public NCTableLine
{

public:

    NCTableHead( unsigned cols )
        : NCTableLine( cols )
        {}

    NCTableHead( std::vector<NCTableCol*> & headCells )
        : NCTableLine( headCells )
        {}

    virtual ~NCTableHead()
        {}

    /**
     * Draw the header line with special attributes. That is the whole reason
     * of having a separate class for this.
     **/
    virtual void DrawAt( NCursesWindow & w,
                         const wrect     at,
			 NCTableStyle &  tableStyle,
			 bool            active ) const;
};


/// Styling for a NCTable: column widths, alignment and colors.
class NCTableStyle
{

    friend std::ostream & operator<<( std::ostream & str, const NCTableStyle & obj );

public:

    static const chtype currentBG = (chtype) - 1;

    NCTableStyle( const NCWidget & parentWidget );
    ~NCTableStyle() {}

    /// Reset columns, setting their alignment and optionally titles.
    /// Column widths are zeroed.
    /// @param head List of strings where their first character
    ///   is the alignment (L, R, C) and the optional rest is the column heading
    /// @return do we have a column heading
    bool SetStyleFrom( const std::vector<NCstring> & head );

    void SetSepChar( const chtype sepChar )	{ _colSepChar = sepChar; }

    /// total width of space between adjacent columns, including the separator character
    void SetSepWidth( const unsigned sepWidth ) { _colSepWidth = sepWidth; }

    void SetHotCol( int hcol )
    {
	_hotCol = ( hcol < 0 || Cols() <= (unsigned) hcol ) ? -1 : hcol;
    }

    /// Forget sizing based on table content, resize according to headline only
    void ResetToMinCols()
    {
	_colWidth.clear();
	AssertMinCols( _headline.Cols() );
	_headline.UpdateFormat( *this );
    }

    /// Ensure we know width and alignment for at least *num* columns.
    void AssertMinCols( unsigned num )
    {
	if ( _colWidth.size() < num )
	{
	    _colWidth.resize( num, 0 );
	    _colAdjust.resize( _colWidth.size(), NC::LEFT );
	}
    }

    /// Update colWidth[num] to be at least *val*.
    /// @param num column number (may be bigger than previously)
    /// @param val width of that column for some line
    void MinColWidth( unsigned num, unsigned val )
    {
	AssertMinCols( num );

	if ( val > _colWidth[num] )
	    _colWidth[ num ] = val;
    }

    NC::ADJUST ColAdjust( unsigned num ) const { return _colAdjust[num]; }

    unsigned Cols()		         const { return _colWidth.size(); }

    unsigned ColWidth( unsigned num )    const { return _colWidth[num]; }

    unsigned ColSepWidth()	         const { return _colSepWidth; }

    chtype   ColSepChar()	         const { return _colSepChar; }

    unsigned HotCol()		         const { return _hotCol; }

    const NCstyle::StList & listStyle()  const { return _parentWidget.listStyle(); }

    chtype getBG() const { return listStyle().item.plain; }

    chtype getBG( const NCTableLine::STATE lstate,
		  const NCTableCol::STYLE  cstyle = NCTableCol::PLAIN ) const;

    chtype highlightBG( const NCTableLine::STATE lstate,
			const NCTableCol::STYLE  cstyle,
			const NCTableCol::STYLE  dstyle = NCTableCol::PLAIN ) const ;

    chtype hotBG( const NCTableLine::STATE lstate, unsigned colidx ) const
    {
	return ( colidx == _hotCol ) ?
            getBG( lstate, NCTableCol::HINT ) : currentBG;
    }

    const NCTableLine & Headline() const { return _headline; }

    /// Add up the widths of columns with the separators
    unsigned TableWidth() const
    {
	unsigned twidth = 0;

	for ( unsigned i = 0; i < Cols(); ++i )
	    twidth += _colWidth[i];

	if ( Cols() > 1 )
	    twidth += _colSepWidth * ( Cols() - 1 );

	return twidth;
    }


private:

    const NCWidget &            _parentWidget;
    NCTableHead                 _headline;
    std::vector<unsigned>	_colWidth;  ///< column widths
    std::vector<NC::ADJUST>	_colAdjust; ///< column alignment


    /// total width of space between adjacent columns, including the separator character
    unsigned _colSepWidth;

    chtype   _colSepChar;	///< column separator character
    unsigned _hotCol;		///< which column is "hot"
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
        // Use parent DrawAt to draw the static part: "[ ]"
	NCTableCol::DrawAt( w, at, tableStyle, linestate, colidx );

	if ( _selected )
	{
            // Draw the "x" inside the "[ ]" with different attributes

	    setBkgd( w, tableStyle, linestate, DATA );
            wrect drawRect = prefixAdjusted( at );
	    w.addch( drawRect.Pos.L, drawRect.Pos.C + 1, 'x' );
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


#endif // NCTableItem_h
