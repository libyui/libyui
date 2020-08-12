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

   File:       ncursesw.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

/*
  Copyright (C) 1989 Free Software Foundation
  written by Eric Newton (newton@rocky.oswego.edu)

  This file is part of the GNU C++ Library.  This library is free
  software; you can redistribute it and/or modify it under the terms of
  the GNU Library General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your
  option) any later version.  This library is distributed in the hope
  that it will be useful, but WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the GNU Library General Public License for more details.
  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free Software
  Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

  modified by Ulrich Drepper  (drepper@karlsruhe.gmd.de)
	  and Anatoly Ivasyuk (anatoly@nick.csh.rit.edu)

  modified by Juergen Pfeifer (Juergen.Pfeifer@T-Online.de)
*/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ncursesw/term.h>
#undef line
#undef columns

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include <yui/YUIException.h>

#include "ncursesw.h"
#include "NCstring.h"


#define COLORS_NEED_INITIALIZATION  -1
#define COLORS_NOT_INITIALIZED	     0
#define COLORS_MONOCHROME	     1
#define COLORS_ARE_REALLY_THERE      2

//
// static class variables
//
long NCursesWindow::count = 0L;
bool NCursesWindow::b_initialized = FALSE;



int
NCursesWindow::printw( const char * fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    char buf[BUFSIZ];
    vsprintf( buf, fmt, args );
    va_end( args );
    return waddstr( w, buf );
}


int
NCursesWindow::printw( int y, int x, const char * fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    int result = wmove( w, y, x );

    if ( result == OK )
    {
	char buf[BUFSIZ];
	vsprintf( buf, fmt, args );
	result = waddstr( w, buf );
    }

    va_end( args );

    return result;
}

int
NCursesWindow::addwstr( int y, int x, const wchar_t * str, int n )
{
    const std::wstring wstr( str );
    std::string out;

    if ( NCstring::terminalEncoding() != "UTF-8" )
    {
	NCstring::RecodeFromWchar( wstr, NCstring::terminalEncoding(), &out );
	return ::mvwaddnstr( w, y, x, out.c_str(), n );
    }
    else
	return ::mvwaddnwstr( w, y, x, ( wchar_t* )str, n );

}


int
NCursesWindow::addwstr( const wchar_t* str, int n )
{
    const std::wstring wstr( str );
    std::string out;

    if ( NCstring::terminalEncoding() != "UTF-8" )
    {
	NCstring::RecodeFromWchar( wstr, NCstring::terminalEncoding(), &out );
	return ::waddnstr( w, out.c_str(), n );
    }
    else
	return ::waddnwstr( w, ( wchar_t* )str, n );
}


int
NCursesWindow::in_wchar( int y, int x, cchar_t *combined )
{
    int ret = mvwin_wch( w, y, x, combined );
    combined->attr = combined->attr & ( A_CHARTEXT | A_ALTCHARSET );

// libncurses6 enables ext_color from struct cchar_t (see curses.h).
// Set ext_color to 0 to respect the settings got from mvwin_wch (bnc#652240).
#ifdef NCURSES_EXT_COLORS
    combined->ext_color = 0;
#endif
    return ret;
}

int
NCursesWindow::in_wchar( cchar_t *combined )
{
    int ret = win_wch( w, combined );
    combined->attr = combined->attr & ( A_CHARTEXT | A_ALTCHARSET );
// libncurses6 enables ext_color from struct cchar_t (see curses.h).
// Set ext_color to 0 to respect the settings got from win_wch (bnc#652240).
#ifdef NCURSES_EXT_COLORS
    combined->ext_color = 0;
#endif
    return ret;
}

int
NCursesWindow::add_attr_char( int y, int x )
{
    int ret = ERR;

    if ( NCstring::terminalEncoding() != "UTF-8" )
    {
	ret = addch( inchar( y, x ) );
    }
    else
    {
	cchar_t combined;
	ret = in_wchar( y, x, &combined );

	if ( ret == OK )
	{
	    ret = add_wch( &combined );
	}
    }

    return ret;
}

