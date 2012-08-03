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

   File:       NCStyleDef.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#include <iostream>
#include <fstream>
#include <vector>
#include <list>


#define	 YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCstyle.h"


class NCStyleDef
{

public:

    // Self reference
    static NCStyleDef * NCStyleDef_p;

    // Reference to class NCstyle
    NCstyle & NCstyle_C;

    // Master panel
    NCursesPanel p;

    // draw panel frame in move mode or plain
    void pbox( bool on = false );

    // move panel interactive or determined by key
    int	 movePad( int key = -1 );

    static const NCursesPanel & pad()			{ return NCStyleDef_p->p; }

    static void			refresh()		{ NCStyleDef_p->p.refresh(); }

    static int			movepad( int key = -1 ) { return NCStyleDef_p->movePad( key ); }


    // Example popup is somwhat special. It's handmade, so we habe to
    // keep it in sync when attributes change.

    enum ExMode { EX_OFF, EX_ON, EX_TOGGLE, EX_UPDATE };

    static void showex( ExMode mode );

    // Advise NCstyle to fake attributes, i.e. always return the set we currently process
    static void fakestyle( NCstyle::StyleSet style )
    {
	NCStyleDef_p->NCstyle_C.fakestyle( style );

	if ( style != NCstyle::MaxStyleSet )
	{
	    NCStyleDef_p->doshowset( NCStyleDef_p->wSet.cset, true );
	    showex( EX_UPDATE );
	}
	else
	{
	    showex( EX_OFF );
	}
    }

    // to be called to reflect changed attributes on screen
    static void attrchanged()
    {
	NCurses::Redraw();
	showex( EX_UPDATE );
    }

    // SetTypes handled by struct Wset.

    enum SetType
    {
	Global = 0,
	DialogBorder,
	DialogBorderActive,
	DialogText,
	Widget,
	WidgetActive,
	FrameWidget,
	FrameWidgetActive,
	List,
	ListActive,
	RichText,
	ProgressBar,
	// last entry
	MaxSetType
    };

    static std::string dumpName( SetType a )
    {
#define PRT(t) case t: return #t;

	switch ( a )
	{
	    PRT( Global );
	    PRT( DialogBorder );
	    PRT( DialogBorderActive );
	    PRT( DialogText );
	    PRT( Widget );
	    PRT( WidgetActive );
	    PRT( FrameWidget );
	    PRT( FrameWidgetActive );
	    PRT( List );
	    PRT( ListActive );
	    PRT( RichText );
	    PRT( ProgressBar );

	    case MaxSetType:
		break;
	}

	return "unknown";

#undef PRT
    }

    // Aset serves as menu entry for struct Wchattr.
    // Any modification of the associated chtype is handled here.

    struct Aset
    {

private:
	chtype * attr_p;

public:
	std::string	 label;
	Aset( chtype & ch, const std::string & l ) : attr_p( &ch ), label( l )
	{}

	chtype attr()	  const { return *attr_p; }

	chtype textattr() const { return attr() &  ~NCattribute::char_mask; }

	void setBg( bool prev ) { NCattribute::setBg( *attr_p, NCattribute::getBg( *attr_p ) + ( prev ? -1 : 1 ) ); }

	void setFg( bool prev ) { NCattribute::setFg( *attr_p, NCattribute::getFg( *attr_p ) + ( prev ? -1 : 1 ) ); }

	void toggleStyle( chtype sty ) { NCattribute::toggleStyle( *attr_p, sty ); }

	void setStyle( chtype sty )    { NCattribute::setStyle( *attr_p, sty ); }

	void setChar( chtype sty )     { NCattribute::setChar( *attr_p, sty ); }
    };

    // Retrieve appropriate chtype from NCstyle
    const chtype & attr( NCstyle::STglobal a )
    {
	return NCstyle_C.getStyle( NCstyle_C.fakestyle_e ).attr( a );
    }

    const chtype & attr( NCstyle::STlocal a )
    {
	return NCstyle_C.getStyle( NCstyle_C.fakestyle_e ).attr( a );
    }

    // Build the Aset std::vector associated with a SetType and assing it to struct Wchattr.
    void doshowset( SetType a, bool reset = false );

    static void showset( SetType a )
    {
	NCStyleDef_p->doshowset( a );
    }

    // queryCharEnt serves as menu entry for the ACS char popoup in queryChar()
    // queryChar(). Used in struct Wchattr.

    struct queryCharEnt
    {
	std::string l;
	chtype c;
	queryCharEnt( std::string L, chtype C ) { l = L; c = C; }
    };

    static chtype queryChar( int column = 0, chtype selbg = A_REVERSE );

    // Show details of Wchattr's current item (fg/bg and attributes)

    void doshowstat( const Aset & a ) { wChstat.stat( a ); }

    static void showstat( const Aset & a )
    {
	NCStyleDef_p->doshowstat( a );
    }

    static const char * dumpColor( short c )
    {
	switch ( c )
	{
	    case -1:
		return "-1";
		break;

	    case COLOR_BLACK:
		return "bk";
		break;

	    case COLOR_RED:
		return "re";
		break;

	    case COLOR_GREEN:
		return "gr";
		break;

	    case COLOR_YELLOW:
		return "ye";
		break;

	    case COLOR_BLUE:
		return "bl";
		break;

	    case COLOR_MAGENTA:
		return "ma";
		break;

	    case COLOR_CYAN:
		return "cy";
		break;

	    case COLOR_WHITE:
		return "wh";
		break;
	}

	return "??";
    }


    /**
     * SubWin: base class for the windows used
     **/
    struct SubWin
    {
	static const int taglen = 7;
	NCursesWindow w;
	std::string	tag;
	chtype		changestyle;
	SubWin( std::string T, NCursesWindow & P, int H, int W, int L, int C )
		: w( P, H, W, L, C, 'r' )
		, tag( std::string( "<" ) + T + ">" )
	{
	    changestyle = A_NORMAL;
	}

	virtual ~SubWin() {}

	virtual void draw( bool immediate = false )
	{
	    w.syncup();

	    if ( immediate )
		refresh();
	}

