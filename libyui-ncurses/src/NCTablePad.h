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

   File:       NCTablePad.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCTablePad_h
#define NCTablePad_h

#include <iosfwd>
#include <vector>
#include <memory>		// unique_ptr

#include "NCTableItem.h"
#include "NCPad.h"
#include "NCstring.h"

class NCTableLine;
class NCTableCol;

/// Just a pair: column number, reverse flag.
class NCTableSortStrategyBase
{
public:

    NCTableSortStrategyBase() : _column(0), _reverse(false) {}
    NCTableSortStrategyBase(int column) : _column(column), _reverse(false) {}

    virtual ~NCTableSortStrategyBase() {}

    virtual void sort (
		       std::vector<NCTableLine *>::iterator itemsBegin,
		       std::vector<NCTableLine *>::iterator itemsEnd
		       ) = 0;

    int getColumn () const		{ return _column; }
    void setColumn ( int column )	{ _column = column; }

    bool isReverse () const		{ return _reverse; }
    void setReverse ( bool reverse )	{ _reverse = reverse; }

private:

    int _column;
    bool _reverse;

};


class NCTableSortDefault : public NCTableSortStrategyBase
{
public:
    /// sort in place
    virtual void sort ( std::vector<NCTableLine *>::iterator itemsBegin,
			std::vector<NCTableLine *>::iterator itemsEnd ) override;

private:
    class Compare
    {
    public:
	Compare ( int column, bool reverse )
	    : column(column), reverse(reverse)
	    {}

	bool operator() ( const NCTableLine * first,
			  const NCTableLine * second ) const;

    private:

	// if available returns the sort key otherwise the first line of the label
	std::wstring smartSortKey( const NCTableLine * tableLine ) const;

	long long toNumber(const std::wstring& s, bool* ok) const;

	const int column;
	const bool reverse;
    };

};

/// A column (1 cell) used as a selection marker: `[ ]`/`[x]` or `( )`/`(x)`.
class NCTableTag : public NCTableCol
{
private:

    YItem *yitem; ///< (not owned, never nullptr, should have been a reference)
    bool selected;
    bool single_selection;

public:

    /// @param item (must not be nullptr, not owned)
    /// @param sel currently selected, draw an `x` inside
    /// @param single_sel if true  draw this in a radio-button style `(x)`;
    ///                   if false draw this in a checkbox style     `[x]`
    NCTableTag( YItem *item, bool sel = false, bool single_sel = false )
        : NCTableCol( NCstring( single_sel ? "( )" : "[ ]" ), SEPARATOR )
        , yitem( item )
        , selected( sel )
        , single_selection( single_sel )
    {
	// store pointer to this tag in Yitem data
	yitem->setData( this );
    }

    virtual ~NCTableTag() {}

    virtual void SetLabel( const NClabel & ) { /*NOOP*/; }

    virtual void DrawAt( NCursesWindow & w, const wrect at,
			 NCTableStyle & tableStyle,
			 NCTableLine::STATE linestate,
			 unsigned colidx ) const
    {
	NCTableCol::DrawAt( w, at, tableStyle, linestate, colidx );

	if ( selected )
	{
	    setBkgd( w, tableStyle, linestate, DATA );
	    w.addch( at.Pos.L, at.Pos.C + 1, 'x' );
	}
    }

    virtual void SetSelected( bool sel ) { selected = sel; }

    virtual bool Selected() const       { return selected; }

    virtual bool SingleSelection() const       { return single_selection; }

    YItem *origItem() const { return yitem; }
};


/// A NCPad for a NCTable
class NCTablePad : public NCPad
{

    friend std::ostream & operator<<( std::ostream & str, const NCTablePad & obj );

    NCTablePad & operator=( const NCTablePad & );
    NCTablePad( const NCTablePad & );

private:

    NCursesPad	Headpad;
    bool	dirtyHead;
    bool	dirtyFormat;    ///< does table format (size) need recalculating

    NCTableStyle	 ItemStyle;
    NCTableLine		 Headline;
    std::vector<NCTableLine*> Items; ///< (owned)
    wpos		 citem;      ///< current/cursor position

    std::unique_ptr<NCTableSortStrategyBase> sortStrategy;

    /// ensure a line with *index* exists
    void assertLine( unsigned index );

protected:

    void	 DirtyFormat() { dirty = dirtyFormat = true; }

    virtual wsze UpdateFormat();

    virtual int  dirtyPad() { return setpos( CurPos() ); }

    virtual int  setpos( const wpos & newpos );
    virtual int  DoRedraw();
    virtual void updateScrollHint();

    virtual void directDraw( NCursesWindow & w, const wrect at, unsigned lineno );

public:

    NCTablePad( int lines, int cols, const NCWidget & p );
    virtual ~NCTablePad();

public:

    virtual void wRecoded();

    /// CurPos().L is the index of the selected item
    virtual wpos CurPos() const;
    virtual bool handleInput( wint_t key );

    bool setItemByKey( int key );

    wsze tableSize()
    {
	return dirtyFormat ? UpdateFormat()
	       : wsze( Lines(), ItemStyle.TableWidth() );
    }

    /// Sort by *column*; if that is the sorting column already, sort in
    /// reverse order if *do_reverse*.
    /// Do nothing if column < 0.
    void setOrder( int column, bool do_reverse = false );

    void sort();

public:

    bool SetHeadline( const std::vector<NCstring> & head );

    virtual void SendHead()
    {
	SetHead( Headpad, srect.Pos.C );
	dirtyHead = false;
    }

    void SetSepChar( const chtype colSepchar )
    {
	ItemStyle.SetSepChar( colSepchar );
    }

    void SetSepWidth( const unsigned sepwidth )
    {
	ItemStyle.SetSepWidth( sepwidth );
    }

    void SetHotCol( int hcol )
    {
	ItemStyle.SetHotCol( hcol );
    }

    /// Table columns (logical, not screen)
    unsigned Cols()  const { return ItemStyle.Cols(); }

    /// Table lines (logical, not screen)
    unsigned Lines() const { return Items.size(); }

    unsigned HotCol()const { return ItemStyle.HotCol(); }

    /// Expand or shrink to have exactly *count* logical lines
    void     SetLines( unsigned count );
    void     SetLines( std::vector<NCTableLine*> & nItems );
    void     ClearTable()  { SetLines( 0 ); }

    void Append( NCTableLine * item )		{ AddLine( Lines(), item ); }

    void Append( std::vector<NCTableCol*> & nItems, int index = -1 )
    {
	AddLine( Lines(), new NCTableLine( nItems, index ) );
    }

    /// Add *item* at position *idx*, expanding if needed
    /// @param item we take ownership
    /// @deprecated Used only by Append; meaning after sorting is undefined
    void AddLine( unsigned idx, NCTableLine * item );

    /// blanks out the line at *idx*
    /// @deprecated Unused
    void DelLine( unsigned idx );

    const NCTableLine * GetLine( unsigned idx ) const;
    /// Return line at *idx* and mark it as modified
    NCTableLine *	ModifyLine( unsigned idx );

    /// Find the item index in a sorted table.
    /// Return -1 if not found.
    /// An item/line remembers its insertion index...
    /// @param id the index before sorting
    int findIndexById(int id) const;

    void stripHotkeys();

    /// @param newSortStrategy (we take ownership)
    void setSortStrategy ( NCTableSortStrategyBase * newSortStrategy ) // dyn. allocated
    {
        if ( newSortStrategy != 0 )
            sortStrategy.reset ( newSortStrategy );
    }
};


#endif // NCTablePad_h