int
NCursesWindow::add_attr_char()
{
    int ret = ERR;

    if ( NCstring::terminalEncoding() != "UTF-8" )
    {
	ret = addch( inchar() );
    }
    else
    {
	cchar_t combined;
	ret = in_wchar( &combined );

	if ( ret == OK )
	{
	    ret = add_wch( &combined );
	}
    }

    return ret;
}

void
NCursesWindow::init( void )
{
    // Setting back_color_erase to FALSE was added because of bug #418613.
    // This isn't necessary any longer because the kernel patch which
    // has caused the bug was reverted (in SLES11-GM).
#if 0
    static char * env;
    if (!env && (env = ::getenv("TERM"))) {
	if (::strncmp(env, "linux", 5) == 0)
	    back_color_erase = FALSE;
    }
#endif
    leaveok( 0 );
    keypad( 1 );
    meta( 1 );
}

void
NCursesWindow::err_handler( const char *msg ) const THROWS( NCursesException )
{
    THROW( new NCursesException( msg ) );
}

void
NCursesWindow::initialize()
{
    if ( !b_initialized )
    {
	//::initscr();
	b_initialized = TRUE;

	if ( colorInitialized == COLORS_NEED_INITIALIZATION )
	{
	    colorInitialized = COLORS_NOT_INITIALIZED;
	    useColors();
	}

	::noecho();

	::cbreak();
    }
}

NCursesWindow::NCursesWindow()
	: w(0), alloced(FALSE), par(0), subwins(0), sib(0)
{
    if ( !b_initialized )
	initialize();

    w =  static_cast<WINDOW *>(0);

    init();

    count++;
}

NCursesWindow::NCursesWindow( int lines, int cols, int begin_y, int begin_x )
	: w(0), alloced(TRUE), par(0), subwins(0), sib(0)
{
    if ( !b_initialized )
	initialize();

    if ( lines <= 0 )
	lines = 1;

    if ( cols <= 0 )
	cols = 1;

    if ( lines + begin_y > NCursesWindow::lines() )
	lines = NCursesWindow::lines() - begin_y;

    if ( cols + begin_x > NCursesWindow::cols() )
	cols = NCursesWindow::cols() - begin_x;

    // yuiDebug() << "Lines: " << lines << " Cols: " << cols << " y: " << begin_y << " x: " << begin_x << std::endl;

    w = ::newwin( lines, cols, begin_y, begin_x );

    if ( w == 0 )
    {
	err_handler( "Cannot construct window" );
    }

    init();

    count++;
}

NCursesWindow::NCursesWindow( WINDOW* window )
	: w(0), alloced(FALSE), par(0), subwins(0), sib(0)
{
    if ( !b_initialized )
	initialize();

    w = window ? window : ::stdscr;

    init();

    count++;
}

NCursesWindow::NCursesWindow( NCursesWindow& win, int l, int c,
			      int begin_y, int begin_x, char absrel )
	: w(0), alloced(TRUE), par(0), subwins(0), sib(0)
{
    if ( l <= 0 )
	l = 1;

    if ( c <= 0 )
	c = 1;

    if ( begin_y < 0 )
	begin_y = 0;

    if ( begin_x < 0 )
	begin_x = 0;

    if ( absrel == 'a' ) // absolute origin
    {
	begin_y -= win.begy();
	begin_x -= win.begx();
    }

    if ( l + begin_y > win.height() )
	l = win.height() - begin_y;

    if ( c + begin_x > win.width() )
	c = win.width() - begin_x;

    // Even though we treat subwindows as a tree, the standard curses
    // library needs the `subwin' call to link to the parent in
    // order to correctly perform refreshes, etc.
    // Friendly enough, this also works for pads.
    w = ::derwin( win.w, l, c, begin_y, begin_x );

    if ( w == 0 )
    {
	yuiError() << "NULL subwindow; throw " << wpos( begin_y, begin_x ) << wsze( l, c ) << std::endl;
        YUI_THROW( YUIException( "NULL ncurses lowlevel subwindow" ) );
    }

    // yuiDebug() << "created " << wpos(begin_y, begin_x) << wsze(l, c) << std::endl;

    par = &win;
    sib = win.subwins;
    win.subwins = this;
    count++;
}