	virtual int dtag()
	{
	    if ( tag.size() > 2 )
	    {
		w.printw( 0, 0, "%-*.*s", taglen, taglen, tag.c_str() );
		return w.width() - taglen;
	    }

	    w.move( 0, 0 );

	    return w.width();
	}

	virtual int change()
	{
	    enterchange();
	    bool continue_bi = true;
	    int	 in	     = -1;

	    do
	    {
		switch (( in = getch() ) )
		{
		    case -1:
		    case KEY_ESC:
		    case KEY_TAB:
		    case KEY_RETURN:
			in = -1; // no reconsume

		    case KEY_F( 1 ):
		    case KEY_F( 2 ):
		    case KEY_F( 3 ):
		    case KEY_F( 4 ):
		    case KEY_F( 5 ):
		    case KEY_F( 6 ):
		    case KEY_F( 7 ):
		    case KEY_F( 8 ):
		    case KEY_F( 9 ):
		    case KEY_F( 10 ):
			continue_bi = false;
			break;

		    default:
			handle( in );
			break;
		}
	    }
	    while ( continue_bi );

	    leavechange();

	    return in;
	}

	virtual void enterchange()
	{
	    changestyle = A_REVERSE;
	    w.bkgd( changestyle );
	    draw( true );
	}

	virtual void leavechange()
	{
	    changestyle = A_NORMAL;
	    w.bkgd( changestyle );
	    draw( true );
	}

	virtual void handle( int in )
	{
	}
    };


    /**
     * Wstyle: Selection of the current NCstyle::StyleSet to process
     **/
    struct Wstyle : public SubWin
    {
	NCstyle::StyleSet cset;
	Wstyle( std::string T, NCursesWindow & P, int H, int W, int L, int C )
		: SubWin( T, P, H, W, L, C )
	{
	    cset = ( NCstyle::StyleSet )( 0 );
	}

	virtual void draw( bool immediate = false )
	{
	    int len = dtag();
	    w.printw( "%-*.*s", len, len, NCstyle::dumpName( cset ).c_str() );

	    if ( cset == 0 )
		w.addch( 0, 2, '-' );

	    if ( cset ==  NCstyle::MaxStyleSet - 1 )
		w.addch( 0, 4, '-' );

	    SubWin::draw( immediate );
	}

	virtual void handle( int in )
	{
	    switch ( in )
	    {
		case KEY_UP:

		    if ( cset > 0 )
		    {
			cset = ( NCstyle::StyleSet )( cset - 1 );
			draw( true );
			fakestyle( cset );
		    }
		    break;

		case KEY_DOWN:

		    if ( cset + 1 < NCstyle::MaxStyleSet )
		    {
			cset = ( NCstyle::StyleSet )( cset + 1 );
			draw( true );
			fakestyle( cset );
		    }
		    break;
	    }
	}
    };


    /**
     * Wset: Selection of the current attribute std::set to process
     **/
    struct Wset : public SubWin
    {
	SetType cset;
	Wset( std::string T, NCursesWindow & P, int H, int W, int L, int C )
		: SubWin( T, P, H, W, L, C )
	{
	    cset = ( SetType )( 0 );
	}

	virtual void draw( bool immediate = false )
	{
	    int len = dtag();
	    w.printw( "%-*.*s", len, len, dumpName( cset ).c_str() );

	    if ( cset == 0 )
		w.addch( 0, 2, '-' );

	    if ( cset ==  MaxSetType - 1 )
		w.addch( 0, 4, '-' );

	    SubWin::draw( immediate );
	}

	virtual void handle( int in )
	{
	    switch ( in )
	    {
		case KEY_UP:

		    if ( cset > 0 )
		    {
			cset = ( SetType )( cset - 1 );
			draw( true );
			showset( cset );
		    }

		    break;

		case KEY_DOWN:

		    if ( cset + 1 < MaxSetType )
		    {
			cset = ( SetType )( cset + 1 );
			draw( true );
			showset( cset );
		    }

		    break;
	    }
	}
    };


    /**
     * Wchattr: handle modification of the current attribute std::set.
     **/
    struct Wchattr : public SubWin
    {
	std::vector<Aset> aset;
	unsigned     fitem;
	unsigned     citem;
	Wchattr( std::string T, NCursesWindow & P, int H, int W, int L, int C )
		: SubWin( T, P, H, W, L, C )
	{
	    fitem = citem = 0;
	}

	virtual void draw( bool immediate = false )
	{
	    w.box();
	    dtag();
	    drawTable( immediate );
	}

	virtual void handle( int in )
	{
	    if ( !aset.size() )
		return;

	    bool redraw	   = true;

	    bool redrawall = true;

	    switch ( in )
	    {
		case KEY_UP:

		    if ( citem )
		    {
			--citem;
			redrawall = false;
		    }

		    break;

		case KEY_DOWN:

		    if ( citem + 1 < aset.size() )
		    {
			++citem;
			redrawall = false;
		    }

		    break;

		case KEY_LEFT:
		case KEY_RIGHT:
		    movepad( in );
		    break;

		case KEY_PPAGE:
		    aset[citem].setBg( true );
		    break;

		case KEY_NPAGE:
		    aset[citem].setBg( false );
		    break;

		case KEY_HOME:
		    aset[citem].setFg( true );
		    break;

		case KEY_END:
		    aset[citem].setFg( false );
		    break;

		case KEY_IC:
		case 'b':
		    aset[citem].toggleStyle( A_BOLD );
		    break;

		case KEY_DC:
		case 'n':
		    aset[citem].setStyle( A_NORMAL );
		    break;

		case 'l':
		    aset[citem].toggleStyle( A_BLINK );
		    break;

		case 's':
		    aset[citem].toggleStyle( A_STANDOUT );
		    break;

		case 'u':
		    aset[citem].toggleStyle( A_UNDERLINE );
		    break;

		case 'r':
		    aset[citem].toggleStyle( A_REVERSE );
		    break;

		case 'd':
		    aset[citem].toggleStyle( A_DIM );
		    break;

		case 'i':
		    aset[citem].toggleStyle( A_INVIS );
		    break;

		case 'a':
		    {
			chtype ach = queryChar( pad().begx() + 5, aset[citem].textattr() );

			if ( ach != ( chtype ) - 1 )
			    aset[citem].setChar( ach );
		    }

		    break;

		default:
		    redraw = redrawall = false;
		    break;
	    }

	    if ( redraw )
		drawTable( true );

	    if ( redrawall )
		attrchanged();
	}

