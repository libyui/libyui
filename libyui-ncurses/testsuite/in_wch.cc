#include <stdio.h>
#include <errno.h>

#include <iostream>
#include <string>

#include <wchar.h>

#include <curses.h>


int add_attr_char( )
{
    int ret = ERR;
    cchar_t combined;
    ret = in_wch( &combined );

    if ( ret == OK )
    {
	ret = add_wch( &combined );
    }

    return ret;
}


int main (int argc, char *argv[])
{

    (void) initscr();      // initialize the curses library
    keypad(stdscr, TRUE);  // enable keyboard mapping 
    (void) nonl();         // tell curses not to do NL->CR/NL on output 
    (void) cbreak();       // take input chars one at a time, no wait for \n 
    
    (void) noecho();	   // echo OFF (don't echo input)
    nodelay(stdscr, FALSE); 
    int pos = 1;

    for (;;)
    {
	wint_t k;
	
        // waiting for input
        get_wch( &k );

	move( 1, pos++ );
	addch( ACS_CKBOARD );

	if ( k == KEY_LEFT )
	{
	    move( 1, pos-2 );
	    add_attr_char();
	}
	refresh();

    }

}
