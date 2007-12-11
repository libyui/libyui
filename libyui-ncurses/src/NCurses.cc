/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCurses.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
extern "C" {
#include <unistd.h>
}

#include <cstdarg>
#include <fstream>
#include <list>
using namespace std;

#include "../config.h"

#include "Y2Log.h"
#include "NCurses.h"
#include "NCDialog.h"
#include "NCi18n.h"

#include <signal.h>

#include <y2util/y2log.h>
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fnmatch.h>
}

/*
  Textdomain "packages"
*/

NCurses * NCurses::myself = 0;
set<NCDialog*> NCurses::_knownDlgs;
const NCursesEvent NCursesEvent::Activated        ( NCursesEvent::button, YEvent::Activated        );
const NCursesEvent NCursesEvent::SelectionChanged ( NCursesEvent::button, YEvent::SelectionChanged );
const NCursesEvent NCursesEvent::ValueChanged     ( NCursesEvent::button, YEvent::ValueChanged     );


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCursesError::NCursesError
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
#define CONVERR(n,p) \
  va_list ap;        \
  va_list ap1;       \
  va_start( ap, p ); \
  va_start( ap1, p );\
  errval_i = n;      \
  errmsg_t = vform( p, ap, ap1 ); \
  va_end( ap );      \
  va_end( ap1 )

NCursesError::NCursesError( char * msg, ... )
    : errval_i( ERR )
{
  CONVERR( ERR, msg );
}

NCursesError::NCursesError( int val, char * msg, ... )
    : errval_i( val )
{
  CONVERR( val, msg );
}

NCursesError & NCursesError::NCError( char * msg, ... ) {
  CONVERR( ERR, msg );
  return *this;
}

NCursesError & NCursesError::NCError( int val, char * msg, ... ) {
  CONVERR( val, msg );
  return *this;
}