	void set( std::vector<Aset> & nset, bool reset = false )
	{
	    aset.swap( nset );
	    draw( true );
	}

	void drawTable( bool immediate = false )
	{
	    unsigned t	  = w.height() - 1;
	    unsigned high = ( t - 1 );
	    unsigned spot = high / 2;
	    unsigned l = 1;

	    if ( !aset.size() )
		citem = 0;
	    else if ( citem >= aset.size() )
		citem = aset.size() - 1;

	    if ( high >= aset.size() || citem <= spot )
	    {
		fitem = 0;
	    }
	    else
	    {
		fitem = citem - spot;

		if ( fitem + high >= aset.size() )
		    fitem = aset.size() - high;
	    }

	    for ( unsigned i = fitem; l < t; ++i, ++l )
	    {
		drawItemAt( l, i );
	    }

	    if ( !aset.size() )
	    {
		int len = w.width() - 2;
		w.bkgdset( changestyle );
		w.printw( 1, 1, "%-*.*s", len, len, "<empty>" );
	    }

	    w.bkgdset( changestyle );

	    SubWin::draw( immediate );
	}

	void drawItemAt( unsigned line, unsigned num )
	{
	    int len = w.width() - 4;

	    if ( num < aset.size() )
	    {
		w.bkgdset( changestyle );

		if ( num == citem )
		{
		    w.addstr( line, 1, "->" );
		    showstat( aset[num] );
		}
		else
		{
		    w.addstr( line, 1, "  " );
		}

		w.bkgdset( aset[num].attr() );

		w.printw( line, 3, "%-*.*s", len, len, "" );
		w.bkgdset( aset[num].textattr() );
		w.addstr( line, 3, aset[num].label.c_str(), len );
	    }
	    else
	    {
		w.bkgdset( changestyle );
		w.printw( line, 1, "%-*.*s", len + 2, len + 2, "" );
	    }
	}
    };


    /**
     * Wchstat: show current attributes definition
     **/
    struct Wchstat : public SubWin
    {
	Wchstat( std::string T, NCursesWindow & P, int H, int W, int L, int C )
		: SubWin( T, P, H, W, L, C )
	{
	}

	virtual int change() { return -1; }

	virtual void draw( bool immediate = false )
	{
	    w.bkgdset( A_NORMAL );
	    w.clear();
	    SubWin::draw( immediate );
	}

	void stat( const Aset & a )
	{
	    static char buf[1024];
	    int len = w.width() - 2;
	    chtype ch = a.attr();
	    w.bkgdset( a.attr() );
	    w.clear();
	    w.bkgdset( a.textattr() );
	    w.box();
	    w.addstr( 0, 1, a.label.c_str(), len );
	    sprintf( buf, "%s/%s %c%c%c%c%c%c%c %c%3u",
		     dumpColor( NCattribute::fg_color_of( ch ) ),
		     dumpColor( NCattribute::bg_color_of( ch ) ),
		     ( ch&A_INVIS ?	 'i' : '.' ),
		     ( ch&A_BOLD ?	 'b' : '.' ),
		     ( ch&A_DIM ?	 'd' : '.' ),
		     ( ch&A_BLINK ?	 'l' : '.' ),
		     ( ch&A_REVERSE ?	 'r' : '.' ),
		     ( ch&A_UNDERLINE ?	 'u' : '.' ),
		     ( ch&A_STANDOUT ?	 's' : '.' ),
		     ( ch&A_ALTCHARSET ? 'A' : '.' ),
		     ( unsigned )( ch&A_CHARTEXT )
		   );
	    w.addstr( 1, 1, buf, len );
	    SubWin::draw( true );
	}
    };


    /**
     * Wex: popup and down the example Dialog
     **/
    struct Wex : public SubWin
    {
	SetType cset;
	Wex( std::string T, NCursesWindow & P, int H, int W, int L, int C )
		: SubWin( T, P, H, W, L, C )
	{
	    showex( EX_OFF );
	}

	virtual void draw( bool immediate = false )
	{
	    int len = dtag();
	    w.printw( "%-*.*s", len, len, "Example Dialog" );
	    SubWin::draw( immediate );
	}

	virtual int change()
	{
	    showex( EX_TOGGLE );
	    return -1;
	}
    };


    //
    // Construct the panel and widgets
    //

    Wstyle  wStyle;
    Wset    wSet;
    Wchstat wChstat;
    Wchattr wChattr;
    Wex	    wEx;

    NCStyleDef( NCstyle & style )
	    : NCstyle_C( style )
	    , p( 19, 30, NCurses::lines() - 19, 1 )
	    , wStyle( "F1/2", p,  1, 28,  1, 1 )
	    , wSet( "F3/4", p,	1, 28,	2, 1 )
	    , wChstat( "",     p,  3, 28,  3, 1 )
	    , wChattr( "",     p, 11, 28,  6, 1 )
	    , wEx( "F6",   p,  1, 28, 17, 1 )
    {
	NCStyleDef_p = this;
	pbox();
	wStyle.draw();
	wSet.draw();
	wChstat.draw();
	wChattr.draw();
	wEx.draw();
    }

    ~NCStyleDef() { NCStyleDef_p = 0; }

    void changeStyle();

    void saveStyle();

    // that's the way the chtype is saved to file
    std::ostream & dumpChtype( std::ostream & str, const chtype & ch )
    {
	static chtype mask = A_STANDOUT | A_UNDERLINE | A_REVERSE | A_BLINK | A_DIM | A_BOLD | A_INVIS;
	chtype base  = ch & ~mask;
	chtype style = ch & mask;
#define PRTIF(S) if ( style & S ) str << "|" << #S
	str << base;
	PRTIF( A_STANDOUT );
	PRTIF( A_UNDERLINE );
	PRTIF( A_REVERSE );
	PRTIF( A_BLINK );
	PRTIF( A_DIM );
	PRTIF( A_BOLD );
	PRTIF( A_INVIS );
#undef PRTIF
	return str;
    }

