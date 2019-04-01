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
   File:       YNCursesUI.cc
   Author:     Michael Andres <ma@suse.de>
just to make the y2makepot script happy:
textdomain "ncurses"
/-*/

#include "YNCHttpUI.h"
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <langinfo.h>

#include <yui/YButtonBox.h>
#include <yui/YCommandLine.h>
#include <yui/YDialog.h>
#include "NCDialog.h"
#include "NCHttpDialog.h"
#include "NCBusyIndicator.h"
#include <yui/YEvent.h>
#include <yui/YMacro.h>
#include <yui/YUI.h>
#include "YHttpServer.h"

#define YUILogComponent "ncurses"
#include <yui/YUILog.h>

#include "NCstring.h"
#include "NCWidgetFactory.h"
#include "NCOptionalWidgetFactory.h"
#include "NCPackageSelectorPluginStub.h"
#include "NCPopupTextEntry.h"
#include "NCi18n.h"

YNCHttpUI * YNCHttpUI::_ui = 0;

YNCHttpUI::YNCHttpUI( bool withThreads )
    : YNCursesUI( withThreads, false )
{
    yuiMilestone() << "YNCHttpUI constructor start" << std::endl;
    yuiMilestone() << "This is libyui-ncurses with http " << VERSION << std::endl;
    _ui	= this;
    yuiMilestone() << "YNCHttpUI constructor finished" << std::endl;
    topmostConstructorHasFinished();
}

extern YUI * createYNCHttpUI( bool withThreads )
{
    if ( ! YNCHttpUI::ui() )
	new YNCHttpUI( withThreads );

    return YNCHttpUI::ui();
}

void YNCHttpUI::idleLoop( int fd_ycp )
{
    int	   timeout = 5;

    struct timeval tv;
    fd_set fdset_read, fdset_write, fdset_excpt;
    int	   retval;

    do
    {
        tv.tv_sec  = timeout;
 	tv.tv_usec = 0;

 	FD_ZERO( &fdset_read );
 	FD_ZERO( &fdset_write );
 	FD_ZERO( &fdset_excpt );
 	FD_SET( 0,	&fdset_read );
 	FD_SET( fd_ycp, &fdset_read );

        // the higest fd number to watch
        int fd_max = fd_ycp;

         // watch HTTP server fd
         yuiMilestone() << "Adding HTTP server notifiers..." << std::endl;
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
 	 retval = select( fd_max + 1, &fdset_read, &fdset_write, &fdset_excpt, &tv );
         yuiWarning() << "select() result: " << retval << std::endl;

         if ( retval < 0 )
         {
             if ( errno != EINTR )
 	           yuiError() << "idleLoop error in select() (" << errno << ')' << std::endl;
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
                 YHttpServer::yserver()->process_data();

 	    //do not throw here, as current dialog may not necessarily exist yet
 	    //if we have threads
 	    YDialog *currentDialog = YDialog::currentDialog( false );

 	    if ( currentDialog )
 	    {
 		NCHttpDialog * ncd = static_cast<NCHttpDialog *>( currentDialog );

 		if ( ncd )
 		{
 		    extern NCBusyIndicator* NCBusyIndicatorObject;

 		    if ( NCBusyIndicatorObject )
 			NCBusyIndicatorObject->handler( 0 );

 		    ncd->idleInput();
 		}
 	    }
 	} // else no input within timeout sec.
    }
    while ( !FD_ISSET( fd_ycp, &fdset_read ) );
}
