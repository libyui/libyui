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

   File:       NCPushButton.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCPushButton.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPushButton::NCPushButton
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPushButton::NCPushButton( NCWidget * parent, YWidgetOpt & opt,
			    YCPString nlabel )
    : YPushButton( opt, nlabel )
    , NCWidget( parent )
    , label( nlabel )
{
  WIDDBG << endl;
  hotlabel = &label;
  setLabel( nlabel );
  if ( opt.isDefaultButton.value() )
    setKeyboardFocus();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPushButton::~NCPushButton
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPushButton::~NCPushButton()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPushButton::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCPushButton::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPushButton::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPushButton::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YPushButton::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPushButton::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPushButton::wHandleInput( int key )
{
  NCursesEvent ret;
  switch ( key ) {
  case KEY_HOTKEY:
  case KEY_RETURN:
    ret = NCursesEvent::button;
    break;
  }
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPushButton::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPushButton::setLabel( const YCPString & nlabel )
{
  label = NCstring( nlabel );
  defsze = wsze( label.height(), label.width() + 2 );
  YPushButton::setLabel( nlabel );
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPushButton::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPushButton::wRedraw()
{
  if ( !win )
    return;


  const NCstyle::StWidget & style( widgetStyle() );
  win->bkgd( style.plain );
  win->clear();
  if ( label.height() <= 1 ) {
    win->printw( 0, 0, "[" );
    win->printw( 0, win->maxx(), "]" );
  } else {
    win->box();
  }
  label.drawAt( *win, style, wpos(0,1), wsze(-1, win->width() - 2),
		NC::CENTER );
}