    void restoreStyle();


    /**
     * helper struct to std::map strings to NCstyle enum values
     **/
    struct lookupIdx
    {
	NCstyle::STglobal glob;
	NCstyle::STlocal  loc;
	lookupIdx()			 { glob = NCstyle::MaxSTglobal; loc = NCstyle::MaxSTlocal; }

	lookupIdx( NCstyle::STglobal g ) { glob = g; loc = NCstyle::MaxSTlocal; }

	lookupIdx( NCstyle::STlocal  l ) { glob = NCstyle::MaxSTglobal; loc = l; }

	bool isLoc()	 const { return glob == NCstyle::MaxSTglobal && loc != NCstyle::MaxSTlocal; }

	bool isGlob()	 const { return glob != NCstyle::MaxSTglobal && loc == NCstyle::MaxSTlocal; }

	bool isUnknown() const { return glob == NCstyle::MaxSTglobal && loc == NCstyle::MaxSTlocal; }

	unsigned     uindex() const { if ( isLoc() ) return loc; return isGlob() ? glob : ( unsigned ) - 1; }

	const char * stat()   const { if ( isLoc() ) return "L"; return isGlob() ? "G"	: "?"; }
    };

    void showHelp();
};


NCStyleDef * NCStyleDef::NCStyleDef_p = 0;



void NCStyleDef::doshowset( SetType a, bool reset )
{
    std::vector<Aset> aset;

    switch ( a )
    {
#define PRT(T) aset.push_back( Aset( const_cast<chtype&>( attr( NCstyle::T ) ), #T ) )

	case Global:
	    PRT( AppTitle );
	    PRT( AppText );
	    break;

	case DialogBorder:
	    PRT( DialogBorder );
	    PRT( DialogTitle );
	    break;

	case DialogBorderActive:
	    PRT( DialogActiveBorder );
	    PRT( DialogActiveTitle );
	    break;

	case DialogText:
	    PRT( DialogText );
	    PRT( DialogHeadline );
	    PRT( DialogDisabled );
	    break;

	case Widget:
	    PRT( DialogPlain );
	    PRT( DialogLabel );
	    PRT( DialogData );
	    PRT( DialogHint );
	    PRT( DialogScrl );
	    break;

	case WidgetActive:
	    PRT( DialogActivePlain );
	    PRT( DialogActiveLabel );
	    PRT( DialogActiveData );
	    PRT( DialogActiveHint );
	    PRT( DialogActiveScrl );
	    break;

	case FrameWidget:
	    PRT( DialogFramePlain );
	    PRT( DialogFrameLabel );
	    PRT( DialogFrameData );
	    PRT( DialogFrameHint );
	    PRT( DialogFrameScrl );
	    break;

	case FrameWidgetActive:
	    PRT( DialogActiveFramePlain );
	    PRT( DialogActiveFrameLabel );
	    PRT( DialogActiveFrameData );
	    PRT( DialogActiveFrameHint );
	    PRT( DialogActiveFrameScrl );
	    break;

	case List:
	    PRT( ListTitle );
	    PRT( ListPlain );
	    PRT( ListLabel );
	    PRT( ListData );
	    PRT( ListHint );
	    PRT( ListSelPlain );
	    PRT( ListSelLabel );
	    PRT( ListSelData );
	    PRT( ListSelHint );
	    break;

	case ListActive:
	    PRT( ListActiveTitle );
	    PRT( ListActivePlain );
	    PRT( ListActiveLabel );
	    PRT( ListActiveData );
	    PRT( ListActiveHint );
	    PRT( ListActiveSelPlain );
	    PRT( ListActiveSelLabel );
	    PRT( ListActiveSelData );
	    PRT( ListActiveSelHint );
	    break;

	case RichText:
	    PRT( RichTextPlain );
	    PRT( RichTextTitle );
	    PRT( RichTextLink );
	    PRT( RichTextArmedlink );
	    PRT( RichTextActiveArmedlink );
	    PRT( RichTextVisitedLink );
	    PRT( RichTextB );
	    PRT( RichTextI );
	    PRT( RichTextT );
	    PRT( RichTextBI );
	    PRT( RichTextBT );
	    PRT( RichTextIT );
	    PRT( RichTextBIT );
	    break;

	case ProgressBar:
	    PRT( ProgbarCh );
	    PRT( ProgbarBgch );
	    PRT( TextCursor );
	    break;

	case MaxSetType:
	    break;
#undef PRT
    }

    wChattr.set( aset, reset );
}



void NCStyleDef::pbox( bool on )
{
    p.bkgdset( A_NORMAL );
    p.box();

    if ( on )
    {
	p.bkgdset( A_REVERSE );
	p.addstr( 0, 1, "<ENTER> done " );
    }
    else
	p.addstr( 0, 1, "<F5> move pad " );

    p.bkgdset( A_NORMAL );
}



int NCStyleDef::movePad( int key )
{
    pbox( true );
    p.show();

    bool continue_bi = true;
    int in = -1;

    do
    {
	refresh();

	if ( key == -1 )
	    in = getch();
	else
	    in = key;

	switch ( in )
	{
	    case KEY_UP:

		if ( p.begy() > 0 )
		{
		    p.mvwin( p.begy() - 1, p.begx() );
		}
		break;

	    case KEY_DOWN:

		if ( p.begy() + p.height() < NCurses::lines() )
		{
		    p.mvwin( p.begy() + 1, p.begx() );
		}
		break;

	    case KEY_LEFT:

		if ( p.begx() > 0 )
		{
		    p.mvwin( p.begy(), p.begx() - 1 );
		}
		break;

	    case KEY_RIGHT:

		if ( p.begx() + p.width() < NCurses::cols() )
		{
		    p.mvwin( p.begy(), p.begx() + 1 );
		}
		break;

	    case -1:
	    case KEY_ESC:
	    case KEY_TAB:
	    case KEY_RETURN:
	    case KEY_F( 1 ):
	    case KEY_F( 2 ):
	    case KEY_F( 3 ):
	    case KEY_F( 4 ):
	    case KEY_F( 5 ):
	    case KEY_F( 6 ):
	    case KEY_F( 7 ):
	    case KEY_F( 8 ):
	    case KEY_F( 9 ):
	    case KEY_F( 10 ):
		continue_bi = false;
		break;
	}

    }
    while ( continue_bi && key == -1 );

    pbox( false );

    if ( key != -1 )
	in = -1;

    return in;
}



