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

   File:       ncursesw.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef _CURSESW_H
#define _CURSESW_H

#include <iosfwd>

#include <ncursesw/curses.h>
#ifndef NCURSES_CXX_IMPEXP
#define NCURSES_CXX_IMPEXP NCURSES_EXPORT_GENERAL_IMPORT
#endif
#include <ncursesw/etip.h>
#include <cstdio>
#include <cstdarg>
#include <climits>
#include "position.h"

/* SCO 3.2v4 curses.h includes term.h, which defines lines as a macro.
   Undefine it here, because NCursesWindow uses lines as a method.  */
#undef lines

/* "Convert" macros to inlines. We'll define it as another symbol to avoid
 * conflict with library symbols.
 */
#undef UNDEF
#define UNDEF(name) CUR_ ##name

#ifdef addch
inline int UNDEF( addch )( chtype ch )	{ return addch( ch ); }

#undef addch
#define addch UNDEF(addch)
#endif

#ifdef add_wch
inline int UNDEF( add_wch )( cchar_t * cch )  { return add_wch( cch ); }

#undef add_wch
#define add_wch UNDEF(add_wch)
#endif

#ifdef echochar
inline int UNDEF( echochar )( chtype ch )  { return echochar( ch ); }

#undef echochar
#define echochar UNDEF(echochar)
#endif

#ifdef insdelln
inline int UNDEF( insdelln )( int n )  { return insdelln( n ); }

#undef insdelln
#define insdelln UNDEF(insdelln)
#endif

#ifdef addstr
/* The (char*) cast is to hack around missing const's */
inline int UNDEF( addstr )( const char * str )	{ return addstr(( char* )str ); }

#undef addstr
#define addstr UNDEF(addstr)
#endif

#ifdef addwstr
/* The (wchar*_t) cast is to hack around missing const's */
inline int UNDEF( addwstr )( const wchar_t * str )  { return addwstr(( wchar_t* )str ); }

#undef addwstr
#define addwstr UNDEF(addwstr)
#endif

#ifdef attron
inline int UNDEF( attron )( chtype at ) { return attron( at ); }

#undef attron
#define attron UNDEF(attron)
#endif

#ifdef attroff
inline int UNDEF( attroff )( chtype at ) { return attroff( at ); }

#undef attroff
#define attroff UNDEF(attroff)
#endif

#ifdef attrset
inline chtype UNDEF( attrset )( chtype at ) { return attrset( at ); }

#undef attrset
#define attrset UNDEF(attrset)
#endif

#ifdef border
inline int UNDEF( border )( chtype ls, chtype rs, chtype ts, chtype bs, chtype tl, chtype tr, chtype bl, chtype br )
{ return border( ls, rs, ts, bs, tl, tr, bl, br ); }

#undef border
#define border UNDEF(border)
#endif

#ifdef box
inline int UNDEF( box )( WINDOW *win, int v, int h ) { return box( win, v, h ); }

#undef box
#define box UNDEF(box)
#endif

#ifdef mvwhline
inline int UNDEF( mvwhline )( WINDOW *win, int y, int x, chtype c, int n )
{
    return mvwhline( win, y, x, c, n );
}

#undef mvwhline
#define mvwhline UNDEF(mvwhline)
#endif

#ifdef mvwvline
inline int UNDEF( mvwvline )( WINDOW *win, int y, int x, chtype c, int n )
{
    return mvwvline( win, y, x, c, n );
}

#undef mvwvline
#define mvwvline UNDEF(mvwvline)
#endif

#ifdef clear
inline int UNDEF( clear )()  { return clear(); }

#undef clear
#define clear UNDEF(clear)
#endif

#ifdef clearok
inline int UNDEF( clearok )( WINDOW* win, bool bf )  { return clearok( win, bf ); }

#undef clearok
#define clearok UNDEF(clearok)
#else
extern "C" int clearok( WINDOW*, bool );
#endif

#ifdef clrtobot
inline int UNDEF( clrtobot )()	{ return clrtobot(); }

#undef clrtobot
#define clrtobot UNDEF(clrtobot)
#endif

#ifdef clrtoeol
inline int UNDEF( clrtoeol )()	{ return clrtoeol(); }

#undef clrtoeol
#define clrtoeol UNDEF(clrtoeol)
#endif

#ifdef delch
inline int UNDEF( delch )()  { return delch(); }

#undef delch
#define delch UNDEF(delch)
#endif

#ifdef deleteln
inline int UNDEF( deleteln )()	{ return deleteln(); }

#undef deleteln
#define deleteln UNDEF(deleteln)
#endif

#ifdef erase
inline int UNDEF( erase )()  { return erase(); }

#undef erase
#define erase UNDEF(erase)
#endif

#ifdef flushok
inline int UNDEF( flushok )( WINDOW* _win, bool _bf )
{
    return flushok( _win, _bf );
}

#undef flushok
#define flushok UNDEF(flushok)
#else
#define _no_flushok
#endif

#ifdef getch
inline int UNDEF( getch )()  { return getch(); }

#undef getch
#define getch UNDEF(getch)
#endif

#ifdef getstr
inline int UNDEF( getstr )( char *_str )  { return getstr( _str ); }

#undef getstr
#define getstr UNDEF(getstr)
#endif

#ifdef instr
inline int UNDEF( instr )( char *_str )  { return instr( _str ); }

#undef instr
#define instr UNDEF(instr)
#endif

#ifdef innstr
inline int UNDEF( innstr )( char *_str, int n )  { return innstr( _str, n ); }

#undef innstr
#define innstr UNDEF(innstr)
#endif

#ifdef mvwinnstr
inline int UNDEF( mvwinnstr )( WINDOW *win, int y, int x, char *_str, int n )
{
    return mvwinnstr( win, y, x, _str, n );
}

#undef mvwinnstr
#define mvwinnstr UNDEF(mvwinnstr)
#endif

#ifdef mvinnstr
inline int UNDEF( mvinnstr )( int y, int x, char *_str, int n )
{
    return mvinnstr( y, x, _str, n );
}

#undef mvinnstr
#define mvinnstr UNDEF(mvinnstr)
#endif

#ifdef winsstr
inline int UNDEF( winsstr )( WINDOW *w, const char *_str )
{
    return winsstr( w, _str );
}

#undef winsstr
#define winsstr UNDEF(winsstr)
#endif

