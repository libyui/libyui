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

	    p1 ^= 0xAA55AA55;
	    p2 ^= 0xAA55AA55;
	    
	    fb.setPixel( x, y, p2 );
	    fb.setPixel( width-1 - x, height-1 - y, p1 );
	}
    }

#if 0

    // Crossed lines
    
    int x_offset = ( width-height ) / 2;
    
    for ( int m = 0; m < height; m++ )
    {
	int x = m + x_offset;
	
	for ( int i = -5; i < 5; i++ )
	{
	    int y = m;
	    YFrameBuffer::FBPixel px = fb.pixel( x+i, y );
	    fb.setPixel( x+i, y, px ^ 0xAA55AA55 );
	}
    }
    
    for ( int m = 0; m < height; m++ )
    {
	int x = width-1 - ( m + x_offset );
	
	for ( int i = -5; i < 5; i++ )
	{
	    int y = m;
	    YFrameBuffer::FBPixel px = fb.pixel( x+i, y );
	    fb.setPixel( x+i, y, px ^ 0xAA55AA55 );
	}
    }
#endif
    
    sleep( 7 );
}
