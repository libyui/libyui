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
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCImage.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCImage::NCImage
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCImage::NCImage( NCWidget * parent, const YWidgetOpt & opt,
		  YCPString defaulttext )
    : YImage( opt )
    , NCWidget( parent )
    , label( defaulttext )
{
  WIDDBG << endl;
  defsze = 0;
  wstate = NC::WSdumb;
  skipNoDimWin = true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCImage::~NCImage
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCImage::~NCImage()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCImage::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCImage::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCImage::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCImage::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YImage::setSize( newwidth, newheight );
}
