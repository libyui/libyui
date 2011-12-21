/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/

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

   File:       NCstyle.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCstyle_h
#define NCstyle_h

#include <ncursesw/ncurses.h>

#include <iosfwd>
#include <string>
#include <vector>

#include "NCtypes.h"


struct NCattribute
{
    //
    // available colors and color pairs
    //
    static int _colors;
    static int _pairs;

    //if we have color support, return number of available colors
    //(at most 8 though)
    //will be initialized by init_color() function
    inline static int colors()	    { return _colors ? _colors : ::COLORS; }

    // do the same with color pairs
    inline static int color_pairs() { return _pairs ? _pairs : ::COLOR_PAIRS; }

    //
    //
    // color pair to chtype
    //
    inline static chtype color_pair( short fg, short bg ) { return colors() ? COLOR_PAIR( bg * colors() + COLOR_WHITE - fg ) : A_NORMAL; }

    inline static chtype color_pair( int i )		  { return colors() ? COLOR_PAIR( i ) : A_NORMAL; }

    inline static short  fg_color_pair( int i )		  { return colors() ? ( COLOR_WHITE - ( i % colors() ) ) : -1; }

    inline static short  bg_color_pair( int i )		  { return colors() ? ( i / colors() ) : -1; }

    //
    // chtype to color pair
    //
    inline static int	 color_pair_of( chtype ch )	  { return PAIR_NUMBER( ch ); }

    inline static short  fg_color_of( chtype ch )	  { return fg_color_pair( color_pair_of( ch ) ); }

    inline static short  bg_color_of( chtype ch )	  { return bg_color_pair( color_pair_of( ch ) ); }

    //
    // chtype manipualtion
    //
    static const chtype style_mask = A_ATTRIBUTES & ~A_COLOR & ~A_ALTCHARSET;
    static const chtype color_mask = A_COLOR;
    static const chtype char_mask  = A_CHARTEXT | A_ALTCHARSET;
    //
    inline static chtype getStyle( chtype a )		  { return a & style_mask; }

    inline static chtype getColor( chtype a )		  { return a & color_mask; }

    inline static chtype getChar( chtype a )		  { return a & char_mask;  }

    inline static chtype getNonChar( chtype a )		  { return a & ~char_mask;  }

    inline static void	 setStyle( chtype & a, chtype ch ) { a = ( a & ~style_mask ) | ( ch & style_mask ); }

    inline static void	 setColor( chtype & a, chtype ch ) { if ( colors() ) a = ( a & ~color_mask ) | ( ch & color_mask ); }

    inline static void	 setChar( chtype & a, chtype ch ) { a = ( a & ~char_mask ) | ( ch & char_mask );  }

    inline static void	 addStyle( chtype & a, chtype ch ) { a = a | ( ch & style_mask ); }

    inline static void	 delStyle( chtype & a, chtype ch ) { a = a & ~( ch & style_mask ); }

    inline static void	 toggleStyle( chtype & a, chtype ch ) { setStyle( a, ( a & ~ch ) | (( a ^ ch ) & ch ) ); }

    inline static void	 addAlt( chtype & a ) { a |= A_ALTCHARSET; }

    inline static void	 delAlt( chtype & a ) { a &= ~A_ALTCHARSET; }

    //
    inline static short  getFg( chtype a )		{ return fg_color_of( a ); }

    inline static short  getBg( chtype a )		{ return bg_color_of( a ); }

    inline static void	 setFg( chtype & a, short c )	{ if ( colors() ) setColor( a, color_pair(( colors() + c ) % colors(), getBg( a ) ) ); }

    inline static void	 setBg( chtype & a, short c )	{ if ( colors() ) setColor( a, color_pair( getFg( a ), ( colors() + c ) % colors() ) ); }

private:

    friend class NCurses;

    static void init_colors()
    {

	//get number of available colors (property of the terminal)
	//the same with color pairs

	_colors = ::COLORS;
	_pairs = ::COLOR_PAIRS;

	//if we have more than 8 colors available, use only 8 anyway
	//in order to preserve the same color palette even for
	//e.g. 256color terminal

	if ( _colors > COLOR_WHITE + 1 )
	    //_colors = 8 at all times
	    _colors = COLOR_WHITE + 1;

	if ( _pairs > _colors * _colors )
	    //_pairs == 64 at all times
	    _pairs = _colors * _colors;

	for ( short i = 1; i < color_pairs(); ++i )
	    ::init_pair( i, fg_color_pair( i ), bg_color_pair( i ) );
    }
};



class NCattrset
{

private:

    vector<chtype> attr;

public:

    NCattrset( unsigned num ) : attr( num, A_NORMAL ) {}

    virtual ~NCattrset() {}

public:

    const chtype & operator[]( unsigned a ) const { return attr[a]; }

    chtype getAttr( unsigned a ) const { return attr[a]; }

    chtype getStyle( unsigned a ) const { return NCattribute::getStyle( attr[a] ); }

