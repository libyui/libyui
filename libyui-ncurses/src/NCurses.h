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

   File:       NCurses.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCurses_h
#define NCurses_h

#include <iostream>
#include <string>
#include <set>
#include <map>

#include <yui/YEvent.h>
#include <yui/YWidget.h>
#include <yui/YMenuItem.h>

#include <ncursesw/curses.h>	/* curses.h: #define  NCURSES_CH_T cchar_t */
#include <wchar.h>

#include "ncursesw.h"
#include "ncursesp.h"
#include "position.h"
#include "NCstyle.h"

class NCWidget;
class NCDialog;


class NCursesError
{
public:

    int		errval_i;
    std::string	errmsg_t;

    NCursesError( const char * msg = "unknown error", ... );
    NCursesError( int val, const char * msg = "unknown error", ... );

    virtual ~NCursesError() {}

    NCursesError & NCError( const char * msg = "unknown error", ... );
    NCursesError & NCError( int val, const char * msg = "unknown error", ... );

    virtual const char * location() const { return "NCurses"; }
};

extern std::ostream & operator<<( std::ostream & STREAM, const NCursesError & OBJ );



class NCursesEvent
{

public:

    enum Type
    {
	handled = -1,
	none	= 0,
	cancel,
	timeout,
	button,
	menu,
	key
    };

    enum DETAIL
    {
	NODETAIL   = -1,
	CONTINUE   = -2,
	USERDEF    = -3
    };

    Type       type;
    NCWidget * widget;
    YMenuItem * selection;	// used for MenuEvent (the menu selection)

    std::string	result;		// can be used for any (std::string) result

    std::string	keySymbol;	// used for KeyEvent (symbol pressed key)

    int        detail;

    YEvent::EventReason reason;

    NCursesEvent( Type t = none, YEvent::EventReason r = YEvent::UnknownReason )
	: type( t )
	, widget( 0 )
	, selection( 0 )
	, result( "" )
	, detail( NODETAIL )
	, reason( r )
    {}

    virtual ~NCursesEvent() {}

    // not operator bool() which would be propagated to almost everything
    operator void*() const { return type != none ? ( void* )1 : ( void* )0; }

    bool operator==( const NCursesEvent & e ) const { return type == e.type; }

    bool operator!=( const NCursesEvent & e ) const { return type != e.type; }

    bool isReturnEvent()   const { return type > none; }

    bool isInternalEvent() const { return type < none; }


    // Some predefined events that can be used as return values

    static const NCursesEvent Activated;
    static const NCursesEvent SelectionChanged;
    static const NCursesEvent ValueChanged;
};

extern std::ostream & operator<<( std::ostream & STREAM, const NCursesEvent & OBJ );



class NCurses
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCurses & OBJ );

    NCurses & operator=( const NCurses & );
    NCurses( const NCurses & );

private:

    static NCurses * myself;

    static WINDOW * ripped_w_top;
    static WINDOW * ripped_w_bottom;
    static int ripinit_top( WINDOW * , int );
    static int ripinit_bottom( WINDOW * , int );

protected:

    SCREEN *	theTerm;
    std::string	myTerm;
    std::string	envTerm;
    WINDOW *	title_w;
    WINDOW *	status_w;
    std::string	title_t;

    std::map <int, std::string>   status_line;

    NCstyle *	   styleset;
    NCursesPanel * stdpan;

    void init();
    bool initialized() const { return stdpan; }

    virtual bool title_line()	{ return true; }

    virtual bool want_colors()	{ return true; }

    virtual void setup_screen();
    virtual void init_title();
    virtual void init_screen();

public:

    NCurses();
    virtual ~NCurses();

    int stdout_save;
    int stderr_save;

    static int cols()  { return ::COLS; }

    static int lines() { return ::LINES; }

    static int tabsize() { return ::TABSIZE; }

    void run();

public:

    static const NCstyle & style();

    static void Update();
    static void Redraw();
    static void Refresh();
    static void SetTitle( const std::string & str );
    static void SetStatusLine( std::map <int, std::string> fkeys );
    static void ScreenShot( const std::string & name = "screen.shot" );

    static void drawTitle();

public:
    // actually not for public use
    static void ForgetDlg( NCDialog * dlg_r );
    static void RememberDlg( NCDialog * dlg_r );
    void RedirectToLog();
    static void ResizeEvent();

private:
    static std::set<NCDialog*> _knownDlgs;
};


#define CTRL(x)     ((x) & 0x1f)
#define KEY_TAB     011
#define KEY_RETURN  012
#define KEY_ESC     033
#define KEY_SPACE   040
#define KEY_HOTKEY  KEY_MAX+1


#endif // NCurses_h
