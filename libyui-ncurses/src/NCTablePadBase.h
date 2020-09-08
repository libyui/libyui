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
     * not meant to be used directly.
     **/
    NCTablePadBase( int lines, int cols, const NCWidget & p );

public:
    virtual ~NCTablePadBase();


public:

    virtual void wRecoded();

    /// CurPos().L is the index of the selected item
    virtual wpos CurPos() const;

    wsze tableSize()
    {
	return dirtyFormat ? UpdateFormat()
	       : wsze( Lines(), ItemStyle.TableWidth() );
    }

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


private:

    // Disable unwanted assignment operator and copy constructor

    NCTablePadBase & operator=( const NCTablePadBase & );
    NCTablePadBase( const NCTablePadBase & );


protected:

    virtual wsze UpdateFormat() = 0;

    void	 DirtyFormat() { dirty = dirtyFormat = true; }

    virtual int  dirtyPad() { return setpos( CurPos() ); }

    /// ensure a line with *index* exists
    void assertLine( unsigned index );


    //
    // Data members
    //

    std::vector<NCTableLine*> Items;       ///< (owned)
    NCTableLine		      Headline;
    NCursesPad	              Headpad;
    bool	              dirtyHead;
    bool	              dirtyFormat; ///< does table format (size) need recalculating?
    NCTableStyle	      ItemStyle;
    wpos		      citem;       ///< current/cursor position
};



/// A column (one cell) used as a selection marker: `[ ]`/`[x]` or `( )`/`(x)`.
class NCTableTag : public NCTableCol
{
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

    virtual bool Selected() const        { return selected; }

    virtual void SetSelected( bool sel ) { selected = sel; }

    virtual bool SingleSelection() const { return single_selection; }

    YItem *origItem() const              { return yitem; }

private:

    YItem * yitem; ///< (not owned, never nullptr, should have been a reference)
    bool    selected;
    bool    single_selection;

};


#endif  // NCTablePadBase_h
