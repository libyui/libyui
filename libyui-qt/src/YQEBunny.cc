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

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-easter-bunny"
#include <ycp/y2log.h>

#include <qapplication.h>
#include <qdesktopwidget.h>

#include "YFrameBuffer.h"
#include "YQEBunny.h"



void
YQEasterBunny::layEgg()
{
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

    draw();
}


void
YQEasterBunny::draw()
{
    QDesktopWidget * screen = QApplication::desktop();
    int width  = screen->width();
    int height = screen->height();
    YFrameBuffer fb( width, height, screen->x11AppDepth() );

    for ( int y = 0; y < height/2; y++ )
    {
	for ( int x = 0; x < width; x++ )
	{
	    YFrameBuffer::FBPixel p1, p2;

	    p1 = fb.pixel( x, y );
	    p2 = fb.pixel( width-1 - x, height-1 - y );
	    fb.setPixel( x, y, p2 );
	    fb.setPixel( width-1 - x, height-1 - y, p1 );
	}
    }

    sleep( 7 );
}