void NCStyleDef::changeStyle()
{
    fakestyle( wStyle.cset );
    p.show();

    bool continue_bi = true;
    int in   = -1;
    int rein =	1;

    do
    {
	refresh();

	if ( rein != -2 )
	{
	    in = rein;
	    rein = -1;
	}
	else
	{
	    in = getch();
	}

	switch ( in )
	{
	    case KEY_F( 1 ):
		wStyle.handle( KEY_UP );
		break;

	    case KEY_F( 2 ):
		wStyle.handle( KEY_DOWN );
		break;

	    case KEY_F( 3 ):
		wSet.handle( KEY_UP );
		break;

	    case KEY_F( 4 ):
		wSet.handle( KEY_DOWN );
		break;

	    case KEY_F( 5 ):
		rein = movePad();
		break;

	    case KEY_F( 6 ):
		rein = wEx.change();
		break;

	    case KEY_F( 8 ):
		restoreStyle();
		break;

	    case KEY_F( 9 ):
		saveStyle();
		break;

	    case KEY_ESC:
	    case KEY_F( 10 ):
		continue_bi = false;
		break;

	    default:
		rein = wChattr.change();
		break;
	}
    }
    while ( continue_bi );

    p.hide();

    p.refresh();

    fakestyle( NCstyle::MaxStyleSet );
}



// query popup for ACS chars
chtype NCStyleDef::queryChar( int column, chtype selbg )
{
    std::vector<queryCharEnt> men;
    men.push_back( queryCharEnt( "NO CHAR", ' ' ) );
    men.push_back( queryCharEnt( "BLANK", ' ' ) );
#define PUT(a) men.push_back( queryCharEnt( #a, a ) );
    PUT( ACS_CKBOARD );
    PUT( ACS_BOARD );
    PUT( ACS_BLOCK );
    PUT( ACS_DIAMOND );
    PUT( ACS_BULLET );
    PUT( ACS_DEGREE );
    PUT( ACS_PLMINUS );
    PUT( ACS_LEQUAL );
    PUT( ACS_GEQUAL );
    PUT( ACS_NEQUAL );
    PUT( ACS_S1 );
    PUT( ACS_S3 );
    PUT( ACS_S7 );
    PUT( ACS_S9 );
    PUT( ACS_PI );
    PUT( ACS_LANTERN );
    PUT( ACS_STERLING );
    PUT( ACS_LARROW );
    PUT( ACS_RARROW );
    PUT( ACS_DARROW );
    PUT( ACS_UARROW );
    PUT( ACS_ULCORNER );
    PUT( ACS_URCORNER );
    PUT( ACS_LLCORNER );
    PUT( ACS_LRCORNER );
    PUT( ACS_VLINE );
    PUT( ACS_LTEE );
    PUT( ACS_RTEE );
    PUT( ACS_BTEE );
    PUT( ACS_TTEE );
    PUT( ACS_HLINE );
    PUT( ACS_PLUS );
#undef PUT

    chtype defbg = A_NORMAL;
    unsigned lrow = men.size() - men.size() / 2;

    NCursesPanel popup( lrow + 2, 39, 1, 1 );
    popup.bkgd( defbg );
    popup.box();

    popup.show();
    chtype   ret = ( chtype ) - 1;
    unsigned idx = 0;
    int	     in	 = -1;

    do
    {
	int l = 1;
	int c = 1;
	int len = 13;

	for ( unsigned i = 0; i < men.size(); ++i, ++l )
	{
	    if ( i == lrow )
	    {
		c += len + 6;
		l = 1;
	    }

	    popup.bkgdset( defbg );

	    popup.addstr( l, c, ( i == idx ? "->" : "  " ) );
	    popup.bkgdset( i == idx ? selbg : defbg );
	    popup.addch( ' ' );
	    popup.addch( men[i].c );
	    popup.printw( " %-*.*s", len, len, men[i].l.c_str() );
	}

	popup.refresh();

	switch ( ( in = getch() ) )
	{

	    case KEY_UP:

		if ( idx )
		    --idx;
		break;

	    case KEY_DOWN:

		if ( idx + 1 < men.size() )
		    ++idx;
		break;

	    case KEY_RETURN:
		ret = idx ? men[idx].c : 0;

	    case KEY_ESC:
		in = -1;

		break;
	}
    }
    while ( in != -1 );

    popup.hide();
    popup.refresh();

    return ret;
}


#define BGSET(a) W.bkgdset( st.a )

static int hi = 4;
static int wi = 15;


inline void frame( NCursesWindow & w, int l, int c, int H = 0, int W = 0 )
{
    if ( !H )
	H = hi;

    if ( !W )
	W = wi;

    w.vline( l, c, H );
    w.vline( l, c + W, H );
    w.hline( l, c, W );
    w.hline( l + H, c, W );

    w.addch( l, c, ACS_ULCORNER );
    w.addch( l + H, c, ACS_LLCORNER );
    w.addch( l, c + W, ACS_URCORNER );
    w.addch( l + H, c + W, ACS_LRCORNER );
}


inline void laex( NCursesWindow & W, const char * T, NCstyle::StItem st )
{
    BGSET( hint );
    W.addch( *T );
    BGSET( label );
    W.addstr( T + 1 );
}


