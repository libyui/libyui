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

   File:       NCMenuButton.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCMenuButton.h"
#include "NCPopupMenu.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMenuButton::NCMenuButton
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCMenuButton::NCMenuButton( NCWidget * parent, YWidgetOpt & opt,
			    YCPString nlabel )
    : YMenuButton( opt, nlabel )
    , NCWidget( parent )
    , label( nlabel )
{
  WIDDBG << endl;
  hotlabel = &label;
  setLabel( nlabel );
  if ( opt.isDefaultButton.value() )
    setKeyboardFocus();

  setFunctionHotkey( opt );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMenuButton::~NCMenuButton
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCMenuButton::~NCMenuButton()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMenuButton::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCMenuButton::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMenuButton::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMenuButton::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YMenuButton::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMenuButton::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCMenuButton::wHandleInput( wint_t key )
{
  NCursesEvent ret;
  switch ( key ) {
  case KEY_HOTKEY:
  case KEY_SPACE:
  case KEY_RETURN:
  case KEY_DOWN:
    ret = postMenu();
    break;
  }
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMenuButton::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMenuButton::setLabel( const YCPString & nlabel )
{
  label = NCstring( nlabel );
  defsze = wsze( label.height(), label.width() + 3 );
  YMenuButton::setLabel( nlabel );
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMenuButton::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMenuButton::wRedraw()
{
  if ( !win )
    return;
  const NCstyle::StWidget & style( widgetStyle() );

  win->bkgdset( style.plain );
  if ( label.height() > 1 ) {
    win->box( wrect(0,win->size()-wsze(0,1)) );
  }
  win->printw( 0, 0, "(" );
  win->printw( 0, win->maxx()-1, ")" );

  label.drawAt( *win, style, wpos(0,1), wsze(-1, win->width() - 3),
		NC::CENTER );

  win->bkgdset( style.scrl );
  win->vline( 0, win->maxx(), win->height(), ' ' );
  win->addch( 0, win->maxx(), ACS_DARROW );

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMenuButton::createMenu
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMenuButton::createMenu()
{
  YMenu * toplevel = getToplevelMenu();
  setEnabling( toplevel && toplevel->hasChildren() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMenuButton::postMenu
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCMenuButton::postMenu()
{
  wpos at( ScreenPos() + wpos( win->height(), 0 ) );
  NCPopupMenu dialog( at, *getToplevelMenu() );
  int selection = dialog.post();

  if ( selection < 0 )
    return NCursesEvent::none;

  NCursesEvent ret = NCursesEvent::menu;
  ret.selection    = indexToId( selection );

  return ret;
}

