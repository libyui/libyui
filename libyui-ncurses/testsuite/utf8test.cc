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

   File:       utf8test.cc

   Author:     Gabriele Strattner (gs@suse.de)

/-*/

#include <stdio.h>
#include <errno.h>

#include <ycp/y2log.h>

#include <iostream>
#include <string>

#include <wchar.h>

#include <curses.h>	/* curses.h: #define  NCURSES_CH_T cchar_t */

#include "NCstring_test.h"

#define typeMalloc(type,n) (type *) malloc((n) * sizeof(type))

using std::string;
using std::wstring;

extern int yydebug;

using namespace std;

//------------------------------------------------------
// gettext
//------------------------------------------------------

#define TEXTDOMAIN	"testdomain"
#define LOCALEDIR	"/usr/share/locale"

//------------------------------------------------------


static void finish(int sig)
{
    endwin();
    /* do your non-curses wrapup here */
    exit(0);
}


//
// main
// 
int main (int argc, char *argv[])
{
    string text = "";
    string language = "";
    int argp = 1;
    bool use_utf8 = false;
    
    if (argc > 1)
    {
	while (argp < argc)
	{
	    if ((argv[argp][0] == '-')
	        && (argv[argp][1] == 'l')
	        && (argp+1 < argc))
	    {
		argp++;
		y2setLogfileName (argv[argp]);
	    }
	    else if (text == "")
	    {
		text = argv[argp];
	    }
	    else if ( language == "" )
	    {
		language = argv[argp];
	    }
	    else
	    {
		fprintf (stderr, "Bad argument '%s'\nUsage: utf8test text_to_translate language\n",
			 argv[argp]);
		exit(1);
	    }
	    argp++;
	}
    }
    else
    {
	fprintf( stderr, "Usage: utf8test text_to_translate language\n" );
	exit(1);
    }

    //
    // WFM parts
    //

    if ( setlocale(LC_ALL, "") == NULL )
    {
	fprintf(stderr,"warning: locale not supported by libc\n");
    }

    bindtextdomain( TEXTDOMAIN, LOCALEDIR);

    // codeset of the terminal (as long as not yet changed by a start script or
    // a setenv( LANG ) call)
    char *codeset;

    codeset = nl_langinfo( CODESET );
    cout << "Initial codeset: " << codeset << endl;
    
    // Explicitly set LC_CTYPE to this codeset because the terminal runs with this codeset
    // and must not be changed by setting the LANG variable.
    // export LANG= ... or setenv( LANG ) changes all LC_... variables accordingly except those
    // explicitly set before.
    // Functions which are respecting the locale e.g. wcwidth() are looking for the LC_CTYPE.
    // The LC_CTYPE must be set to the language e.g.: de_DE, de_DE@euro, de_DE.UTF-8 and NOT
    // to the codeset got from nl_langinfo( CODESET )!

    if ( getenv( "LANG" ) != NULL )
    {
	language = getenv( "LANG" );
    }

    if ( setlocale( LC_CTYPE, language.c_str() ) == NULL )
    {
	fprintf(stderr, "error: setlocale %s\n", language.c_str() );	
    }
    
    if ( strcmp( codeset, "UTF-8") == 0 )
    {
	use_utf8 = true;
    }

    // always use UTF-8 for gettext() !!!
    bind_textdomain_codeset ( TEXTDOMAIN, "UTF-8" );

    textdomain ( TEXTDOMAIN );

    //
    // TEST: LC_CTYPE is not changed by setenv () if explicitly set
    //
    setenv ( "LANG", "cs_CZ", 1 );
    cout << "LANG is changed to: " <<  getenv( "LANG" ) << " - but codeset remains: "
	 << nl_langinfo( CODESET ) << endl;

    // BUT another setlocale( LC_ALL, language ) call would change all LC_... variables
    // including the LC_CTYPE! 

    //
    // TEST: RecodeToWchar() / RecodeFromWchar()
    //
    bool ok;
    wstring wtext;

    // TEST cases:
    // (all testfiles are UTF-8 encoded)
    // - utf8test `cat testfile.cs` in cs_CZ.UTF-8 locale -> OK
    // - utf8test `cat testfile.cs` in de_DE@euro locale -> RecodeFromWchar Not OK (terminal cannot show czech)
    // - utf8test `cat testfile.cs` in cs_CZ locale -> OK 
    // - utf8test `cat testfile.de` in de_DE@euro locale -> OK
    //
    // - utf8test öööäää            in de_DE@euro  locale  -> RecodeToWchar Not OK (input not UTF-8)
    //
    ok = NCstring_test::RecodeToWchar( text, "UTF-8", &wtext ); // INPUT is always UTF-8
    fprintf( stderr, "RecodeToWchar(): %ls\n", wtext.c_str());

    if ( ok )
    {
	if ( use_utf8 )
	{
	    fprintf( stderr, "UTF-8 terminal -> wstring not recoded: %ls\n", wtext.c_str());
	}
	else
	{
	    string recoded;
	    bool ok = NCstring_test::RecodeFromWchar( wtext, codeset, &recoded );
	    if (ok )
	    {
		fprintf( stderr, "%s terminal -> RecodeFromWchar(): %s\n", codeset, recoded.c_str());
	    }
	    else
	    {
		fprintf( stderr, "wstring NOT recoded" );
	    }
	}
    }
    else
    {
	fprintf( stderr, "Cannot create wstring\n");
    }

    //--------------------------------------------------------------------------- 
    // Curses INIT
    //---------------------------------------------------------------------------

    (void) initscr();      // initialize the curses library
    keypad(stdscr, TRUE);  // enable keyboard mapping 
    (void) nonl();         // tell curses not to do NL->CR/NL on output 
    (void) cbreak();       // take input chars one at a time, no wait for \n 
    
    (void) noecho();	   // echo OFF (don't echo input)
    nodelay(stdscr, FALSE);   // delay mode  FALSE: get_wch waits for an input
                              //             TRUE: no celay
    if (has_colors())
    {
        start_color();
	/* Simple color assignment, often all we need.  Color pair 0 cannot
	 * be redefined. */
	init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_GREEN,   COLOR_BLACK);
        init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
    }

    //-----------------------------------------------------------------------------
    
    int line = 1;
    size_t i;
    
    NCstring_test textinput;
    
    textinput = text;
    
    for (;;)
    {
	wint_t k;

	// waiting for input
        get_wch( &k );

	line = line+2;
	mvprintw( line, 1, "The argument (converted in wstring): \n" );
	
	for ( i = 0; i <= textinput.str().length() ; i++ )
	{
	    wchar_t out[CCHARW_MAX+1];
	    out[0] =  textinput.str()[i];
	    out[1] = L'\0';
	    addwstr( out );	
	}
	
	size_t outwidth = 0;
	for ( i = 0; i <= textinput.str().length() ; i++ )
	{
	    // The behaviour of wcwidth depends on the LC_CTYPE  category
	    // of the current locale.
	    // That means width is correct for UTF-8 text on UTF-8 terminal
	    // and UTF-8 text with luit on terminal with encoding corresponding
	    // to the language of input and for text of encoding which
	    // corresponds encoding of the terminal
	    outwidth += wcwidth( textinput.str()[i] );
	}
	line = line+2;
	mvprintw( line, 1, "Width on the terminal %d:\n", outwidth );

	refresh();

    }


    finish(0);               /* we're done */	
}