#undef CONVERR

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const NCursesError & OBJ )
{
  STREAM << form( "%s: (%d) %s"
		  , OBJ.location()
		  , OBJ.errval_i
		  , OBJ.errmsg_t.c_str() );
  return STREAM;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const NCursesEvent & OBJ )
{
#define ENUM_OUT(v) case NCursesEvent::v: return STREAM << "Ev::" << #v
  switch ( OBJ.type ) {
  ENUM_OUT( none );
  ENUM_OUT( handled );
  ENUM_OUT( cancel );
  ENUM_OUT( button );
  ENUM_OUT( menu );
  ENUM_OUT( timeout );
  ENUM_OUT( key );
  }
#undef ENUM_OUT
  return STREAM << "Ev::unknown";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::NCurses
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCurses::NCurses()
    : theTerm ( 0 )
    , title_w ( 0 )
    , styleset( 0 )
    , stdpan  ( 0 )
{
  const char * term = getenv( "TERM" );
  if ( term && *term )
    envTerm = term;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::~NCurses
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCurses::~NCurses()
{
  UIMIL << "Shutdown NCurses..." << endl;
  myself = 0;

  //restore env. variable - might have been changed by NCurses::init()
  setenv("TERM", envTerm.c_str(), 1);
  delete styleset;
  delete stdpan;
  if ( title_w )
    ::delwin( title_w );
  ::endwin();
  if ( theTerm )
    ::delscreen( theTerm );
  UIMIL << "NCurses down" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::init
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
WINDOW * NCurses::ripped_w = 0;

int NCurses::ripinit( WINDOW * w, int c )
{
  ripped_w = w;
  return OK;
}

void NCurses::init()
{
  UIMIL << "Launch NCurses..." 
#ifdef VERSION
    << "(ui-ncurses-" << VERSION << ")"
#endif
    << endl;
  UIMIL << "TERM=" << envTerm << endl;

  signal( SIGINT, SIG_IGN );	// ignore Ctrl C
  
  if ( title_line() && ::ripoffline( 1, ripinit ) != OK )
      throw NCursesError( "ripoffline() failed" );
  
  UIMIL << "isatty(stdin)" << (isatty(0) ? "yes" : "no") << endl;
  if (isatty( 0 )) {
    char * mytty = ttyname( 0 );
    if ( mytty ) {
      UIMIL << "mytty: " << mytty << endl;
      FILE * fdi = fopen( mytty, "r" );
      if (!fdi) {
         UIERR << "fdi: (" << errno << ") " << strerror(errno) << endl;
      }
      FILE * fdo = fopen( mytty, "w" );
      if (!fdo) {
         UIERR << "fdo: (" << errno << ") " << strerror(errno) << endl;
      }
      if ( fdi && fdo ) {
	theTerm = newterm( 0, fdo, fdi );

	//initialization failed
	if ( theTerm == NULL ) {
	  //bug #235954: workaround missing terminfos in inst-sys
	  //let's close the first term
	  ::endwin();

	  string fallbackTerm = "";
	  //try generic xterm for xterm-like terminals, otherwise use vt100
	  if (! fnmatch("xterm*", envTerm.c_str(), 0))
	    fallbackTerm = "xterm";
	  else
	    fallbackTerm = "vt100";

	  UIWAR << "newterm() failed, using generic " << fallbackTerm << " as a fallback" << endl;
	  //overwrite environment variable
	  setenv("TERM", fallbackTerm.c_str(), 1);
	  //.. and try again
	  theTerm = newterm(0, fdo, fdi);

	  if (theTerm == NULL)
	    throw NCursesError( "fallback newterm() failed" );
	}
	if ( set_term( theTerm ) == NULL )
	  throw NCursesError( "set_term() failed" );
	myTerm = mytty;
      }
    }
  }
 
  //duplicate stdout and stderr before redirecting them to log
  //so that they can be regenerated before system() call
  stdout_save = dup(1);
  stderr_save = dup(2);

  RedirectToLog();
  
  if ( !theTerm ) {
    UIMIL << "no term so fall back to initscr" << endl;
    if ( ::initscr() == NULL )
      throw NCursesError( "initscr() failed" );
  }

  UIMIL << "have color = " << ::has_colors()  << endl;
  if ( want_colors() && ::has_colors() ) {
    if ( ::start_color() != OK )
      throw NCursesError( "start_color() failed" );
    NCattribute::init_colors();
  }

  if ( title_line() ) {
    if ( !ripped_w )
      throw NCursesError( "ripinit() failed" );
    title_w = ripped_w;
  }

  setup_screen();

  UIMIL << form( "screen size %d x %d\n", lines(), cols() );

  myself = this;

  styleset = new NCstyle( envTerm );
  stdpan   = new NCursesPanel();
  stdpan->bkgd( style()(NCstyle::AppText) );

  if ( title_line() )
    init_title();

  init_screen();

  UIMIL << "NCurses ready" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::setup_screen
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCurses::setup_screen()
{
  ::cbreak();
  ::noecho();
  ::keypad ( ::stdscr, true );
  ::meta   ( ::stdscr, true );
  ::leaveok( ::stdscr, true );
  ::curs_set( 0 );

  ::define_key( "\e[Z",   KEY_BTAB );
  ::define_key( "\e\t",   KEY_BTAB );
  ::define_key( "\030\t", KEY_BTAB );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::init_title
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCurses::init_title()
{
  ::wbkgd( title_w, style()(NCstyle::AppTitle) );
  ::wnoutrefresh( title_w );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::init_screen
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCurses::init_screen()
{
    bool redefine = false;

    char *value = getenv( "Y2NCPSEUDO" );
    
    // The 9.0 workaround for missing ACS chars (bug #30512) is not necessary
    // any longer (a patch is provided for ncurses-5.4).

    // Redefine ACS chars if Y2NCPSEUDO is set to "1" (just in case of ...)  
    if ( value != NULL && strcmp(value, "1") == 0 )
    {
	redefine = true;
    }
    
    if ( redefine )
    {
	chtype cch = 0;

	NCattribute::setChar(cch, '+');
	ACS_ULCORNER = cch;
	ACS_LLCORNER = cch;
	ACS_URCORNER = cch;
	ACS_LRCORNER = cch;
	ACS_LTEE = cch;
	ACS_RTEE = cch;
	ACS_BTEE = cch;
	ACS_TTEE = cch;
	ACS_PLUS = cch;

	NCattribute::setChar(cch, '|');
	ACS_VLINE = cch;

	NCattribute::setChar(cch, '-' );
	ACS_HLINE = cch;

	NCattribute::setChar(cch, '#' );
	ACS_DIAMOND = cch;
	ACS_CKBOARD = cch;
	ACS_BOARD = cch;

	NCattribute::setChar(cch, '<' );
	ACS_LARROW = cch;

	NCattribute::setChar(cch, '>' );
	ACS_RARROW = cch;   

	NCattribute::setChar(cch, 'v' );
	ACS_DARROW = cch;

	NCattribute::setChar(cch, '^' );
	ACS_UARROW = cch;   
    }
#if 0
    ret = stdpan->addch( 1, 1, ACS_HLINE);
    ret = stdpan->addch( 2, 1, ACS_VLINE );
    Redraw();
    ::getch();
#endif
    
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::style
//	METHOD TYPE : const NCstyle &
//
//	DESCRIPTION :
//
const NCstyle & NCurses::style()
{
  return *myself->styleset;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::Update
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCurses::Update()
{
  if ( myself && myself->initialized() ) {
    //myself->stdpan->refresh();
    myself->stdpan->redraw();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::Refresh
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCurses::Refresh()
{
  if ( myself && myself->initialized() ) {
    UIMIL << "start refresh ..." << endl;
    SetTitle( myself->title_t );
    ::clearok( ::stdscr, true );
    myself->stdpan->refresh();
    UIMIL << "done refresh ..." << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::Redraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCurses::Redraw()
{
  if ( myself && myself->initialized() ) {
    UIMIL << "start redraw ..." << endl;

    // initialize all dialogs rewdraw
    PANEL * pan = ::panel_above( NULL );
    while ( pan ) {
      NCDialog * dlg = NCursesUserPanel<NCDialog>::UserDataOf( *pan );
      if ( dlg ) {
	dlg->Recoded();
      }
      pan = ::panel_above( pan );
    }

    // TBD: initialize all dialogs rewdraw
    Refresh();
    UIMIL << "done redraw ..." << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::SetTitle
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCurses::SetTitle( const string & str )
{
  if ( myself && myself->title_w ) {
    myself->title_t = str;
    ::wbkgd( myself->title_w, myself->style()(NCstyle::AppTitle) );
    ::wclear( myself->title_w );

    NCMIL << "Draw title called" << endl;
    
    setTextdomain( "packages" );
    // part of title (headline) of the textmode yast
    NCstring helpF1 ( _( "Press F1 for Help" ) );
    NCtext textF1 ( helpF1 );

    int s = myself->title_w->_maxx - textF1.Columns();
    
    if ( NCstring::terminalEncoding() != "UTF-8" )
    {
	string out;
	NCstring::RecodeFromWchar( helpF1.str(), NCstring::terminalEncoding(), &out );
	::mvwaddstr( myself->title_w, 0, s, out.c_str() );
    }
    else
    {
	::mvwaddwstr( myself->title_w, 0, s, (wchar_t *)helpF1.str().c_str() );
    }
    ::mvwaddstr( myself->title_w, 0, 1, myself->title_t.c_str() );
    ::wnoutrefresh( myself->title_w );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::drawTitle
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCurses::drawTitle()
{
  if ( myself && myself->title_w ) {
    SetTitle( myself->title_t );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::RememberDlg
//	METHOD TYPE : void
//
void NCurses::RememberDlg( NCDialog * dlg_r )
{
  if ( dlg_r ) {
    _knownDlgs.insert( dlg_r );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::ForgetDlg
//	METHOD TYPE : void
//
void NCurses::ForgetDlg( NCDialog * dlg_r )
{
  if ( dlg_r ) {
    _knownDlgs.erase( dlg_r );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::RedirectToLog
//	METHOD TYPE : void
//
void NCurses::RedirectToLog()
{
  string log = get_log_filename();
  
  UIMIL << "isatty(stderr)" << (isatty(2) ? "yes" : "no") << endl;
  if (isatty(2) && theTerm) {
    // redirect stderr to log
    close(2);
    open(log.c_str(), O_APPEND | O_CREAT);
  }

  UIMIL << "isatty(stdout)" << (isatty(1) ? "yes" : "no") << endl;
  if (isatty(1) && theTerm) {
    // redirect stdout to log
    close(1);
    open(log.c_str(), O_APPEND | O_CREAT);
  }

}
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::ResizeEvent
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCurses::ResizeEvent()
{
  if ( myself && myself->initialized() ) {
    UIMIL << "start resize to " << NCurses::lines() << 'x' << NCurses::cols() << "..." << endl;

    // remember stack of visible dialogs.
    // don't hide on the fly, as it will mess up stacking order.
    list<NCDialog*> dlgStack;
    for ( PANEL * pan = ::panel_above( NULL ); pan; pan = ::panel_above( pan ) ) {
      NCDialog * dlg = NCursesUserPanel<NCDialog>::UserDataOf( *pan );
      if ( dlg ) {
	dlgStack.push_back( dlg );
      }
    }

    // hide all visible dialogs.
    for ( list<NCDialog*>::iterator it = dlgStack.begin(); it != dlgStack.end(); ++it ) {
      (*it)->getInvisible();
    }
    drawTitle();
    Update();

    // relayout all dialogs
    for ( set<NCDialog*>::iterator it = _knownDlgs.begin(); it != _knownDlgs.end(); ++it ) {
      (*it)->resizeEvent();
    }

    // recreate stack of visible dialogs
    for ( list<NCDialog*>::iterator it = dlgStack.begin(); it != dlgStack.end(); ++it ) {
      (*it)->getVisible();
    }
    Update();

    UIMIL << "done resize ..." << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCurses::ScreenShot
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCurses::ScreenShot( const string & name )
{
  if ( !myself )
    return;
  //ofstream out( name.c_str(), ios::out|ios::app );
  ostream & out( UIINT );
  int curscrlines = myself->title_line() ? lines() + 1 : lines();
  for ( int l = 0; l < curscrlines; ++l ) {
    for ( int c = 0; c < cols(); ++c ) {

      chtype al = ::mvwinch(::curscr,l,c) & (A_ALTCHARSET | A_CHARTEXT);
      if ( al & A_ALTCHARSET ) {
	if (    al == ACS_ULCORNER
	     || al == ACS_LLCORNER
	     || al == ACS_URCORNER
	     || al == ACS_LRCORNER
	     || al == ACS_LTEE
	     || al == ACS_RTEE
	     || al == ACS_BTEE
	     || al == ACS_TTEE
	     || al == ACS_PLUS ) {
	  out << '+';
	} else if ( al == ACS_HLINE ) {
	  out << '-';
	} else if ( al == ACS_VLINE ) {
	  out << '|';
	} else if (    al == ACS_DIAMOND
		    || al == ACS_CKBOARD
		    || al == ACS_BOARD ) {
	  out << '#';
	} else if ( al == ACS_LARROW ) {
	  out << '<';
	} else if ( al == ACS_RARROW ) {
	  out << '>';
	} else if ( al == ACS_DARROW ) {
	  out << 'v';
	} else if ( al == ACS_UARROW ) {
	  out << '^';
	} else {
	  out << (char)(al & A_CHARTEXT);
	}
      } else {
	out << (char)(al & A_CHARTEXT);
      }

    }
    out << endl;
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & STREAM, const NCurses & OBJ )
{
  STREAM << form( "NC - %d x %d - colors %d - pairs %d\n"
		  , OBJ.lines(), OBJ.cols()
		  , NCattribute::colors(), NCattribute::color_pairs() );

  WINDOW * cw = ::stdscr;
  STREAM << form( "NC - rootw %p", cw );
  if ( cw )
    STREAM << form( " - (%2hd,%2hd)%2hdx%2hd - {%p - (%2d,%2d)}\n"
		    , cw->_begy, cw->_begx
		    , cw->_maxy, cw->_maxx
		    , cw->_parent
		    , cw->_pary, cw->_parx
		    );
  else
    STREAM << endl;

  cw = OBJ.title_w;
  STREAM << form( "NC - title %p", cw );
  if ( cw )
    STREAM << form( " - (%2hd,%2hd)%2hdx%2hd - {%p - (%2d,%2d)}\n"
		    , cw->_begy, cw->_begx
		    , cw->_maxy, cw->_maxx
		    , cw->_parent
		    , cw->_pary, cw->_parx
		    );
  else
    STREAM << endl;

  return STREAM;
}