#ifdef mvwinsstr
inline int UNDEF( mvwinsstr )( WINDOW *w, int y, int x,  const char *_str )
{
    return mvwinsstr( w, y, x, _str );
}

#undef mvwinsstr
#define mvwinsstr UNDEF(mvwinsstr)
#endif

#ifdef insstr
inline int UNDEF( insstr )( const char *_str )
{
    return insstr( _str );
}

#undef insstr
#define insstr UNDEF(insstr)
#endif

#ifdef mvinsstr
inline int UNDEF( mvinsstr )( int y, int x, const char *_str )
{
    return mvinsstr( y, x, _str );
}

#undef mvinsstr
#define mvinsstr UNDEF(mvinsstr)
#endif

#ifdef insnstr
inline int UNDEF( insnstr )( const char *_str, int n )
{
    return insnstr( _str, n );
}

#undef insnstr
#define insnstr UNDEF(insnstr)
#endif

#ifdef mvwinsnstr
inline int UNDEF( mvwinsnstr )( WINDOW *w, int y, int x, const char *_str, int n )
{
    return mvwinsnstr( w, y, x, _str, n );
}

#undef mvwinsnstr
#define mvwinsnstr UNDEF(mvwinsnstr)
#endif

#ifdef mvinsnstr
inline int UNDEF( mvinsnstr )( int y, int x, const char *_str, int n )
{
    return mvinsnstr( y, x, _str, n );
}

#undef mvinsnstr
#define mvinsnstr UNDEF(mvinsnstr)
#endif

#ifdef getnstr
inline int UNDEF( getnstr )( char *_str, int n )  { return getnstr( _str, n ); }

#undef getnstr
#define getnstr UNDEF(getnstr)
#endif

#ifdef getyx
inline void UNDEF( getyx )( const WINDOW* win, int& y, int& x )
{
    getyx( win, y, x );
}

#undef getyx
#define getyx UNDEF(getyx)
#endif

#ifdef getbegyx
inline void UNDEF( getbegyx )( WINDOW* win, int& y, int& x ) { getbegyx( win, y, x ); }

#undef getbegyx
#define getbegyx UNDEF(getbegyx)
#endif

#ifdef getmaxyx
inline void UNDEF( getmaxyx )( WINDOW* win, int& y, int& x ) { getmaxyx( win, y, x ); }

#undef getmaxyx
#define getmaxyx UNDEF(getmaxyx)
#endif

#ifdef hline
inline int UNDEF( hline )( chtype ch, int n ) { return hline( ch, n ); }

#undef hline
#define hline UNDEF(hline)
#endif

#ifdef inch
inline chtype UNDEF( inch )()  { return inch(); }

#undef inch
#define inch UNDEF(inch)
#endif

#ifdef in_wch
inline int UNDEF( in_wch )( cchar_t * cch )  { return in_wch( cch ); }

#undef in_wch
#define in_wch UNDEF(in_wch)
#endif

#ifdef insch
inline int UNDEF( insch )( char c )  { return insch( c ); }

#undef insch
#define insch UNDEF(insch)
#endif

#ifdef ins_wch
inline int UNDEF( ins_wch )( const cchar_t *c )  { return ins_wch( c ); }

#undef ins_wch
#define ins_wch UNDEF(ins_wch)
#endif

#ifdef mvwins_wch
inline int UNDEF( mvwins_wch )( WINDOW *w, int y, int x, const cchar_t *cchar )  { return mvwins_wch( w, y, x, cchar ); }

#undef mvwins_wch
#define mvwins_wch UNDEF(mvwins_wch)
#endif

#ifdef insertln
inline int UNDEF( insertln )()	{ return insertln(); }

#undef insertln
#define insertln UNDEF(insertln)
#endif

#ifdef leaveok
inline int UNDEF( leaveok )( WINDOW* win, bool bf )  { return leaveok( win, bf ); }

#undef leaveok
#define leaveok UNDEF(leaveok)
#else
extern "C" int leaveok( WINDOW* win, bool bf );
#endif

#ifdef move
inline int UNDEF( move )( int x, int y )  { return move( x, y ); }

#undef move
#define move UNDEF(move)
#endif

#ifdef refresh
inline int UNDEF( refresh )()  { return refresh(); }

#undef refresh
#define refresh UNDEF(refresh)
#endif

#ifdef redrawwin
inline int UNDEF( redrawwin )( WINDOW *win )  { return redrawwin( win ); }

#undef redrawwin
#define redrawwin UNDEF(redrawwin)
#endif

#ifdef scrl
inline int UNDEF( scrl )( int l ) { return scrl( l ); }

#undef scrl
#define scrl UNDEF(scrl)
#endif

#ifdef scroll
inline int UNDEF( scroll )( WINDOW *win ) { return scroll( win ); }

#undef scroll
#define scroll UNDEF(scroll)
#endif

#ifdef scrollok
inline int UNDEF( scrollok )( WINDOW* win, bool bf )  { return scrollok( win, bf ); }

#undef scrollok
#define scrollok UNDEF(scrollok)
#else
#if	defined(__NCURSES_H)
extern "C" int scrollok( WINDOW*, bool );
#else
extern "C" int scrollok( WINDOW*, char );
#endif
#endif

#ifdef setscrreg
inline int UNDEF( setscrreg )( int t, int b ) { return setscrreg( t, b ); }

#undef setscrreg
#define setscrreg UNDEF(setscrreg)
#endif

#ifdef standend
inline int UNDEF( standend )()	{ return standend(); }

#undef standend
#define standend UNDEF(standend)
#endif

#ifdef standout
inline int UNDEF( standout )()	{ return standout(); }

#undef standout
#define standout UNDEF(standout)
#endif

#ifdef subpad
inline WINDOW *UNDEF( subpad )( WINDOW *p, int l, int c, int y, int x )
{ return derwin( p, l, c, y, x ); }

#undef subpad
#define subpad UNDEF(subpad)
#endif

#ifdef timeout
#if NCURSES_VERSION_MAJOR < 5
inline int UNDEF( timeout )( int delay ) { return timeout( delay ); }

#else
inline void UNDEF( timeout )( int delay ) { timeout( delay ); }

#endif
#undef timeout
#define timeout UNDEF(timeout)
#endif