    chtype getColor( unsigned a ) const { return NCattribute::getColor( attr[a] ); }

    chtype getChar( unsigned a ) const { return NCattribute::getChar( attr[a] ); }

    chtype getNonChar( unsigned a ) const { return NCattribute::getNonChar( attr[a] ); }

    void setAttr( unsigned a, chtype ch ) { attr[a] = ch; }

    void setStyle( unsigned a, chtype ch ) { NCattribute::setStyle( attr[a], ch ); }

    void setColor( unsigned a, chtype ch ) { NCattribute::setColor( attr[a], ch ); }

    void setChar( unsigned a, chtype ch ) { NCattribute::setChar( attr[a], ch ); }

    void addStyle( unsigned a, chtype ch ) { NCattribute::addStyle( attr[a], ch ); }

    void delStyle( unsigned a, chtype ch ) { NCattribute::delStyle( attr[a], ch ); }

    void toggleStyle( unsigned a, chtype ch ) { NCattribute::toggleStyle( attr[a], ch ); }

    void addAlt( unsigned a ) { NCattribute::addAlt( attr[a] ); }

    void delAlt( unsigned a ) { NCattribute::delAlt( attr[a] ); }

public:

    short getFg( unsigned a ) const { return NCattribute::getFg( attr[a] ); }

    short getBg( unsigned a ) const { return NCattribute::getBg( attr[a] ); }

    void setFg( unsigned a, short c ) { NCattribute::setFg( attr[a], c ); }

    void setBg( unsigned a, short c ) { NCattribute::setBg( attr[a], c ); }
};



class NCstyle
{

    friend class NCStyleDef;

public:

    enum STglobal
    {
	AppTitle,
	AppText,
	// last entry
	MaxSTglobal
    };

    enum STlocal
    {
	DialogBorder,
	DialogTitle,
	DialogActiveBorder,
	DialogActiveTitle,
	//
	DialogText,
	DialogHeadline,
	//
	DialogDisabled,
	//
	DialogPlain,
	DialogLabel,
	DialogData,
	DialogHint,
	DialogScrl,
	DialogActivePlain,
	DialogActiveLabel,
	DialogActiveData,
	DialogActiveHint,
	DialogActiveScrl,
	//
	DialogFramePlain,
	DialogFrameLabel,
	DialogFrameData,
	DialogFrameHint,
	DialogFrameScrl,
	DialogActiveFramePlain,
	DialogActiveFrameLabel,
	DialogActiveFrameData,
	DialogActiveFrameHint,
	DialogActiveFrameScrl,
	//
	ListTitle,
	ListPlain,
	ListLabel,
	ListData,
	ListHint,
	ListSelPlain,
	ListSelLabel,
	ListSelData,
	ListSelHint,
	//
	ListActiveTitle,
	ListActivePlain,
	ListActiveLabel,
	ListActiveData,
	ListActiveHint,
	ListActiveSelPlain,
	ListActiveSelLabel,
	ListActiveSelData,
	ListActiveSelHint,
	//
	RichTextPlain,
	RichTextTitle,
	RichTextLink,
	RichTextArmedlink,
	RichTextActiveArmedlink,
	RichTextVisitedLink,
	RichTextB,
	RichTextI,
	RichTextT,
	RichTextBI,
	RichTextBT,
	RichTextIT,
	RichTextBIT,
	//
	ProgbarCh,
	ProgbarBgch,
	//
	TextCursor,
	// last entry
	MaxSTlocal
    };

public:

    struct StBase
    {
	const chtype & title;
	const chtype & text;
	StBase( const chtype & ti, const chtype & te )
		: title( ti ), text( te )
	{}
    };

    struct STChar
    {
	const chtype & chattr;
	chtype getChar()    const { return NCattribute::getChar( chattr ); }

	chtype getNonChar() const { return NCattribute::getNonChar( chattr ); }

	STChar( const chtype & cha )
		: chattr( cha )
	{}
    };

    struct StItem
    {
	const chtype & plain;
	const chtype & label;
	const chtype & data;
	const chtype & hint;
	StItem( const chtype & p, const chtype & l, const chtype & d, const chtype & h )
		: plain( p ), label( l ), data( d ), hint( h )
	{}
    };

    struct StWidget : public StItem
    {
	const chtype & scrl;
	StWidget( const chtype & p, const chtype & l, const chtype & d, const chtype & h,
		  const chtype & s )
		: StItem( p, l, d, h ), scrl( s )
	{}
    };

    struct StList
    {
	const chtype & title;
	const StItem   item;
	const StItem   selected;
	StList( const chtype & t, const StItem & i, const StItem & s )
		: title( t ), item( i ), selected( s )
	{}

	const StItem & getItem( bool sel ) const { return sel ? selected : item; }
    };

    struct StProgbar
    {
	const STChar bar;
	const STChar nonbar;
	StProgbar( const chtype & b, const chtype & nb )
		: bar( b ), nonbar( nb )
	{}
    };