NCursesWindow NCursesWindow::Clone()
{
    WINDOW *d = ::dupwin( w );
    NCursesWindow W( d );
    W.subwins = subwins;
    W.sib = sib;
    W.par = par;
    W.alloced = alloced;
    return W;
}

typedef int ( *RIPOFFINIT )( NCursesWindow& );
static RIPOFFINIT R_INIT[5];	   // There can't be more
static int r_init_idx	= 0;
static RIPOFFINIT* prip = R_INIT;


NCursesWindow::NCursesWindow( WINDOW *win, int cols )
{
    w = win;
    assert(( w->_maxx + 1 ) == cols );
    alloced = FALSE;
    subwins = par = sib = 0;
}

int NCursesWindow::ripoff_init( WINDOW *w, int cols )
{
    int res = ERR;

    RIPOFFINIT init = *prip++;

    if ( init )
    {
	NCursesWindow* W = new NCursesWindow( w, cols );
	res = init( *W );
    }

    return res;
}

int NCursesWindow::ripoffline( int ripoff_lines,
			       int ( *init )( NCursesWindow& win ) )
{
    int code = ::ripoffline( ripoff_lines, ripoff_init );

    if ( code == OK && init && ripoff_lines )
    {
	R_INIT[r_init_idx++] = init;
    }

    return code;
}

bool
NCursesWindow::isDescendant( NCursesWindow& win )
{
    for ( NCursesWindow* p = subwins; p != NULL; p = p->sib )
    {
	if ( p == &win )
	    return TRUE;
	else
	{
	    if ( p->isDescendant( win ) )
		return TRUE;
	}
    }

    return FALSE;
}

void
NCursesWindow::kill_subwindows()
{
    for ( NCursesWindow* p = subwins; p != 0; p = p->sib )
    {
	p->kill_subwindows();

	if ( p->alloced )
	{
	    if ( p->w != 0 )
		::delwin( p->w );

	    p->alloced = FALSE;
	}

	p->w = 0; // cause a run-time error if anyone attempts to use...
    }
}


NCursesWindow::~NCursesWindow()
{
    kill_subwindows();

    if ( par != 0 )  // Snip us from the parent's list of subwindows.
    {
	NCursesWindow * win = par->subwins;
	NCursesWindow * trail = 0;

	for ( ;; )
	{
	    if ( win == 0 )
		break;
	    else if ( win == this )
	    {
		if ( trail != 0 )
		    trail->sib = win->sib;
		else
		    par->subwins = win->sib;

		break;
	    }
	    else
	    {
		trail = win;
		win = win->sib;
	    }
	}
    }

    if ( alloced && w != 0 )
	delwin( w );

    if ( alloced )
    {
	--count;

	if ( count == 0 )
	{
	    ::endwin();
	}
	else if ( count < 0 ) // cannot happen!
	{
	    err_handler( "Too many windows destroyed" );
	}
    }
}

// ---------------------------------------------------------------------
// Color stuff
int NCursesWindow::colorInitialized = COLORS_NOT_INITIALIZED;

void
NCursesWindow::useColors( void )
{
    if ( colorInitialized == COLORS_NOT_INITIALIZED )
    {
	if ( b_initialized )
	{
	    if ( ::has_colors() )
	    {
		::start_color();
		colorInitialized = COLORS_ARE_REALLY_THERE;
	    }
	    else
		colorInitialized = COLORS_MONOCHROME;
	}
	else
	    colorInitialized = COLORS_NEED_INITIALIZATION;
    }
}

short
NCursesWindow::getcolor( int getback ) const
{
    short fore, back;

    if ( colorInitialized == COLORS_ARE_REALLY_THERE )
    {
	if ( pair_content( PAIR_NUMBER( w->_attrs ), &fore, &back ) )
	    err_handler( "Can't get color pair" );
    }
    else
    {
	// Monochrome means white on black
	back = COLOR_BLACK;
	fore = COLOR_WHITE;
    }

    return getback ? back : fore;
}

int NCursesWindow::NumberOfColors()
{
    if ( colorInitialized == COLORS_ARE_REALLY_THERE )
	return COLORS;
    else
	return 1; // monochrome (actually there are two ;-)
}