#ifdef touchline
inline int UNDEF( touchline )( WINDOW *win, int s, int c )
{ return touchline( win, s, c ); }

#undef touchline
#define touchline UNDEF(touchline)
#endif

#ifdef touchwin
inline int UNDEF( touchwin )( WINDOW *win ) { return touchwin( win ); }

#undef touchwin
#define touchwin UNDEF(touchwin)
#endif

#ifdef untouchwin
inline int UNDEF( untouchwin )( WINDOW *win ) { return untouchwin( win ); }

#undef untouchwin
#define untouchwin UNDEF(untouchwin)
#endif

#ifdef vline
inline int UNDEF( vline )( chtype ch, int n ) { return vline( ch, n ); }

#undef vline
#define vline UNDEF(vline)
#endif

#ifdef waddstr
inline int UNDEF( waddstr )( WINDOW *win, char *str ) { return waddstr( win, str ); }

#undef waddstr
#define waddstr UNDEF(waddstr)
#endif

#ifdef waddwstr
inline int UNDEF( waddwstr )( WINDOW *win, wchar_t *str ) { return waddwstr( win, str ); }

#undef waddwstr
#define waddwstr UNDEF(waddwstr)
#endif

#ifdef mvwaddwstr
inline int UNDEF( mvwaddwstr )( WINDOW *win, int y, int x, wchar_t *str )
{ return mvwaddwstr( win, y, x, str ); }

#undef mvwaddwstr
#define mvwaddwstr UNDEF(mvwaddwstr)
#endif

#ifdef waddchstr
inline int UNDEF( waddchstr )( WINDOW *win, chtype *at ) { return waddchstr( win, at ); }

#undef waddchstr
#define waddchstr UNDEF(waddchstr)
#endif

#ifdef wstandend
inline int UNDEF( wstandend )( WINDOW *win )  { return wstandend( win ); }

#undef wstandend
#define wstandend UNDEF(wstandend)
#endif

#ifdef wstandout
inline int UNDEF( wstandout )( WINDOW *win )  { return wstandout( win ); }

#undef wstandout
#define wstandout UNDEF(wstandout)
#endif


#ifdef wattroff
inline int UNDEF( wattroff )( WINDOW *win, int att ) { return wattroff( win, att ); }

#undef wattroff
#define wattroff UNDEF(wattroff)
#endif

#ifdef chgat
inline int UNDEF( chgat )( int n, attr_t attr, short color, const void *opts )
{
    return chgat( n, attr, color, opts );
}

#undef chgat
#define chgat UNDEF(chgat)
#endif

#ifdef mvchgat
inline int UNDEF( mvchgat )( int y, int x, int n,
			     attr_t attr, short color, const void *opts )
{
    return mvchgat( y, x, n, attr, color, opts );
}

#undef mvchgat
#define mvchgat UNDEF(mvchgat)
#endif

#ifdef mvwchgat
inline int UNDEF( mvwchgat )( WINDOW *win, int y, int x, int n,
			      attr_t attr, short color, const void *opts )
{
    return mvwchgat( win, y, x, n, attr, color, opts );
}

#undef mvwchgat
#define mvwchgat UNDEF(mvwchgat)
#endif

#ifdef wattrset
inline int UNDEF( wattrset )( WINDOW *win, int att ) { return wattrset( win, att ); }

#undef wattrset
#define wattrset UNDEF(wattrset)
#endif

#ifdef winch
inline chtype UNDEF( winch )( const WINDOW* win ) { return winch( win ); }

#undef winch
#define winch UNDEF(winch)
#endif

#ifdef mvwaddch
inline int UNDEF( mvwaddch )( WINDOW *win, int y, int x, const chtype ch )
{ return mvwaddch( win, y, x, ch ); }

#undef mvwaddch
#define mvwaddch UNDEF(mvwaddch)
#endif

#ifdef mvwaddchnstr
inline int UNDEF( mvwaddchnstr )( WINDOW *win, int y, int x, chtype *str, int n )
{ return mvwaddchnstr( win, y, x, str, n ); }

#undef mvwaddchnstr
#define mvwaddchnstr UNDEF(mvwaddchnstr)
#endif

#ifdef mvwaddchstr
inline int UNDEF( mvwaddchstr )( WINDOW *win, int y, int x, chtype *str )
{ return mvwaddchstr( win, y, x, str ); }

#undef mvwaddchstr
#define mvwaddchstr UNDEF(mvwaddchstr)
#endif

#ifdef addnstr
inline int UNDEF( addnstr )( const char *str, int n )
{ return addnstr(( char* )str, n ); }

#undef addnstr
#define addnstr UNDEF(addnstr)
#endif

#ifdef addnwstr
inline int UNDEF( addnwstr )( const wchar_t *str, int n )
{ return addnwstr(( wchar_t* )str, n ); }

#undef addnwstr
#define addnwstr UNDEF(addnwstr)
#endif

#ifdef mvwaddnstr
inline int UNDEF( mvwaddnstr )( WINDOW *win, int y, int x, const char *str, int n )
{ return mvwaddnstr( win, y, x, ( char* )str, n ); }

#undef mvwaddnstr
#define mvwaddnstr UNDEF(mvwaddnstr)
#endif

#ifdef mvwaddnwstr
inline int UNDEF( mvwaddnwstr )( WINDOW *win, int y, int x, const wchar_t *str, int n )
{ return mvwaddnwstr( win, y, x, ( wchar_t* )str, n ); }

#undef mvwaddnwstr
#define mvwaddnwstr UNDEF(mvwaddnwstr)
#endif

#ifdef mvwaddstr
inline int UNDEF( mvwaddstr )( WINDOW *win, int y, int x, const char * str )
{ return mvwaddstr( win, y, x, ( char* )str ); }

#undef mvwaddstr
#define mvwaddstr UNDEF(mvwaddstr)
#endif

#ifdef mvwdelch
inline int UNDEF( mvwdelch )( WINDOW *win, int y, int x )
{ return mvwdelch( win, y, x ); }

#undef mvwdelch
#define mvwdelch UNDEF(mvwdelch)
#endif

#ifdef mvwgetch
inline int UNDEF( mvwgetch )( WINDOW *win, int y, int x ) { return mvwgetch( win, y, x );}

#undef mvwgetch
#define mvwgetch UNDEF(mvwgetch)
#endif