    struct StRichtext
    {
	const chtype & plain;
	const chtype & title;
	const chtype & link;
	const chtype & armedlink;
	const chtype & activearmedlink;
	const chtype & visitedlink;
	const chtype & B;
	const chtype & I;
	const chtype & T;
	const chtype & BI;
	const chtype & BT;
	const chtype & IT;
	const chtype & BIT;

	StRichtext( const chtype & p, const chtype & tit,
		    const chtype & l, const chtype & al, const chtype & aal, const chtype & vl,
		    const chtype & b, const chtype & i, const chtype & t,
		    const chtype & bi, const chtype & bt, const chtype & it,
		    const chtype & bit )
		: plain( p ), title( tit ),
		link( l ), armedlink( al ), activearmedlink( aal ), visitedlink( vl ),
		B( b ), I( i ), T( t ),
		BI( bi ), BT( bt ), IT( it ),
		BIT( bit )
	{}

	const chtype & getArmed( NC::WState s ) const
	{
	    return ( s == NC::WSactive ) ? activearmedlink : armedlink;
	}
    };

    struct StDialog
    {
	StBase	  border;
	StBase	  activeBorder;
	StBase	  dumb;
	StWidget  disabled;
	StWidget  normal;
	StWidget  active;
	StWidget  frame;
	StWidget  activeFrame;
	StList	  list;
	StList	  activeList;
	StList	  disabledList;
	StProgbar  progbar;
	StRichtext richtext;
	const chtype & cursor;
	//
	StDialog( const StBase & b, const StBase & ab, const StBase & d, const StWidget & dis,
		  const StWidget & n, const StWidget & a,
		  const StWidget & f, const StWidget & af,
		  const StList & l, const StList & al, const StList & dl,
		  const StProgbar & pbar,
		  const StRichtext & rtext,
		  const chtype & curs )
		: border( b ), activeBorder( ab ), dumb( d ), disabled( dis ),
		normal( n ), active( a ),
		frame( f ), activeFrame( af ),
		list( l ), activeList( al ), disabledList( dl ),
		progbar( pbar ),
		richtext( rtext ),
		cursor( curs )
	{}

public:

	const StBase & getDlgBorder( bool active ) const { return active ? activeBorder : border; }

	const StBase & getDumb()		   const { return dumb; }

	const StWidget & getWidget( NC::WState s, bool nonactive = false ) const
	{
	    switch ( s )
	    {

		case NC::WSdisabeled:
		    return disabled;

		case NC::WSactive:
		    return nonactive ? normal : active;

		case NC::WSnormal:

		case NC::WSdumb:
		    break;
	    }

	    return normal;
	}

	const StWidget & getFrame( NC::WState s ) const
	{
	    switch ( s )
	    {

		case NC::WSdisabeled:
		    return disabled;

		case NC::WSactive:
		    return activeFrame;

		case NC::WSnormal:

		case NC::WSdumb:
		    break;
	    }

	    return frame;
	}

	const StList & getList( NC::WState s ) const
	{
	    switch ( s )
	    {

		case NC::WSdisabeled:
		    return disabledList;

		case NC::WSactive:
		    return activeList;

		case NC::WSnormal:

		case NC::WSdumb:
		    break;
	    }

	    return list;
	}
    };

public:

    
    class Style : private NCattrset, public StDialog
    {

	friend class NCstyle;

	Style & operator=( const Style & ); // no assignment

    private:

	static unsigned sanitycheck();
	static NCattrset attrGlobal;

    public:

	NCattrset & getAttrGlobal() { return attrGlobal; }

	NCattrset & getAttrLocal() { return *this; }

    private:

	StDialog initDialog(); // use this to initialize StDialog

    public:

	Style();
	Style( const Style & rhs );
	virtual ~Style();

    public:

	const chtype & attr( STglobal a ) const { return attrGlobal[a]; }

	const chtype & attr( STlocal a )  const { return NCattrset::operator[]( a ); }

	const chtype & operator()( STglobal a ) const { return attr( a ); }

	const chtype & operator()( STlocal a )	const { return attr( a ); }
    };

public:

    enum StyleSet
    {
	DefaultStyle,
	InfoStyle,
	WarnStyle,
	PopupStyle,
	// last entry
	MaxStyleSet
    };

private:

    string	  styleName;
    string	  term;
    vector<Style> styleSet;

    StyleSet fakestyle_e;
    void     fakestyle( StyleSet f );
    Style &  getStyle( StyleSet a ) { return styleSet[a]; }

public:

    NCstyle( string term_t );
    ~NCstyle();

    const chtype & operator()( STglobal a ) const { return Style::attrGlobal[a]; }

    const Style &  operator[]( StyleSet a ) const
    {
	if ( fakestyle_e != MaxStyleSet )
	    return styleSet[fakestyle_e];

	return styleSet[a];
    }

    void changeSyle();
    void nextStyle();

    static string dumpName( StyleSet a );
    static string dumpName( STglobal a );
    static string dumpName( STlocal a );
};


#endif // NCstyle_h
