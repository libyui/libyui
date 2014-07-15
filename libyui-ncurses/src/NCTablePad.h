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
#include <memory>		// auto_ptr

#include "NCTableItem.h"
#include "NCPad.h"
#include "NCstring.h"

class NCTableLine;
class NCTableCol;


class NCTableSortStrategyBase
{
public:
    NCTableSortStrategyBase( ) { _uiColumn = -1; }

    virtual ~NCTableSortStrategyBase() {}

    virtual void sort (
		       std::vector<NCTableLine *>::iterator itemsBegin,
		       std::vector<NCTableLine *>::iterator itemsEnd,
		       int  uiColumn
		       ) = 0;
    int getColumn ()			{ return _uiColumn; }
    void setColumn ( int column)	{ _uiColumn = column; }

private:
    int	_uiColumn;

};

class NCTableSortDefault : public NCTableSortStrategyBase {
public:
    virtual void sort (
		       std::vector<NCTableLine *>::iterator itemsBegin,
		       std::vector<NCTableLine *>::iterator itemsEnd,
		       int  uiColumn
		       )
        {
	    std::sort ( itemsBegin, itemsEnd, Compare(uiColumn) );
        }

private:
    class Compare
    {
    public:
	Compare ( int uiCol)
	    : _uiCol ( uiCol )
	    {}

	bool operator() ( NCTableLine * first,
			  NCTableLine * second
			  ) const
	    {
                std::wstring w1 = first->GetCol( _uiCol )->Label().getText().begin()->str();
                std::wstring w2 = second->GetCol( _uiCol )->Label().getText().begin()->str();
		wchar_t *endptr1 = 0;
		wchar_t *endptr2 = 0;

		long int number1 = std::wcstol( w1.c_str(), &endptr1, 10 );
		long int number2 = std::wcstol( w2.c_str(), &endptr2, 10 );

		if ( *endptr1 == L'\0' && *endptr2 == L'\0' )
		{
		    // both are numbers
		    return number1 < number2;
		}
		else
		{
		    // compare strings using collating information
		    int result = std::wcscoll ( w1.c_str(), w2.c_str() );

		    return result < 0;
		}
	    }

    private:
	int _uiCol;
    };


};

class NCTableTag : public NCTableCol
{
private:

    YItem *yitem;
    bool selected;

public:

    NCTableTag( YItem *item, const bool sel = false )
	    : NCTableCol( NCstring( "[ ]" ), SEPARATOR )
	    , yitem( item )
	    , selected( sel )
    {
	//store pointer to this tag in Yitem data
	yitem->setData( this );
    }

    virtual ~NCTableTag() {}

    virtual void SetLabel( const NCstring & ) { /*NOOP*/; }

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

    void SetSelected( const bool sel ) { selected = sel; }

    bool Selected() const	       { return selected; }

    YItem *origItem() { return yitem; }
};

class NCTablePad : public NCPad
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCTablePad & OBJ );

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

    std::auto_ptr<NCTableSortStrategyBase> sortStrategy;

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

    void SetHotCol( const int hcol )
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