#ifdef mvwgetstr
inline int UNDEF( mvwgetstr )( WINDOW *win, int y, int x, char *str )
{return mvwgetstr( win, y, x, str );}

#undef mvwgetstr
#define mvwgetstr UNDEF(mvwgetstr)
#endif

#ifdef mvwgetnstr
inline int UNDEF( mvwgetnstr )( WINDOW *win, int y, int x, char *str, int n )
{return mvwgetnstr( win, y, x, str, n );}

#undef mvwgetnstr
#define mvwgetnstr UNDEF(mvwgetnstr)
#endif

#ifdef mvwinch
inline chtype UNDEF( mvwinch )( WINDOW *win, int y, int x )
{
    return mvwinch( win, y, x );
}

#undef mvwinch
#define mvwinch UNDEF(mvwinch)
#endif

#ifdef mvwin_wch
inline int UNDEF( mvwin_wch )( WINDOW *win, int y, int x, cchar_t * cch )
{
    return mvwin_wch( win, y, x, cch );
}

#undef mvwin_wch
#define mvwin_wch UNDEF(mvwin_wch)
#endif

#ifdef mvwinsch
inline int UNDEF( mvwinsch )( WINDOW *win, int y, int x, char c )
{ return mvwinsch( win, y, x, c ); }

#undef mvwinsch
#define mvwinsch UNDEF(mvwinsch)
#endif

#ifdef mvaddch
inline int UNDEF( mvaddch )( int y, int x, chtype ch )
{ return mvaddch( y, x, ch ); }

#undef mvaddch
#define mvaddch UNDEF(mvaddch)
#endif

#ifdef mvaddnstr
inline int UNDEF( mvaddnstr )( int y, int x, const char *str, int n )
{ return mvaddnstr( y, x, ( char* )str, n ); }

#undef mvaddnstr
#define mvaddnstr UNDEF(mvaddnstr)
#endif

#ifdef mvaddstr
inline int UNDEF( mvaddstr )( int y, int x, const char * str )
{ return mvaddstr( y, x, ( char* )str ); }

#undef mvaddstr
#define mvaddstr UNDEF(mvaddstr)
#endif

#ifdef mvwadd_wch
inline int UNDEF( mvwadd_wch )( WINDOW *win, int y, int x, const cchar_t * cch )
{ return mvwadd_wch( win, y, x, ( cchar_t* )cch ); }

#undef mvwadd_wch
#define mvwadd_wch UNDEF(mvwadd_wch)
#endif

#ifdef mvdelch
inline int UNDEF( mvdelch )( int y, int x ) { return mvdelch( y, x );}

#undef mvdelch
#define mvdelch UNDEF(mvdelch)
#endif

#ifdef mvgetch
inline int UNDEF( mvgetch )( int y, int x ) { return mvgetch( y, x );}

#undef mvgetch
#define mvgetch UNDEF(mvgetch)
#endif

#ifdef mvgetstr
inline int UNDEF( mvgetstr )( int y, int x, char *str ) {return mvgetstr( y, x, str );}

#undef mvgetstr
#define mvgetstr UNDEF(mvgetstr)
#endif

#ifdef mvgetnstr
inline int UNDEF( mvgetnstr )( int y, int x, char *str, int n )
{
    return mvgetnstr( y, x, str, n );
}

#undef mvgetnstr
#define mvgetnstr UNDEF(mvgetnstr)
#endif

#ifdef mvinch
inline chtype UNDEF( mvinch )( int y, int x ) { return mvinch( y, x );}

#undef mvinch
#define mvinch UNDEF(mvinch)
#endif

#ifdef mvinsch
inline int UNDEF( mvinsch )( int y, int x, char c )
{ return mvinsch( y, x, c ); }

#undef mvinsch
#define mvinsch UNDEF(mvinsch)
#endif

#ifdef napms
inline void UNDEF( napms )( unsigned long x ) { napms( x ); }

#undef napms
#define napms UNDEF(napms)
#endif

#ifdef fixterm
inline int UNDEF( fixterm )( void ) { return fixterm(); }

#undef fixterm
#define fixterm UNDEF(fixterm)
#endif

#ifdef resetterm
inline int UNDEF( resetterm )( void ) { return resetterm(); }

#undef resetterm
#define resetterm UNDEF(resetterm)
#endif

#ifdef saveterm
inline int UNDEF( saveterm )( void ) { return saveterm(); }

#undef saveterm
#define saveterm UNDEF(saveterm)
#endif

#ifdef crmode
inline int UNDEF( crmode )( void ) { return crmode(); }

#undef crmode
#define crmode UNDEF(crmode)
#endif

#ifdef nocrmode
inline int UNDEF( nocrmode )( void ) { return nocrmode(); }

#undef nocrmode
#define nocrmode UNDEF(nocrmode)
#endif

#ifdef getbkgd
inline chtype UNDEF( getbkgd )( const WINDOW *win ) { return getbkgd( win ); }

#undef getbkgd
#define getbkgd UNDEF(getbkgd)
#endif

#ifdef bkgd
inline int UNDEF( bkgd )( chtype ch ) { return bkgd( ch ); }

#undef bkgd
#define bkgd UNDEF(bkgd)
#endif

#ifdef bkgdset
inline void UNDEF( bkgdset )( chtype ch ) { bkgdset( ch ); }

#undef bkgdset
#define bkgdset UNDEF(bkgdset)
#endif

template <class _Tp>   inline int ncursesMaxCoord()            { return INT_MAX; }
template <>            inline int ncursesMaxCoord<short>()     { return SHRT_MAX; }

/**
 * @short C++ class for windows.
*/

class NCursesWindow
{
    friend std::ostream & operator<<( std::ostream & Stream, const NCursesWindow & Obj_Cv );
    friend std::ostream & operator<<( std::ostream & Stream, const NCursesWindow * Obj_Cv );

    friend class NCursesMenu;
    friend class NCursesForm;

private:
    static bool    b_initialized;
    static void    initialize();
    static int	   ripoff_init( WINDOW *, int );

    void	   init();

    short	   getcolor( int getback ) const;

    static int	   setpalette( short fore, short back, short pair );
    static int	   colorInitialized;

    /**
     * This private constructor is only used during the initialization
     * of windows generated by ripoffline() calls.
    */
    NCursesWindow( WINDOW* win, int cols );

protected:
    /**
     * Signal an error with the given message text.
    */
    void	   err_handler( const char * ) const THROWS( NCursesException );