inline void itex( NCursesWindow & W, const char * T, NCstyle::StItem st )
{
    BGSET( plain );
    W.addstr( "(" );
    BGSET( data );
    W.addstr( "X" );
    BGSET( plain );
    W.addstr( ") " );
    laex( W, T, st );
}


inline void butex( NCursesWindow & W, int L, int C, const char * T, NCstyle::StWidget st )
{
    W.move( L, C );
    itex( W, T, st );
    BGSET( scrl );
    W.addch( ACS_DARROW );
}


inline void frameex( NCursesWindow & W, int L, int C, const char * T, NCstyle::StWidget st )
{
    BGSET( plain );
    frame( W, L, C );
    W.move( L, C + 1 );
    laex( W, T, st );
    BGSET( scrl );
    W.addch( L + hi, C + 1, ACS_LTEE );
    W.addch( ACS_HLINE );
    W.addch( ACS_RTEE );
    W.addch( L + 1, C + wi, ACS_TTEE );
    W.addch( L + 2, C + wi, ACS_BTEE );
}


inline void listex( NCursesWindow & W, int L, int C, NCstyle::StList st )
{
    BGSET( title );
    W.printw( L, C, "%-*s", wi - 1, "Title" );
    ++L;
    BGSET( item.plain );
    W.printw( L, C, "%-*s", wi - 1, "" );
    W.move( L, C );
    itex( W, "Item", st.item );
    ++L;
    BGSET( selected.plain );
    W.printw( L, C, "%-*s", wi - 1, "" );
    W.move( L, C );
    itex( W, "Selected", st.selected );
}


inline void widex( NCursesWindow & W, int L, int C, const char * T,
		   NCstyle::StWidget w, NCstyle::StWidget f, NCstyle::StList l )
{
    butex( W, L, C, T, w );
    L += 2;
    frameex( W, L, C, T, f );
    L += 1;
    listex( W, L, C + 1, l );
}



void NCStyleDef::showex( ExMode mode )
{
    static NCursesPanel W( 14, NCurses::cols(), 0, 0 );

    if ( mode == EX_TOGGLE )
	mode = W.hidden() ? EX_ON : EX_OFF;

    if ( mode == EX_OFF )
    {
	if ( !W.hidden() )
	{
	    W.hide();
	    W.refresh();
	}

	return;
    }

    if ( mode == EX_ON && W.hidden() )
    {
	W.show();
    }

    if ( W.hidden() )
	return;

    const NCstyle::Style & st( NCStyleDef_p->NCstyle_C.getStyle( NCStyleDef_p->NCstyle_C.fakestyle_e ) );

    BGSET( dumb.text );
    W.clear();
    BGSET( border.text );
    W.box();
    BGSET( border.title );
    W.addstr( 0, 1, "normal dialog" );
    int l = 1;
    int c = 1;
    BGSET( activeBorder.text );
    frame( W, l, c, W.height() - 2, W.width() - 2 );
    BGSET( activeBorder.title );
    W.addstr( l, c + 1, "active dialog" );
    l = 3;
    c = 3;
    BGSET( dumb.title );
    W.addstr( l, c, "Heading" );
    BGSET( dumb.text );
    frame( W, l + 2, c );
    W.addstr( l + 2, c + 1, "Frame" );
    W.addstr( l + 4, c + 1, "Some text" );
    c += 20;
    widex( W, l, c, "Disabled", st.disabled, st.disabled, st.disabledList );
    c += 20;
    widex( W, l, c, "Normal", st.normal, st.frame, st.list );
    c += 20;
    widex( W, l, c, "Active", st.active, st.activeFrame, st.activeList );
    l += hi + 4;
    c = 3;
    BGSET( progbar.bar.chattr );
    W.addstr( l, c, "	   " );
    BGSET( progbar.bar.getNonChar() );
    W.addstr( "50" );
    BGSET( progbar.nonbar.getNonChar() );
    W.addstr( "%" );
    BGSET( progbar.nonbar.chattr );
    W.addstr( "	      " );
    W.refresh();
}


