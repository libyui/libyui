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

   File:       NCLabel.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCLabel.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLabel::NCLabel
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCLabel::NCLabel( NCWidget * parent, const YWidgetOpt & opt,
		  const YCPString & nlabel )
    : YLabel( opt, nlabel )
    , NCWidget( parent )
    , heading( opt.isHeading.value() )
{
  WIDDBG << endl;
  setLabel( nlabel );
  hotlabel = &label;
  wstate = NC::WSdumb;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLabel::~NCLabel
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCLabel::~NCLabel()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLabel::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCLabel::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLabel::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLabel::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YLabel::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLabel::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLabel::setLabel( const YCPString & nlabel )
{
  label  = NCstring( nlabel );
  NCDBG << "LABEL: " << NCstring(nlabel) << " Longest line: " << label.width()<< endl;
  defsze = label.size();
  YLabel::setLabel( nlabel );
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLabel::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLabel::wRedraw()
{
  if ( !win )
    return;

  chtype bg = heading ? wStyle().dumb.title
		      : wStyle().dumb.text;
  win->bkgd( bg );
  win->clear();
  label.drawAt( *win, bg, bg );
}

