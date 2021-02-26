/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

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

using std::vector;

class NCTableSortStrategyBase
{
public:
    NCTableSortStrategyBase( ) { _uiColumn = -1; }
    
    virtual ~NCTableSortStrategyBase() {}

    virtual void sort (
		       vector<NCTableLine *>::iterator itemsBegin,
		       vector<NCTableLine *>::iterator itemsEnd,
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
		       vector<NCTableLine *>::iterator itemsBegin,
		       vector<NCTableLine *>::iterator itemsEnd,
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
		return first->GetCol( _uiCol )->Label().getText().begin()->str()
		    < second->GetCol( _uiCol )->Label().getText().begin()->str();
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
    vector<NCTableLine*> Items;
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

    bool SetHeadline( const vector<NCstring> & head );

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
    void     SetLines( vector<NCTableLine*> & nItems );
    void     ClearTable()  { SetLines( 0 ); }

    void Append( NCTableLine * item )		{ AddLine( Lines(), item ); }

    void Append( vector<NCTableCol*> & nItems, int index = -1 )
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