    /**
     * count of all active windows
    */
    static long count;
    //	 We rely on the c++ promise that
    //	 all otherwise uninitialized
    //	 static class vars are set to 0

    /**
     * the curses WINDOW
    */
    WINDOW*	   w;

    /**
     * TRUE if we own the WINDOW
    */
    bool	   alloced;

    /**
     * parent, if subwindow
    */
    NCursesWindow* par;
    /**
     * head of subwindows std::list
    */
    NCursesWindow* subwins;
    /**
     * next subwindow of parent
    */
    NCursesWindow* sib;

    /**
     * Destroy all subwindows.
    */
    void	   kill_subwindows();

    /**
     * Only for use by derived classes. They are then in charge to
     * fill the member variables correctly.
    */
    NCursesWindow();

public:
    /**
     * Constructor. Useful only for stdscr
     **/
    NCursesWindow( WINDOW* window );

    /**
     * Constructor.
     **/
    NCursesWindow( int lines,
		   int cols,
		   int begin_y,
		   int begin_x );

    /**
     * Constructor.
     *
     * If 'absrel' is 'a', begin_x/y are absolute screen pos,
     * if `r', they are relative to parent origin.
     **/
    NCursesWindow( NCursesWindow & parent,
		   int lines,
		   int cols,
		   int begin_y,
		   int begin_x,
		   char absrel = 'a' );

    /**
     * Destructor.
     **/
    virtual ~NCursesWindow();

    /**
     * Make an exact copy of the window.
    */
    NCursesWindow Clone();

    // -------------------------------------------------------------------------
    // Initialization.
    // -------------------------------------------------------------------------
    /**
     * Call this routine very early if you want to have colors.
    */
    static void    useColors( void );

    /**
     * This function is used to generate a window of ripped-of lines.
     * If the argument is positive, lines are removed from the top, if it
     * is negative lines are removed from the bottom. This enhances the
     * lowlevel ripoffline() function because it uses the internal
     * implementation that allows to remove more than just a single line.
     * This function must be called before any other ncurses function. The
     * creation of the window is defered until ncurses gets initialized.
     * The initialization function is then called.
    */
    static int ripoffline( int ripoff_lines,
			   int ( *init )( NCursesWindow& win ) );

    // -------------------------------------------------------------------------
    // terminal status
    // -------------------------------------------------------------------------
    /**
     * Number of lines on terminal, *not* window
    */
    static int	   lines() { initialize(); return LINES; }

    /**
     * Number of cols  on terminal, *not* window
    */
    static int	   cols() { initialize(); return COLS; }

    /**
     * Size of a tab on terminal, *not* window
    */
    static int	   tabsize() { initialize(); return TABSIZE; }

    /**
     * Number of available colors
    */
    static int	   NumberOfColors();

    /**
     * Number of available colors
    */
    int     colors() const { return NumberOfColors(); }

    // -------------------------------------------------------------------------
    // window status
    // -------------------------------------------------------------------------
    /**
     * Number of lines in this window
    */
    int		   height() const { return maxy() + 1; }

    /**
     * Number of columns in this window
    */
    int		   width() const { return maxx() + 1; }

    /**
     * Column of top left corner relative to stdscr
    */
    int		   begx() const { return getbegx(w); }

    /**
     * Line of top left corner relative to stdscr
    */
    int		   begy() const { return getbegy(w); }

    /**
     * Largest x coord in window
    */
    int		   maxx() const { return getmaxx(w) == ERR ? ERR : getmaxx(w)-1; }

    /**
     * Largest y coord in window
    */
    int		   maxy() const { return getmaxy(w) == ERR ? ERR : getmaxy(w)-1; }

    /** Ncurses up to ncurses5 internally uses \c short. */
    static int     maxcoord()   { return ncursesMaxCoord<NCURSES_SIZE_T>(); }

    wsze   size()      const { return wsze( height(), width() ); }

    wpos   begpos()    const { return wpos( begy(), begx() ); }

    wpos   maxpos()    const { return wpos( maxy(), maxx() ); }

    wrect  area()      const { return wrect( begpos(), size() ); }

    /**
     * Actual color pair
    */
    short  getcolor() const;

    /**
     * Actual foreground color
    */
    short  foreground() const { return getcolor( 0 ); }

    /**
     * Actual background color
    */
    short  background() const { return getcolor( 1 ); }

    /**
     * Set color palette entry
    */
    int    setpalette( short fore, short back );

    /**
     * Set actually used palette entry
    */
    int    setcolor( short pair );

    // -------------------------------------------------------------------------
    // window positioning
    // -------------------------------------------------------------------------
    /**
     * Move window to new position with the new position as top left corner.
     * This is virtual because it is redefined in NCursesPanel.
    */
    virtual int  mvwin( int begin_y, int begin_x )
    {
	return ::mvwin( w, begin_y, begin_x );
    }

    int mvsubwin( NCursesWindow* sub, int begin_y, int begin_x );

    virtual int resize( int lines, int columns );

    // -------------------------------------------------------------------------
    // coordinate positioning
    // -------------------------------------------------------------------------
    /**
     * Move cursor the this position
    */
    int		   move( int y, int x ) { return ::wmove( w, y, x ); }

    /**
     * Get current position of the cursor
    */
    void	   getyx( int& y, int& x ) const { ::getyx( w, y, x ); }

    /**
     * Perform lowlevel cursor motion that takes effect immediately.
    */
    int		   mvcur( int oldrow, int oldcol, int newrow, int newcol ) const
    {
	return ::mvcur( oldrow, oldcol, newrow, newcol );
    }

    // -------------------------------------------------------------------------
    // input
    // -------------------------------------------------------------------------

    int		   nodelay( bool bf ) { return ::nodelay( w, bf ); }

    /**
     * Get a keystroke from the window.
    */
    int		   getch() { return ::wgetch( w ); }

    /**
     * Move cursor to position and get a keystroke from the window
    */
    int		   getch( int y, int x ) { return ::mvwgetch( w, y, x ); }

    /**
     * Read a series of characters into str until a newline or carriage return
     * is received. Read at most n characters. If n is negative, the limit is
     * ignored.
    */
    int		   getstr( char* str, int n = -1 )
    {
	return ::wgetnstr( w, str, n );
    }

