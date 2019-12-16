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
    virtual void sort (
		       std::vector<NCTableLine *>::iterator itemsBegin,
		       std::vector<NCTableLine *>::iterator itemsEnd
		       ) override
        {
	    std::sort ( itemsBegin, itemsEnd, Compare(getColumn(), isReverse()) );
	}

private:
    class Compare
    {
    public:
	Compare ( int column, bool reverse )
	    : column(column), reverse(reverse)
	    {}

	// TODO does the comparator always guarantee strict weak
	// ordering, e.g. when mixing strings and numbers?

	bool operator() ( const NCTableLine * first,
			  const NCTableLine * second ) const
	    {
                std::wstring w1 = first->GetCol( column )->Label().getText().begin()->str();
                std::wstring w2 = second->GetCol( column )->Label().getText().begin()->str();
		wchar_t *endptr1 = 0;
		wchar_t *endptr2 = 0;

		long long number1 = std::wcstoll( w1.c_str(), &endptr1, 10 );
		long long number2 = std::wcstoll( w2.c_str(), &endptr2, 10 );

		if ( *endptr1 == L'\0' && *endptr2 == L'\0' )
		{
		    // both are numbers
		    return !reverse ? number1 < number2 : number1 > number2;
		}
		else
		{
		    // compare strings using collating information
		    int result = std::wcscoll ( w1.c_str(), w2.c_str() );

		    return !reverse ? result < 0 : result > 0;
		}
	    }

    private:
	const int column;
	const bool reverse;
    };

};



class NCTableTag : public NCTableCol
{
private:

    YItem *yitem;
    bool selected;
    bool single_selection;

public:

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



class NCTablePad : public NCPad
{

    friend std::ostream & operator<<( std::ostream & str, const NCTablePad & obj );

    NCTablePad & operator=( const NCTablePad & );
    NCTablePad( const NCTablePad & );

private:

    NCursesPad	Headpad;
    bool	dirtyHead;
    bool	dirtyFormat;

    NCTableStyle	 ItemStyle;
    NCTableLine		 Headline;
    std::vector<NCTableLine*> Items;
    wpos		 citem;

    std::unique_ptr<NCTableSortStrategyBase> sortStrategy;

    void assertLine( unsigned idx );

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

    virtual wpos CurPos() const;
    virtual bool handleInput( wint_t key );

    bool setItemByKey( int key );

    wsze tableSize()
    {
	return dirtyFormat ? UpdateFormat()
	       : wsze( Lines(), ItemStyle.TableWidth() );
    }

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

    unsigned Cols()  const { return ItemStyle.Cols(); }

    unsigned Lines() const { return Items.size(); }

    unsigned HotCol()const { return ItemStyle.HotCol(); }

    void     SetLines( unsigned idx );
    void     SetLines( std::vector<NCTableLine*> & nItems );
    void     ClearTable()  { SetLines( 0 ); }

    void Append( NCTableLine * item )		{ AddLine( Lines(), item ); }

    void Append( std::vector<NCTableCol*> & nItems, int index = -1 )
    {
	AddLine( Lines(), new NCTableLine( nItems, index ) );
    }

    void AddLine( unsigned idx, NCTableLine * item );
    void DelLine( unsigned idx );

    const NCTableLine * GetLine( unsigned idx ) const;
    NCTableLine *	ModifyLine( unsigned idx );

    void stripHotkeys();

    void setSortStrategy ( NCTableSortStrategyBase * newSortStrategy ) // dyn. allocated
    {
        if ( newSortStrategy != 0 )
            sortStrategy.reset ( newSortStrategy );
    }
};


#endif // NCTablePad_h
