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

   File:       NCTableItem.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCTableItem_h
#define NCTableItem_h

#include <iosfwd>
#include <vector>

#include "position.h"
#include "NCWidget.h"
#include <yui/YTableItem.h>

class NCTableStyle;
class NCTableCol;


class NCTableLine
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCTableLine & OBJ );

    NCTableLine & operator=( const NCTableLine & );
    NCTableLine( const NCTableLine & );

public:

    enum STATE
    {
	S_NORMAL    = 0x00,
	S_ACTIVE    = 0x01,
	S_DISABELED = 0x10,
	S_HIDDEN    = 0x20,
	S_HEADLINE  = 0x40
    };

private:

    std::vector<NCTableCol*> Items;
    void assertCol( unsigned idx );

    unsigned state;

    int index;

    YTableItem *yitem;

protected:

    mutable STATE vstate;
    virtual void DrawItems( NCursesWindow & w, const wrect at,
			    NCTableStyle & tableStyle,
			    bool active ) const;

public:

    NCTableLine( unsigned cols, int index = -1, const unsigned s = S_NORMAL );
    NCTableLine( std::vector<NCTableCol*> & nItems, int index = -1, const unsigned s = S_NORMAL );
    void setOrigItem( YTableItem *it );
    YTableItem *origItem() const { return yitem; }

    virtual ~NCTableLine();

    unsigned Cols() const { return Items.size(); }

    void     SetCols( unsigned idx );
    void     SetCols( std::vector<NCTableCol*> & nItems );
    void     ClearLine()  { SetCols( 0 ); }

    std::vector<NCTableCol*> GetItems() const { return Items; }

    void Append( NCTableCol * item ) { AddCol( Cols(), item ); }

    void AddCol( unsigned idx, NCTableCol * item );
    void DelCol( unsigned idx );

    NCTableCol *       GetCol( unsigned idx );
    const NCTableCol * GetCol( unsigned idx ) const
    {
	return const_cast<NCTableLine*>( this )->GetCol( idx );
    }

    void  SetState( const STATE s ) { state |= s; }

    void  ClearState( const STATE s ) { state &= ~s; }

    bool  isHidden() const    { return ( state & S_HIDDEN ); }

    bool  isDisabeled() const { return ( state & S_DISABELED ); }

    bool  isSpecial() const   { return ( state & ( S_HIDDEN | S_DISABELED ) ); }

    bool  isActive() const    { return ( state & S_ACTIVE ); }

    virtual bool isVisible() const  { return !isHidden(); }

    virtual bool isEnabeled() const { return isVisible() && !isDisabeled(); }

    int getIndex() const { return index; }

public:

    virtual int  handleInput( wint_t key ) { return 0; }

    virtual int  ChangeToVisible()	{ return 0; }

    virtual unsigned Hotspot( unsigned & at ) const { at = 0; return 0; }

    virtual void UpdateFormat( NCTableStyle & TableStyle );

    virtual void DrawAt( NCursesWindow & w, const wrect at,
			 NCTableStyle & tableStyle,
			 bool active ) const;

    void stripHotkeys();
};



class NCTableCol
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCTableCol & OBJ );

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

private:

    NClabel label;
    STYLE   style;

public:

    NCTableCol( const NCstring & l = "", const STYLE & st = ACTIVEDATA );
    virtual ~NCTableCol();

    const NClabel & Label() const { return label; }

    virtual void SetLabel( const NClabel & l ) { label = l; }

    void stripHotkey() { label.stripHotkey(); }

protected:

    chtype setBkgd( NCursesWindow & w,
		    NCTableStyle & tableStyle,
		    NCTableLine::STATE linestate,
		    STYLE colstyle ) const ;

public:

    virtual wsze Size() const { return wsze( 1, label.width() ); }

    virtual void DrawAt( NCursesWindow & w, const wrect at,
			 NCTableStyle & tableStyle,
			 NCTableLine::STATE linestate,
			 unsigned colidx ) const;

    bool	  hasHotkey() const { return label.hasHotkey(); }

    unsigned char hotkey()    const { return label.hotkey(); }
};



class NCTableHead : public NCTableLine
{

public:

    NCTableHead( unsigned cols )		: NCTableLine( cols )	{}

    NCTableHead( std::vector<NCTableCol*> & nItems ) : NCTableLine( nItems ) {}

    virtual ~NCTableHead() {}

public:

    virtual void DrawAt( NCursesWindow & w, const wrect at,
			 NCTableStyle & tableStyle,
			 bool active ) const;
};



class NCTableStyle
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCTableStyle & OBJ );

private:

    NCTableHead		headline;
    std::vector<unsigned>	colWidth;
    std::vector<NC::ADJUST>	colAdjust;

    const NCWidget & parw;

    unsigned colSepwidth;
    chtype   colSepchar;
    unsigned hotCol;

public:

    static const chtype currentBG = ( chtype ) - 1;

    NCTableStyle( const NCWidget & p );
    ~NCTableStyle() {}

    bool SetStyleFrom( const std::vector<NCstring> & head );
    void SetSepChar( const chtype sepchar )	{ colSepchar = sepchar; }

    void SetSepWidth( const unsigned sepwidth ) { colSepwidth = sepwidth; }

    void SetHotCol( const int hcol )
    {
	hotCol = ( hcol < 0 || Cols() <= ( unsigned )hcol ) ? -1 : hcol;
    }

    void ResetToMinCols()
    {
	colWidth.clear();
	AssertMinCols( headline.Cols() );
	headline.UpdateFormat( *this );
    }

    void AssertMinCols( unsigned num )
    {
	if ( colWidth.size() < num )
	{
	    colWidth.resize( num, 0 );
	    colAdjust.resize( colWidth.size(), NC::LEFT );
	}
    }

    void MinColWidth( unsigned num, unsigned val )
    {
	AssertMinCols( num );

	if ( val > colWidth[num] )
	    colWidth[num] = val;
    }

    NC::ADJUST ColAdjust( unsigned num ) const { return colAdjust[num]; }

    unsigned Cols()		      const { return colWidth.size(); }

    unsigned ColWidth( unsigned num ) const { return colWidth[num]; }

    unsigned ColSepwidth()	      const { return colSepwidth; }

    chtype   ColSepchar()	      const { return colSepchar; }

    unsigned HotCol()		      const { return hotCol; }

    const NCstyle::StList & listStyle() const { return parw.listStyle(); }

    chtype getBG() const { return listStyle().item.plain; }

    chtype getBG( const NCTableLine::STATE lstate,
		  const NCTableCol::STYLE  cstyle = NCTableCol::PLAIN ) const;

    chtype highlightBG( const NCTableLine::STATE lstate,
			const NCTableCol::STYLE  cstyle,
			const NCTableCol::STYLE  dstyle = NCTableCol::PLAIN ) const ;

    chtype hotBG( const NCTableLine::STATE lstate, unsigned colidx ) const
    {
	return ( colidx == hotCol ) ? getBG( lstate, NCTableCol::HINT ) : currentBG;
    }

    const NCTableLine & Headline() const { return headline; }

    unsigned TableWidth() const
    {
	unsigned twidth = 0;

	for ( unsigned i = 0; i < Cols(); ++i )
	    twidth += colWidth[i];

	if ( Cols() > 1 )
	    twidth += colSepwidth * ( Cols() - 1 );

	return twidth;
    }
};


#endif // NCTableItem_h