    /**
     * Move the cursor to the requested position and then perform the getstr()
     * as described above.
    */
    int		   getstr( int y, int x, char* str, int n = -1 )
    {
	return ::mvwgetnstr( w, y, x, str, n );
    }

    /**
     * Get a std::string of characters from the window into the buffer s. Retrieve
     * at most n characters, if n is negative retrieve all characters up to the
     * end of the current line. Attributes are stripped from the characters.
    */
    int		   instr( char *s, int n = -1 ) { return ::winnstr( w, s, n ); }

    /**
     * Move the cursor to the requested position and then perform the instr()
     * as described above.
    */
    int		   instr( int y, int x, char *s, int n = -1 )
    {
	return ::mvwinnstr( w, y, x, s, n );
    }


    // -------------------------------------------------------------------------
    // output
    // -------------------------------------------------------------------------
    /**
     * Put attributed character to the window.
    */
    int		   addch( const char ch )
    {
	return addch(( const chtype )( ch&A_CHARTEXT ) );
    }

    int		   addch( const chtype ch ) { return ::waddch( w, ch ); }

    /**
    * Put attributed character from given position to the window.
    */
    int		add_attr_char( int y, int x );
    int		add_attr_char();

    /**
    * Put a combined character to the window.
    */
    int		   add_wch( const cchar_t * cch ) { return ::wadd_wch( w, cch ); }

    int		   add_wch( int y, int x, const cchar_t * cch ) { return mvwadd_wch( w, y, x, cch ); }

    /**
     * Move cursor to the requested position and then put attributed character
     * to the window.
    */
    int		   addch( int y, int x, const char ch )
    {
	return addch( y, x, ( const chtype )( ch&A_CHARTEXT ) );
    }

    int		   addch( int y, int x, const chtype ch )
    {
	return ::mvwaddch( w, y, x, ch );
    }

    /**
     * Put attributed character to the window and refresh it immediately.
    */
    int		   echochar( const char ch )
    {
	return echochar(( const chtype )( ch&A_CHARTEXT ) );
    }

    int		   echochar( const chtype ch ) { return ::wechochar( w, ch ); }

    /**
     * Write the std::string str to the window, stop writing if the terminating
     * NUL or the limit n is reached. If n is negative, it is ignored.
    */
    int		   addstr( const char* str, int n = -1 )
    {
	return ::waddnstr( w, ( char* )str, n );
    }

    /**
      * Move the cursor to the requested position and then perform the addstr
      * as described above.
     */
    int		   addstr( int y, int x, const char * str, int n = -1 )
    {
	return ::mvwaddnstr( w, y, x, ( char* )str, n );
    }

    /**
      * Write the wchar_t str to the window, stop writing if the terminating
      * NUL or the limit n is reached. If n is negative, it is ignored.
     */
    int		   addwstr( const wchar_t* str, int n = -1 );

    /**
     * Move the cursor to the requested position and then perform the addwstr
     * as described above.
    */
    int		   addwstr( int y, int x, const wchar_t * str, int n = -1 );

    /**
     * Do a formatted print to the window.
    */
    int		   printw( const char* fmt, ... )
#if __GNUG__ >= 2
    __attribute__(( format( printf, 2, 3 ) ) );
#else
    ;
#endif

    /**
     * Move the cursor and then do a formatted print to the window.
    */
    int		   printw( int y, int x, const char * fmt, ... )
#if __GNUG__ >= 2
    __attribute__(( format( printf, 4, 5 ) ) );
#else
    ;
#endif

    /**
     * Retrieve attributed character under the current cursor position.
    */
    chtype	   inch() const { return ::winch( w ); }

    chtype	   inchar() const { return inch()&( A_CHARTEXT | A_ALTCHARSET ); }

    /**
     * Move cursor to requested position and then retrieve attributed character
     * at this position.
    */
    chtype	   inch( int y, int x ) { return ::mvwinch( w, y, x ); }

    chtype	   inchar( int y, int x ) { return inch( y, x )&( A_CHARTEXT | A_ALTCHARSET ); }

    /**
     * Retrieve combined character under the current cursor position.
     */
    int		in_wchar( cchar_t * cchar );
    int 		in_wchar( int y, int x, cchar_t * cchar );

    /**
     * Insert attributed character into the window before current cursor
     * position.
    */
    int		   insch( chtype ch ) { return ::winsch( w, ch ); }

    /**
     * Move cursor to requested position and then insert the attributed
     * character before that position.
    */
    int		   insch( int y, int x, chtype ch )
    {
	return ::mvwinsch( w, y, x, ch );
    }

    /**
      * Move cursor to requested position and then insert the attributed
      * character before that position.
     */
    int		   ins_wch( int y, int x, const cchar_t * cchar )
    {
	return mvwins_wch( w, y, x, cchar );
    }

    /**
    * Insert an empty line above the current line.
    */
    int		   insertln() { return ::winsdelln( w, 1 ); }

    /**
     * If n>0 insert that many lines above the current line. If n<0 delete
     * that many lines beginning with the current line.
    */
    int		   insdelln( int n = 1 ) { return ::winsdelln( w, n ); }

    /**
     * Insert the std::string into the window before the current cursor position.
     * Insert stops at end of std::string or when the limit n is reached. If n is
     * negative, it is ignored.
    */
    int		   insstr( const char *s, int n = -1 )
    {
	return ::winsnstr( w, s, n );
    }

    /**
     * Move the cursor to the requested position and then perform the insstr()
     * as described above.
    */
    int		   insstr( int y, int x, const char *s, int n = -1 )
    {
	return ::mvwinsnstr( w, y, x, s, n );
    }

    /**
     * Switch on the window attributes;
    */
    int		   attron( chtype at ) { return ::wattron( w, at ); }

    /**
     * Switch off the window attributes;
    */
    int		   attroff( chtype at ) { return ::wattroff( w, at ); }

    /**
     * Set the window attributes;
    */
    int		   attrset( chtype at ) { return ::wattrset( w, at ); }

    /**
     * Change the attributes of the next n characters in the current line. If
     * n is negative or greater than the number of remaining characters in the
     * line, the attributes will be changed up to the end of the line.
    */
    int		   chgat( int n, attr_t attr, short color, const void *opts = NULL )
    {
	return ::wchgat( w, n, attr, color, opts );
    }

