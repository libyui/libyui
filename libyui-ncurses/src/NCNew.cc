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

   File:       NCNew.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCNew.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCNew::NCNew
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCNew::NCNew( NCWidget * parent, YWidgetOpt & opt,
		  const YCPString & nlabel )
    : YWidget( opt )
    , NCWidget( parent )
    , heading( opt.isHeading.value() )
    , label( nlabel )
{
  WIDDBG << endl;
  hotlabel = &label;
  setLabel( nlabel );
  wstate = NC::WSdumb;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCNew::~NCNew
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCNew::~NCNew()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCNew::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCNew::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCNew::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCNew::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  //YLabel::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCNew::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCNew::setLabel( const YCPString & nlabel )
{
  label  = NCstring( nlabel );
  defsze = label.size();
  //YLabel::setLabel( nlabel );
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCNew::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCNew::wRedraw()
{
  if ( !win )
    return;

  chtype bg = heading ? wStyle().dumb.title
		      : wStyle().dumb.text;
  win->bkgd( bg );
  win->clear();
  label.drawAt( *win, bg, bg );
}

