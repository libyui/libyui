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

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "YDialog.h"
#include "NCDialog.h"
#include "NCurses.h"
#include "NCPushButton.h"



NCPushButton::NCPushButton( YWidget * parent, const string & nlabel )
    : YPushButton( parent, nlabel )
    , NCWidget( parent )
{
  yuiDebug() << endl;
  setWidgetRep(this);
  setLabel( nlabel );
  hotlabel = &label;
}



NCPushButton::~NCPushButton()
{
  yuiDebug() << endl;
}



int NCPushButton::preferredWidth()
{
    wsze defsize = wGetDefsze();
    return wGetDefsze().W;
}



int NCPushButton::preferredHeight()
{
    return wGetDefsze().H;
}



void NCPushButton::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YPushButton::setEnabled( do_bv );
}



void NCPushButton::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}



NCursesEvent NCPushButton::wHandleInput( wint_t key )
{
  NCursesEvent ret;
  switch ( key ) {
  case KEY_HOTKEY:
  case KEY_RETURN:
    ret = NCursesEvent::Activated;
    break;
  }
  return ret;
}



void NCPushButton::setLabel( const string & nlabel )
{
  label = NCstring( nlabel );
  label.stripHotkey();
  defsze = wsze( label.height(), label.width() + 2 );
  YPushButton::setLabel( nlabel );
  Redraw();
}




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

