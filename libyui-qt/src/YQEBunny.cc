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

  File:	      YQEBunny.cc

  Authors:     	Stefan Hundhammer <sh@suse.de>
  		Juergen Weigert <jw@suse.de> 
		
		Rectangles! Rounded rectangles everywhere, ...
		A horror vision of the new CI.

/-*/


#define y2log_component "qt-easter-bunny"
#include <ycp/y2log.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include <qapplication.h>
#include <qdesktopwidget.h>

#include "YQEBunny.h"


void
YQEasterBunny::layEgg()
{
#if 0
    const char * display = getenv( "DISPLAY" );

    if ( ! display )
    {
	y2warning( "$DISPLAY not set - no easter egg" );
	return;
    }

    if ( strcmp( display, ":0"   ) != 0 &&
	 strcmp( display, ":0.0" ) != 0   )
    {
	y2warning( "Not on system console - no easter egg" );
	return;
    }

    QDesktopWidget * screen = QApplication::desktop();
    int width  = screen->width();
    int height = screen->height();
    YFrameBuffer fb( width, height, screen->x11AppDepth() );

    draw(&fb, width, height);
#else
    y2warning( "Easter egg disabled" );
#endif
}


void
YQEasterBunny::draw(YFrameBuffer *fb, int sw, int sh)
{
    // NOP
}
