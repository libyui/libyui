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

   File:       NCImage.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCImage.h"



NCImage::NCImage( YWidget * parent, string defaulttext, bool animated )
    : YImage( parent, defaulttext, animated )
    , NCWidget( parent )
    , label( defaulttext )
{
  yuiDebug() << endl;
  defsze = 0;
  wstate = NC::WSdumb;
  skipNoDimWin = true;
}



NCImage::~NCImage()
{
  yuiDebug() << endl;
}



long NCImage::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

int NCImage::preferredWidth()
{
     return wGetDefsze().W;
}

int NCImage::preferredHeight()
{
    return wGetDefsze().H;
}

void NCImage::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YImage::setEnabled( do_bv );
}



void NCImage::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}