void NCStyleDef::saveStyle()
{
    std::string fname( "NCstyle." + NCstyle_C.styleName + ".h" );
    std::string hname( "NCstyle_" + NCstyle_C.styleName + "_h" );
    std::string fpath( "/tmp/" );
    fpath += fname;

    std::ofstream out( fpath.c_str(), std::ios::out );
    NCursesPanel  p( 5, NCurses::cols() - 4, ( NCurses::lines() - 5 ) / 2, 2 );
    int pl = p.height() / 2;
    p.box();

    if ( !out.good() )
    {
	p.bkgd( NCattribute::color_pair( COLOR_WHITE, COLOR_RED ) );
	p.printw( pl, 2, "Can't open output file \"%s\"!", fpath.c_str() );
	p.show();
	p.refresh();
	getch();
	p.hide();
	p.refresh();
	return;
    }

    p.bkgd( NCattribute::color_pair( COLOR_CYAN, COLOR_BLACK ) );

    p.printw( pl, 2, "Saving style \"%s\" in \"%s\" ...", NCstyle_C.styleName.c_str(), fpath.c_str() );
    p.show();
    p.refresh();

    out << "/*" << std::endl;
    out << "  Copyright (C) 2000-2012 Novell, Inc" << std::endl;
    out << "  This library is free software; you can redistribute it and/or modify" << std::endl;
    out << "  it under the terms of the GNU Lesser General Public License as" << std::endl;
    out << "  published by the Free Software Foundation; either version 2.1 of the" << std::endl;
    out << "  License, or (at your option) version 3.0 of the License. This library" << std::endl;
    out << "  is distributed in the hope that it will be useful, but WITHOUT ANY" << std::endl;
    out << "  WARRANTY; without even the implied warranty of MERCHANTABILITY or" << std::endl;
    out << "  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public" << std::endl;
    out << "  License for more details. You should have received a copy of the GNU" << std::endl;
    out << "  Lesser General Public License along with this library; if not, write" << std::endl;
    out << "  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth" << std::endl;
    out << "  Floor, Boston, MA 02110-1301 USA" << std::endl;
    out << "*/" << std::endl;
    out << std::endl;
    out << std::endl;
    out << "/*-/" << std::endl;
    out << std::endl;
    out << "   File:       " << fname << std::endl;
    out << std::endl;
    out << "   Author:     Generated by class NCstyle" << std::endl;
    out << std::endl;
    out << "/-*/" << std::endl;
    out << "#ifndef " << hname << std::endl;
    out << "#define " << hname << std::endl;
    out << "" << std::endl;
    out << "#include \"NCstyle.h\"" << std::endl;
    out << "" << std::endl;
    out << "inline void NCstyleInit_" << NCstyle_C.styleName << "( std::vector<NCstyle::Style> & styleSet )" << std::endl;
    out << "{" << std::endl;

    out << "  //=================================================================" << std::endl;
    out << "  // init global attributes" << std::endl;
    out << "  //=================================================================" << std::endl;
    out << "  NCattrset * attrset( &styleSet[NCstyle::" << NCstyle::dumpName( NCstyle::DefaultStyle ) << "].getAttrGlobal() );" << std::endl;

    for ( NCstyle::STglobal a = ( NCstyle::STglobal )0; a < NCstyle::MaxSTglobal; a = ( NCstyle::STglobal )( a + 1 ) )
    {
	out << "  attrset->setAttr( NCstyle::"
	<< NCstyle::dumpName( a )
	<< ", ";
	dumpChtype( out, NCstyle_C( a ) )
	<< " );" << std::endl;
    }

    out << "  //=================================================================" << std::endl;
    out << "  // init local attributes" << std::endl;
    out << "  //=================================================================" << std::endl;

    for ( NCstyle::StyleSet sts = ( NCstyle::StyleSet )0; sts < NCstyle::MaxStyleSet; sts = ( NCstyle::StyleSet )( sts + 1 ) )
    {
	out << "  // " << NCstyle::dumpName( sts ) << std::endl;
	out << "  attrset = &styleSet[NCstyle::" << NCstyle::dumpName( sts ) << "].getAttrLocal();"  << std::endl;

	for ( NCstyle::STlocal a = ( NCstyle::STlocal )0; a < NCstyle::MaxSTlocal; a = ( NCstyle::STlocal )( a + 1 ) )
	{
	    out << "  attrset->setAttr( NCstyle::"
	    << NCstyle::dumpName( a )
	    << ", ";
	    dumpChtype( out, NCstyle_C.styleSet[sts]( a ) )
	    << " );" << std::endl;
	}
    }

    out << "}" << std::endl;
    out << "#endif // " << hname << std::endl;
    out.close();

    if ( ! out.good() )
    {
	p.bkgd( NCattribute::color_pair( COLOR_WHITE, COLOR_RED ) );
	p.printw( " FAILED!" );
    }
    else
    {
	p.printw( " DONE!" );

    }

    p.show();

    p.refresh();
    getch();
    p.hide();
    p.refresh();
}


