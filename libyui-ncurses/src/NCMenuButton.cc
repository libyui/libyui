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

#define  YUILogComponent "ncurses"
#include <YUILog.h>
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
NCMenuButton::NCMenuButton( YWidget * parent,
			    string nlabel )
    : YMenuButton( parent, nlabel )
    , NCWidget( parent )
{
  yuiDebug() << endl;
  setLabel( nlabel );
  hotlabel = &label;
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
  yuiDebug() << endl;
}

int NCMenuButton::preferredWidth()
{
    return wGetDefsze().W;
}

int NCMenuButton::preferredHeight()
{
    return wGetDefsze().H;
}

void NCMenuButton::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YMenuButton::setEnabled( do_bv );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMenuButton::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMenuButton::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
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
void NCMenuButton::setLabel( const string & nlabel )
{
  label = NCstring( nlabel );
  label.stripHotkey();
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
  win->printw( 0, 0, "[" );
  win->printw( 0, win->maxx(), "]" );

  label.drawAt( *win, style, wpos(0,1), wsze(-1, win->width() - 3),
		NC::CENTER );

  win->bkgdset( style.scrl );
  win->vline( 0, win->maxx()-1, win->height(), ' ' );
  haveUtf8() ?
     win->add_wch( 0, win->maxx()-1, WACS_DARROW )
     : win->addch( 0, win->maxx()-1, ACS_DARROW );
}

#if 0
void NCMenuButton::createMenu()
{
  YMenu * toplevel = getToplevelMenu();
  setEnabled( toplevel && toplevel->hasChildren() );
}
#endif

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMenuButton::rebuildMenuTree
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMenuButton::rebuildMenuTree()
{
    // NOP
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
  NCPopupMenu * dialog = new NCPopupMenu( at,
					  itemsBegin(),
					  itemsEnd() );
  YUI_CHECK_NEW( dialog );
  
  int selection = dialog->post();

  if ( selection < 0 ) {
    YDialog::deleteTopmostDialog();  
    return NCursesEvent::none;
  }
  
  NCursesEvent ret = NCursesEvent::menu;
  // FIXME - check this
  // YStringWidgetID * selectionID = new YStringWidgetID( findMenuItem( selection )->label() );
  // ret.selection    = selectionID;

  ret.selection = findMenuItem( selection );
  
  YDialog::deleteTopmostDialog();

  return ret;
}