    /**
     * Move the cursor to the requested position and then perform chgat() as
     * described above.
    */
    int		   chgat( int y, int x,
			  int n, attr_t attr, short color, const void *opts = NULL )
    {
	return ::mvwchgat( w, y, x, n, attr, color, opts );
    }

    // -------------------------------------------------------------------------
    // background
    // -------------------------------------------------------------------------
    /**
     * Get current background setting.
    */
    chtype	   getbkgd() const { return ::getbkgd( w ); }

    /**
     * Set the background property and apply it to the window.
    */
    int		   bkgd( const chtype ch ) { return ::wbkgd( w, ch ); }

    /**
     * Set the background property.
    */
    void	   bkgdset( chtype ch ) { ::wbkgdset( w, ch ); }

    // -------------------------------------------------------------------------
    // borders
    // -------------------------------------------------------------------------
    /**
     * Draw a box around the window with the given vertical and horizontal
     * drawing characters. If you specifiy a zero as character, curses will try
     * to find a "nice" character.
    */
    // int	      box(chtype vert=0, chtype  hor=0) {
    //	 return ::wborder(w, vert, vert, hor, hor, 0, 0 ,0, 0); }

    // workaround for 8.1: don't use wborder to draw the box
    int box() { return box( wrect( wpos( 0, 0 ), size() ) ); }

    /**
     * Draw a border around the window with the given characters for the
     * various parts of the border. If you pass zero for a character, curses
     * will try to find "nice" characters.
    */
    int		   border( chtype left = 0, chtype right = 0,
			   chtype top = 0, chtype bottom = 0,
			   chtype top_left = 0, chtype top_right = 0,
			   chtype bottom_left = 0, chtype bottom_right = 0 )
    {
	return ::wborder( w, left, right, top, bottom, top_left, top_right,
			  bottom_left, bottom_right );
    }

    // -------------------------------------------------------------------------
    // lines and boxes
    // -------------------------------------------------------------------------
    /**
     * Draw a horizontal line of len characters with the given character. If
     * you pass zero for the character, curses will try to find a "nice" one.
    */
    int		   hline( int len, chtype ch = 0 ) { return ::whline( w, ch, len ); }

    /**
     * Move the cursor to the requested position and then draw a horizontal line.
    */
    int		   hline( int y, int x, int len, chtype ch = 0 )
    {
	return ::mvwhline( w, y, x, ch, len );
    }

    /**
     * Draw a vertical line of len characters with the given character. If
     * you pass zero for the character, curses will try to find a "nice" one.
    */
    int		   vline( int len, chtype ch = 0 ) { return ::wvline( w, ch, len ); }

    /**
     * Move the cursor to the requested position and then draw a vertical line.
    */
    int		   vline( int y, int x, int len, chtype ch = 0 )
    {
	return ::mvwvline( w, y, x, ch, len );
    }

    int		   box( const wrect & dim );

    // -------------------------------------------------------------------------
    // erasure
    // -------------------------------------------------------------------------
    /**
     * Erase the window.
    */
    int		   erase() { return ::werase( w ); }

    /**
     * Clear the window.
    */
    int		   clear() { return ::wclear( w ); }

    /**
     * Set/Reset the clear flag. If std::set, the next refresh() will clear the
     * screen.
    */
    int		   clearok( bool bf ) { return ::clearok( w, bf ); }

    /**
     * Clear to the end of the window.
    */
    int		   clrtobot() { return ::wclrtobot( w ); }

    /**
     * Clear to the end of the line.
    */
    int		   clrtoeol() { return ::wclrtoeol( w ); }

    /**
     * Delete character under the cursor.
    */
    int		   delch() { return ::wdelch( w ); }

    /**
     * Move cursor to requested position and delete the character under the
     * cursor.
    */
    int		   delch( int y, int x ) { return ::mvwdelch( w, y, x ); }

    /**
     * Delete the current line.
    */
    int		   deleteln() { return ::winsdelln( w, -1 ); }

    // -------------------------------------------------------------------------
    // screen control
    // -------------------------------------------------------------------------
    /**
     * Scroll amount lines. If amount is positive, scroll up, otherwise
     * scroll down.
    */
    int		   scroll( int amount = 1 ) { return ::wscrl( w, amount ); }

    /**
     * If bf is TRUE, window scrolls if cursor is moved off the bottom
     * edge of the window or a scrolling region, otherwise the cursor is left
     * at the bottom line.
    */
    int		   scrollok( bool bf ) { return ::scrollok( w, bf ); }

    /**
     * Define a soft scrolling region.
    */
    int		   setscrreg( int from, int to )
    {
	return ::wsetscrreg( w, from, to );
    }

    /**
     * If bf is TRUE, use insert/delete line hardware support if possible.
     * Otherwise do it in software.
    */
    int		   idlok( bool bf ) { return ::idlok( w, bf ); }

    /**
     * If bf is TRUE, use insert/delete character hardware support if possible.
     * Otherwise do it in software.
    */
    void	   idcok( bool bf ) { ::idcok( w, bf ); }

    /**
     * Mark the whole window as modified.
    */
    int		   touchwin()	{ return ::wtouchln( w, 0, height(), 1 ); }

    /**
     * Mark the whole window as unmodified.
    */
    int		   untouchwin() { return ::wtouchln( w, 0, height(), 0 ); }

    /**
     * Mark cnt lines beginning from line s as changed or unchanged, depending
     * on the value of the changed flag.
    */
    int		   touchln( int s, int cnt, bool changed = TRUE )
    {
	return ::wtouchln( w, s, cnt, ( int )( changed ? 1 : 0 ) );
    }

    /**
     * Return TRUE if window is marked as changed, FALSE otherwise
    */
    bool	   is_wintouched() const
    {
	return ( ::is_wintouched( w ) ? TRUE : FALSE );
    }

    /**
     * If bf is TRUE, curses will leave the cursor after an update whereever
     * it is after the update.
    */
    int		   leaveok( bool bf ) { return ::leaveok( w, bf ); }

    /**
     * Redraw n lines starting from the requested line
    */
    int		   redrawln( int from, int n ) { return ::wredrawln( w, from, n ); }

    /**
     * Redraw the whole window
    */
    int		   redrawwin() { return ::wredrawln( w, 0, height() ); }