void NCStyleDef::restoreStyle()
{
    std::string fname( "NCstyle." + NCstyle_C.styleName + ".h" );
    std::string fpath( "/tmp/" );
    fpath += fname;

    std::ifstream inp( fpath.c_str(), std::ios::in );
    int ph = 5;
    NCursesPanel  p( ph, NCurses::cols() - 4, ( NCurses::lines() - ph ) / 2, 2 );
    int pl = 2;
    p.box();

    if ( !inp.good() )
    {
	p.bkgd( NCattribute::color_pair( COLOR_WHITE, COLOR_RED ) );
	p.printw( pl, 2, "Can't open input file \"%s\"!", fpath.c_str() );
	p.show();
	p.refresh();
	getch();
	p.hide();
	p.refresh();
	return;
    }

    p.bkgd( NCattribute::color_pair( COLOR_CYAN, COLOR_BLACK ) );

    p.printw( pl, 2, "Reding style \"%s\" from \"%s\" ...", NCstyle_C.styleName.c_str(), fpath.c_str() );
    p.show();
    p.refresh();

    std::list<std::string> data_vec[NCstyle::MaxStyleSet+1];
    NCstyle::StyleSet cvec = NCstyle::MaxStyleSet;

    std::string initfnc_ti( "inline void NCstyleInit_" );
    initfnc_ti += NCstyle_C.styleName + "(";
    std::string attrdef( "  attrset->setAttr( NCstyle::" );
    std::string stydef( "  attrset = &styleSet[NCstyle::" );

    enum STATE { PRE, ONFNC, IN, POST, ERROR };
    STATE psaw = PRE;

    std::string	line( "" );
    unsigned	lineno = 0;

    while ( inp.good() && psaw != POST && psaw != ERROR )
    {
	char c;
	line = "";

	while ( inp.get( c ), inp.good() && c != '\n' )
	    line += c;

	{
	    ++lineno;
	    // consume

	    switch ( psaw )
	    {
		case PRE:

		    if ( line.find( initfnc_ti ) == 0 )
			psaw = ONFNC;

		    break;

		case ONFNC:
		    if ( line == "{" || line.find( "  //" ) == 0 )
			break;

		    if ( line == "  NCattrset * attrset( &styleSet[NCstyle::DefaultStyle].getAttrGlobal() );" )
		    {
			psaw = IN;
			cvec = NCstyle::MaxStyleSet;
			//p.printw( "[GLOBAL]" );
			//p.show();
			//p.refresh();
		    }
		    else
			psaw = ERROR;

		    break;

		case IN:
		    if ( line == "}" )
		    {
			psaw = POST;
			break;
		    }

		    if ( line.find( "  //" ) == 0 )
			break;

		    if ( line.find( attrdef ) == 0 )
		    {
			data_vec[cvec].push_back( line.substr( attrdef.size() ) );
			//p.printw( "." );
		    }
		    else if ( line.find( stydef ) == 0 )
		    {
			line.erase( 0, stydef.size() );

			if ( line.find( "DefaultStyle]" ) == 0 )
			{
			    cvec = NCstyle::DefaultStyle;
			    //p.printw( "[DefaultStyle]" );
			}
			else if ( line.find( "InfoStyle]" ) == 0 )
			{
			    cvec = NCstyle::InfoStyle;
			    //p.printw( "[InfoStyle]" );
			}
			else if ( line.find( "WarnStyle]" ) == 0 )
			{
			    cvec = NCstyle::WarnStyle;
			    //p.printw( "[WarnStyle]" );
			}
			else if ( line.find( "PopupStyle]" ) == 0 )
			{
			    cvec = NCstyle::PopupStyle;
			    //p.printw( "[PopupStyle]" );
			}
			else
			{
			    psaw = ERROR;
			}

			p.show();

			p.refresh();
		    }
		    else
		    {
			psaw = ERROR;
		    }

		    break;

		case ERROR:

		case POST:
		    break;
	    }
	}
    }

    if ( psaw != POST )
    {
	p.bkgd( NCattribute::color_pair( COLOR_WHITE, COLOR_RED ) );
	p.printw( " FAILED stage %d!\n[%d]>>%s<<", psaw, lineno, line.c_str() );
	p.show();
	p.refresh();
	getch();
	p.hide();
	p.refresh();
	return;
    }

    inp.close();

    // parse data

    std::vector<NCattrset> attr_vec;

    for ( cvec = ( NCstyle::StyleSet )0; cvec <= NCstyle::MaxStyleSet; cvec = ( NCstyle::StyleSet )( cvec + 1 ) )
    {
	attr_vec.push_back( cvec == NCstyle::MaxStyleSet ? NCattrset( NCstyle::MaxSTglobal ) : NCattrset( NCstyle::MaxSTlocal ) );
    }

    std::map<std::string, lookupIdx> lookupmap;

    for ( NCstyle::STglobal a = ( NCstyle::STglobal )0; a < NCstyle::MaxSTglobal; a = ( NCstyle::STglobal )( a + 1 ) )
    {
	std::map<std::string, lookupIdx>::value_type v( NCstyle::dumpName( a ), lookupIdx( a ) );
	lookupmap.insert( v );
    }

    for ( NCstyle::STlocal a = ( NCstyle::STlocal )0; a < NCstyle::MaxSTlocal; a = ( NCstyle::STlocal )( a + 1 ) )
    {
	std::map<std::string, lookupIdx>::value_type v( NCstyle::dumpName( a ), lookupIdx( a ) );
	lookupmap.insert( v );
    }

    //p.printw( "\n[PARSE]" );
    //p.show();
    //p.refresh();

    // globals first
    for ( cvec = ( NCstyle::StyleSet )( NCstyle::MaxStyleSet + 1 ); cvec > 0; )
    {
	cvec = ( NCstyle::StyleSet )( cvec - 1 );
	//p.printw( "[%s]", NCstyle::dumpName( cvec ).c_str() );
	//p.show();
	//p.refresh();

	for ( std::list<std::string>::iterator i = data_vec[cvec].begin(); i != data_vec[cvec].end(); ++i )
	{
	    std::string::size_type sep = i->find( ", " );

	    if ( sep != std::string::npos )
	    {
		std::string id( i->substr( 0, sep ) );
		std::string val( i->substr( sep + 2 ) );
		sep = val.find( " " );

		if ( sep != std::string::npos )
		{
		    val.erase( sep );
		}

		std::map<std::string, lookupIdx>::const_iterator ldat = lookupmap.find( id );

		if ( ldat == lookupmap.end() || ldat->second.isUnknown() )
		{
		    p.printw( "{UNKNOWN:%s=%s}", id.c_str(), val.c_str() );
		    p.show();
		    p.refresh();
		}
		else
		{
		    chtype ch = atoi( val.c_str() );
		    sep = val.find( "|" );

		    if ( sep != std::string::npos )
		    {
			val.erase( 0, sep + 1 );

			while ( val.size() )
			{
			    sep = val.find( "|" );
			    std::string tt = val.substr( 0, sep );
#define IFASSIGN(T) if ( tt == #T ) ch |= T
			    IFASSIGN( A_STANDOUT );
			    else IFASSIGN( A_UNDERLINE );
			    else IFASSIGN( A_REVERSE );
			    else IFASSIGN( A_BLINK );
			    else IFASSIGN( A_DIM );
			    else IFASSIGN( A_BOLD );
			    else IFASSIGN( A_INVIS );

#undef IFASSIGN
			    val.erase( 0, ( sep != std::string::npos ) ? sep + 1 : sep );
			}
		    }

		    // ready to assign
		    if ( ldat->second.isLoc() )
		    {
			// actual value is local
			if ( cvec == NCstyle::MaxStyleSet )
			{
			    // global data parsed
			    for ( unsigned ii = 0; ii < NCstyle::MaxStyleSet; ++ii )
			    {
				attr_vec[ii].setAttr( ldat->second.uindex(), ch );
			    }
			}
			else
			{
			    // local data parsed
			    attr_vec[cvec].setAttr( ldat->second.uindex(), ch );
			}

			attr_vec[cvec].setAttr( ldat->second.uindex(), ch );
		    }
		    else if ( ldat->second.isGlob() )
		    {
			// actual value is global
			attr_vec[NCstyle::MaxStyleSet].setAttr( ldat->second.uindex(), ch );
		    }
		}

		//p.printw( "." );
		//p.show();
		//p.refresh();
	    }
	    else
	    {
		p.printw( "{NOVAL:%s}", i->c_str() );
		p.show();
		p.refresh();
	    }
	}
    }

    // apply
    cvec = ( NCstyle::StyleSet )0;

    NCstyle_C.getStyle( cvec ).getAttrGlobal() = attr_vec[NCstyle::MaxStyleSet];

    for ( ; cvec < NCstyle::MaxStyleSet; cvec = ( NCstyle::StyleSet )( cvec + 1 ) )
    {
	NCstyle_C.getStyle( cvec ).getAttrLocal() = attr_vec[cvec];
    }

    attrchanged();

    // finish
    p.printw( " DONE!" );
    p.show();
    p.refresh();
    getch();
    p.hide();
    p.refresh();
}


void NCDefineStyle( NCstyle & style )
{
    static NCStyleDef cstyle( style );
    cstyle.changeStyle();
}
