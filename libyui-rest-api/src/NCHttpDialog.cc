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

   File:       NCDialog.cc

   Author:     Michael Andres <ma@suse.de>

just to make the y2makepot script happy:
textdomain "ncurses"

/-*/

#define	 YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCHttpDialog.h"

#include "NCstring.h"
#include "NCPopupInfo.h"
#include "NCMenuButton.h"
#include <yui/YShortcut.h>
#include "NCtoY2Event.h"
#include "YNCHttpUI.h"
#include "NCHttpDialog.h"
#include "YHttpServer.h"
#include <yui/YDialogSpy.h>
#include <yui/YDialog.h>
 #include <chrono>

#include "ncursesw.h"

NCHttpDialog::NCHttpDialog( YDialogType		dialogType,
		            YDialogColorMode	colorMode )
    : NCDialog( dialogType, colorMode )
{
    yuiDebug() << "Constructor NCHttpDialog(YDialogType t, YDialogColorMode c)" << std::endl;
}

static int wait_for_input(int timeout_millisec)
{
    struct timeval tv;
    fd_set fdset_read, fdset_write, fdset_excpt;

    // infinite timout => do blocking select()
    timeval *tv_ptr = (timeout_millisec < 0) ? nullptr : &tv;

    // remember the original value
    int timeout_millisec_orig = timeout_millisec;

    do
    {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        tv.tv_sec  = 0;
        tv.tv_usec = timeout_millisec * 1000;

        FD_ZERO( &fdset_read );
        FD_ZERO( &fdset_write );
        FD_ZERO( &fdset_excpt );
        FD_SET( 0,	&fdset_read );

        // the higest fd number to watch
        int fd_max = 0;

        // watch HTTP server fd
        yuiMilestone() << "Adding HTTP server notifiers NC dialog..." << std::endl;
        YHttpServerSockets sockets = YHttpServer::yserver()->sockets();

        for(int fd: sockets.read())
        {
            FD_SET( fd, &fdset_read );
            if (fd_max < fd) fd_max = fd;
        }

        for(int fd: sockets.write())
        {
            FD_SET( fd, &fdset_write );
            if (fd_max < fd) fd_max = fd;
        }

        for(int fd: sockets.exception())
        {
            FD_SET( fd, &fdset_excpt );
            if (fd_max < fd) fd_max = fd;
        }

        yuiWarning() << "Calling select()... " << std::endl;
        int retval = select( fd_max + 1, &fdset_read, &fdset_write, &fdset_excpt, tv_ptr );
        yuiWarning() << "select() result: " << retval << std::endl;

        if ( retval < 0 )
        {
            if ( errno != EINTR )
                yuiError() << "error in select() (" << errno << ')' << std::endl;
        }
        else if ( retval != 0 )
        {
            YHttpServerSockets sockets = YHttpServer::yserver()->sockets();
            bool server_ready = false;

            for(int fd: sockets.read())
            {
                if (FD_ISSET( fd, &fdset_read ))
                    server_ready = true;
            }

            for(int fd: sockets.write())
            {
                if (FD_ISSET( fd, &fdset_write ))
                    server_ready = true;
            }

            for(int fd: sockets.exception())
            {
                if (FD_ISSET( fd, &fdset_excpt ))
                    server_ready = true;
            }

            yuiWarning() << "Server ready: " << server_ready << std::endl;

            if (server_ready)
            {
                bool redraw = YHttpServer::yserver()->process_data();

                if (timeout_millisec > 0)
                {
                    std::chrono::steady_clock::time_point finish = std::chrono::steady_clock::now();

                    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
                    yuiWarning() << "Elapsed time (ms): " << elapsed << std::endl;
                    timeout_millisec -= elapsed;
                    yuiWarning() << "Remaining time out (ms): " << timeout_millisec << std::endl;

                    // spent too much time
                    if (timeout_millisec <= 0)
                        timeout_millisec = 0;
                }

                // the request might have changed something in the UI, let's redraw it...
                if (redraw)
                    NCurses::Redraw();

                // any input added by the server call?
                ::nodelay( ::stdscr, true );
                int	ch = ::getch();
                if (ch != ERR)
                {
                    yuiWarning() << "Input available" << std::endl;
                    // put it back and finish
                    ::ungetch(ch);
                    return 0;
                }
            }
        }
        // no input within timeout
        else
        {
            yuiWarning() << "Timeout " << timeout_millisec << "ms reached" << std::endl;
            return timeout_millisec_orig;
        }
    }
    while ( !FD_ISSET( 0, &fdset_read ) );

    // if there is an user input we do not need the spent time
    return 0;
}


wint_t NCHttpDialog::getch( int timeout_millisec )
{
    wint_t got = WEOF;

    yuiWarning() << "NCHttpDialog::getch timeout: " << timeout_millisec << std::endl;

    if ( timeout_millisec < 0 )
    {
	   // wait for input (block)
	   wait_for_input(timeout_millisec);
	   got = getinput();
    }
    else if ( timeout_millisec )
    {
        do
        {
            // wait for input
            int slept = wait_for_input(timeout_millisec);
            yuiWarning() << "slept: " << slept << std::endl;
            timeout_millisec -= slept;
            yuiWarning() << "new timeout: " << timeout_millisec << std::endl;

            got = getinput();
        }
        while ( got == WEOF && timeout_millisec > 0 );
    }
    else
    {
	    // no wait (non blocking)
	    wait_for_input(0);
	    ::nodelay( ::stdscr, true );
	    got = getinput();
    }

    if ( got == KEY_RESIZE )
    {
	NCurses::ResizeEvent();
	int i = 100;
	// after resize sometimes WEOF is returned -> skip this in no timeout mode

	do
	{
	    got =  NCHttpDialog::getch( timeout_millisec );
	}
	while ( timeout_millisec < 0 && got == WEOF && --i );
    }

    return got;
}