short
NCursesWindow::getcolor() const
{
    if ( colorInitialized == COLORS_ARE_REALLY_THERE )
	return PAIR_NUMBER( w->_attrs );
    else
	return 0; // we only have pair zero
}

int
NCursesWindow::setpalette( short fore, short back, short pair )
{
    if ( colorInitialized == COLORS_ARE_REALLY_THERE )
	return init_pair( pair, fore, back );
    else
	return OK;
}

int
NCursesWindow::setpalette( short fore, short back )
{
    if ( colorInitialized == COLORS_ARE_REALLY_THERE )
	return setpalette( fore, back, PAIR_NUMBER( w->_attrs ) );
    else
	return OK;
}

int
NCursesWindow::setcolor( short pair )
{
    if ( colorInitialized == COLORS_ARE_REALLY_THERE )
    {
	if (( pair < 1 ) || ( pair > COLOR_PAIRS ) )
	    err_handler( "Can't std::set color pair" );

	attroff( A_COLOR );

	attrset( COLOR_PAIR( pair ) );
    }

    return OK;
}

bool NCursesWindow::has_mouse() const
{
    return (( ::has_key( KEY_MOUSE ) || ::has_mouse() )
	    ? TRUE : FALSE );
}

NCursesPad::NCursesPad( int lines, int cols ) : NCursesWindow()
{
    if ( lines <= 0 )
	lines = 1;

    if ( cols <= 0 )
	cols = 1;

    w = ::newpad( lines, cols );

    if ( w == ( WINDOW* )0 )
    {
	count--;
	err_handler( "Cannot construct window" );
    }

    alloced = TRUE;
}



int NCursesWindow::box( const wrect & dim )
{
    wrect box_area( dim.intersectRelTo( area() ) );

    if ( box_area.Sze > 0 )
    {
	hline( box_area.Pos.L,		    box_area.Pos.C, box_area.Sze.W );
	hline( box_area.Pos.L + box_area.Sze.H - 1, box_area.Pos.C, box_area.Sze.W );
	vline( box_area.Pos.L, box_area.Pos.C,		    box_area.Sze.H );
	vline( box_area.Pos.L, box_area.Pos.C + box_area.Sze.W - 1, box_area.Sze.H );

	addch( box_area.Pos.L + box_area.Sze.H - 1, box_area.Pos.C, ACS_LLCORNER );
	addch( box_area.Pos.L, box_area.Pos.C + box_area.Sze.W - 1, ACS_URCORNER );
	addch( box_area.Pos.L + box_area.Sze.H - 1, box_area.Pos.C + box_area.Sze.W - 1, ACS_LRCORNER );
	addch( box_area.Pos.L,		    box_area.Pos.C, ACS_ULCORNER );
    }

    return OK;
}



// move subwin tree inside parent
int NCursesWindow::mvsubwin( NCursesWindow * sub, int begin_y, int begin_x )
{
    int ret = ERR;

    if ( sub && sub->parent() )
    {
	sub->w->_parx = -1; // force ncurses to actually move the child
	ret = ::mvderwin( sub->w, begin_y, begin_x );

	for ( NCursesWindow * ch = sub->child(); ch && ret == OK; ch = ch->sibling() )
	{
	    ret = mvsubwin( ch, ch->w->_pary, ch->w->_parx );
	}
    }

    return ret;
}



int NCursesWindow::resize( int lines, int columns )
{
    if ( lines <= 0 )
	lines = 1;

    if ( columns <= 0 )
	columns = 1;

    return ::wresize( w, lines, columns );
}


std::ostream & operator<<( std::ostream & Stream, const NCursesWindow * Obj_Cv )
{
    if ( Obj_Cv )
	return Stream << *Obj_Cv;

    return Stream << "(NoNCWin)";
}


std::ostream & operator<<( std::ostream & Stream, const NCursesWindow & Obj_Cv )
{
    return Stream << "NCWin(" << Obj_Cv.w
	   << wrect( wpos( Obj_Cv.begy(), Obj_Cv.begx() ),
		     wsze( Obj_Cv.height(), Obj_Cv.width() ) ) << ')';
}