    /**
     * Do all outputs to make the physical screen looking like the virtual one
    */
    int		   doupdate()  { return ::doupdate(); }

    /**
     * Propagate the changes down to all descendant windows
    */
    void	   syncdown()  { ::wsyncdown( w ); }

    /**
     * Propagate the changes up in the hierarchy
    */
    void	   syncup()    { ::wsyncup( w ); }

    /**
     * Position the cursor in all ancestor windows corresponding to our setting
    */
    void	   cursyncup() { ::wcursyncup( w ); }

    /**
     * If called with bf=TRUE, syncup() is called whenever the window is changed
    */
    int		   syncok( bool bf ) { return ::syncok( w, bf ); }

#ifndef _no_flushok
    int		   flushok( bool bf ) { return ::flushok( w, bf ); }

#endif

    /**
     * If called with bf=TRUE, any change in the window will cause an
     * automatic immediate refresh()
    */
    void	   immedok( bool bf ) { ::immedok( w, bf ); }

    /**
     * If called with bf=TRUE, the application will interpret function keys.
    */
    int		   keypad( bool bf ) { return ::keypad( w, bf ); }

    /**
     * If called with bf=TRUE, keys may generate 8-Bit characters. Otherwise
     * 7-Bit characters are generated.
    */
    int		   meta( bool bf ) { return ::meta( w, bf ); }

    /**
     * Enable "standout" attributes
    */
    int		   standout() { return ::wstandout( w ); }

    /**
     * Disable "standout" attributes
    */
    int		   standend() { return ::wstandend( w ); }

    // -------------------------------------------------------------------------
    // The next two are virtual, because we redefine them in the
    // NCursesPanel class.
    // -------------------------------------------------------------------------
    /**
     * Propagate the changes in this window to the virtual screen and call
     * doupdate(). This is redefined in NCursesPanel.
    */
    virtual int    refresh() { return ::wrefresh( w ); }

    /**
     * Propagate the changes in this window to the virtual screen. This is
     * redefined in NCursesPanel.
    */
    virtual int    noutrefresh() { return ::wnoutrefresh( w ); }

    // -------------------------------------------------------------------------
    // multiple window control
    // -------------------------------------------------------------------------
    /**
     * Overlay this window over win.
    */
    int		   overlay( NCursesWindow& win )
    {
	return ::overlay( w, win.w );
    }

    /**
     * Overwrite win with this window.
    */
    int		   overwrite( NCursesWindow& win )
    {
	return ::overwrite( w, win.w );
    }

    /**
     * Overlay or overwrite the rectangle in win given by dminrow,dmincol,
     * dmaxrow,dmaxcol with the rectangle in this window beginning at
     * sminrow,smincol.
    */
    int		   copywin( NCursesWindow& win,
			    int sminrow, int smincol,
			    int dminrow, int dmincol,
			    int dmaxrow, int dmaxcol, bool overlay = TRUE )
    {
	return ::copywin( w, win.w, sminrow, smincol, dminrow, dmincol,
			  dmaxrow, dmaxcol, ( int )( overlay ? 1 : 0 ) );
    }

    // -------------------------------------------------------------------------
    // Mouse related
    // -------------------------------------------------------------------------
    /**
     * Return TRUE if terminal supports a mouse, FALSE otherwise
    */
    bool has_mouse() const;

    // -------------------------------------------------------------------------
    // traversal support
    // -------------------------------------------------------------------------
    /**
     * Get the first child window.
    */
    NCursesWindow*	  child()	{ return subwins; }

    const NCursesWindow*  child() const { return subwins; }

    /**
     * Get the next child of my parent.
    */
    NCursesWindow*	  sibling()	  { return sib; }

    const NCursesWindow*  sibling() const { return sib; }

    /**
     * Get my parent.
    */
    NCursesWindow*	  parent()	 { return par; }

    const NCursesWindow*  parent() const { return par; }

    /**
     * Return TRUE if win is a descendant of this.
    */
    bool isDescendant( NCursesWindow& win );
};

/**
 * @short We leave this here for compatibility reasons.
*/

class NCursesColorWindow : public NCursesWindow
{

public:
    /**
     * Constructor. Useful only for stdscr
    */
    NCursesColorWindow( WINDOW* &window )
	    : NCursesWindow( window )
    {
	useColors();
    }

    /**
     * Constructor.
     **/
    NCursesColorWindow( int lines,
			int cols,
			int begin_y,
			int begin_x )
	    : NCursesWindow( lines, cols, begin_y, begin_x )
    {
	useColors();
    }

    /**
     * Constructor.
     *
     *	If 'absrel' is 'a', begin_x/y are absolute screen pos,
     *	else if 'r', they are relative to par origin
    */
    NCursesColorWindow( NCursesWindow& par,
			int lines,
			int cols,
			int begin_y,
			int begin_x,
			char absrel = 'a' )
	    : NCursesWindow( par, lines, cols,
			     begin_y, begin_x,
			     absrel )
    {
	useColors();
    }
};


class NCursesPad : public NCursesWindow
{

public:
    NCursesPad( int lines, int cols );

    /**
     * Put the attributed character onto the pad and immediately do a
     * prefresh().
    */
    int echochar( const chtype ch ) { return ::pechochar( w, ch ); }

    /**
     * For Pad's we reimplement refresh() and noutrefresh() to do nothing.
     * You should call the versions with the argument std::list that are specific
     * for Pad's.
    */
    int refresh() { return OK; };

    int noutrefresh() { return OK; };

    /**
     * The coordinates sminrow,smincol,smaxrow,smaxcol describe a rectangle
     * on the screen. <b>refresh</b> copies a rectangle of this size beginning
     * with top left corner pminrow,pmincol onto the screen and calls doupdate().
    */
    int refresh( int pminrow, int pmincol,
		 int sminrow, int smincol,
		 int smaxrow, int smaxcol )
    {
	return ::prefresh( w, pminrow, pmincol,
			   sminrow, smincol, smaxrow, smaxcol );
    }

    /**
     * Does the same like refresh() but without calling doupdate().
    */
    int noutrefresh( int pminrow, int pmincol,
		     int sminrow, int smincol,
		     int smaxrow, int smaxcol )
    {
	return ::pnoutrefresh( w, pminrow, pmincol,
			       sminrow, smincol, smaxrow, smaxcol );
    }
};

#endif // _CURSESW_H
