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


struct q
{
  int x;	// center coordiantes
  int y;	// center coordiantes
  int rx;	// radius-x
  int ry;	// radius-y
  int state;	// a random value, used for varous decisions.
};

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

    QDesktopWidget * screen = QApplication::desktop();
    int width  = screen->width();
    int height = screen->height();
    YFrameBuffer fb( width, height, screen->x11AppDepth() );

    draw(&fb, width, height);
}

// in_corner returns true, if we are in a rounded corner of the rectangle
static int in_corner(int x, int y, int xmin, int ymin, int xmax, int ymax)
{
  int dx1 = x - xmin;
  int dx2 = xmax - x;
  int dy1 = y - ymin;
  int dy2 = ymax - y;
  int dx = (dx1 > dx2) ? dx2 : dx1;
  int dy = (dy1 > dy2) ? dy2 : dy1;

  int d = (dx < dy) ? dx : dy;	// okay, not really a radius, but close enough ...

  if (d + dx + dy < 10) return 1;
  return 0;
}

// clip variable inside a range.
#define RANGE(var, min, max) (((var) < (min)) ? (min) : (((var) > (max)) ? (max) : (var)))

// return a random integer [0..range]
#define MY_RAND(range) ((int)(1.0*(range)*rand()/(RAND_MAX+1.0)))

static void draw_one (YFrameBuffer *fb, int sw, int sh, struct q *a)
{
  int xmin = RANGE(a->x - a->rx, 0, sw-1);
  int xmax = RANGE(a->x + a->rx, 0, sw-1);
  int ymin = RANGE(a->y - a->ry, 0, sh-1);
  int ymax = RANGE(a->y + a->ry, 0, sh-1);
  int xstep = (a->state & 0x400) ? 1 : 2;
  int ystep = (a->state & 0x800) ? 1 : 2;

  // flip the rectangle 180 degrees around.
  // FIXME: horizontal center line of the rectangle is not 
  // handled correctly. 
  // Our rectangles have an even width and height unless
  // they hit borders. jw.
  //
  for ( int y = (ymax-ymin)/2; y >0; y -= ystep )
    {
	for ( int x = xmax-xmin; x>0; x -= xstep )
	{
	    if (in_corner(x+xmin, y+ymin, xmin, ymin, xmax, ymax))
	      continue;

	    YFrameBuffer::FBPixel p1, p2;

	    p1 = fb->pixel( xmin+x, ymin+y );
	    p2 = fb->pixel( xmax-x, ymax-y );

	    // some rectangles are even color-inverted
            if (a->state & 1)
	      {
                if (a->state & 2)
	          {
		    // this is greenish. 
	            p1 ^= 0xAA55AA55;
	            p2 ^= 0xAA55AA55;
		  }
		else
		  {
		    // this is random
		    p1 ^= a->state;
		    p2 ^= a->state;
		  }
	      }
	    
	    fb->setPixel( xmin+x, ymin+y, p2 );
	    fb->setPixel( xmax-x, ymax-y, p1 );
	}
    }
}

static void draw_all(YFrameBuffer *fb, int sw, int sh, struct q *a, int n)
{
  for (int i = 0; i < n; i++)
    {
      draw_one(fb, sw, sh, a++);
    }
}


void
YQEasterBunny::draw(YFrameBuffer *fb, int sw, int sh)
{
//  int n = sizeof(a) / sizeof (struct q);
  int n = sw/2;

  struct q *a = (struct q *)malloc(sizeof(struct q) * n);
  struct q *b = (struct q *)malloc(sizeof(struct q) * n);

#if 0
  for (int i = 0; i < n; i++)
    {
      a[i].x += sw/2;
      a[i].y += sh/2;
      *(b+n-1-i) = a[i];
    }
#endif
 
  srand(getpid() ^ time(0));

  // to find, where the center of rectangle nr. i is, we draw an invisible square of 
  // 'radius' i+50 around the center of the screen. the random generator selects a position
  // on the border of that square. 
  // this has the effect that rectangles drawn later are fürther away from the center.
  // an undesired sideeffect is, that at an earlier stage,  
  // all rectangles are cuddled towards the center.
  // dx and dy introduce a special case for the first few (one eight of all) rectangles.
  // for them we shift the center-square towards one of the four corners of the screen.
  
  for (int i = 0; i < n; i++)
    {
      int t = i+50;
      int r = -t + MY_RAND(2*t);
      int x = sw/2;
      int y = sh/2;

      int dx = (n-8*i);
      if (dx < 0) dx = 0;
      int dy = dx * sh / sw;

      int state = rand();
      if (state & 0x100)
        {
	  if (state & 0x200)
	    {
	      // top row
	      x +=  r - dx;
	      y += -t - dy;
	    }
	  else
	    {
	      // left col
	      x += -t - dx;
	      y +=  r + dy;
	    }
	}
      else
        {
	  if (state & 0x200)
	    {
	      // bottom row
	      x += r + dx;
	      y += t - dy;
	    }
	  else
	    {
	      // right col
	      x += t + dx;
	      y += r + dy;
	    }
	}

      (a+i)->x = x;
      (a+i)->y = y;
      (a+i)->state = state;

      // most of the recangles are in a fairly small size. 
      // but a few of them are huge.
      // note, that only semi-transparent rctangles can become huge :-)
      // jw.

      (a+i)->rx = (20 + MY_RAND(80)) * ((state & 0x3f00) ? 1 : 4);
      (a+i)->ry = (15 + MY_RAND(60)) * ((state & 0x3f00) ? 1 : 4);
      *(b+n-1-i) = *(a+i);
    }
 
  draw_all(fb, sw, sh, a, n);
  draw_all(fb, sw, sh, b, n);
}
