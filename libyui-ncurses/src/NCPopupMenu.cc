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

   File:       NCPopupMenu.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupMenu.h"

#include "NCTable.h"
#include "YMenuButton.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupMenu::NCPopupMenu
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupMenu::NCPopupMenu( const wpos at, YMenuItem & menuitem )
    : NCPopupTable( at )
    , menu( menuitem )
{
  vector<string> row( 2 );
  createList( row );
  for ( YMenuItemListIterator entry = menu.itemList().begin();
 	entry != menu.itemList().end(); ++entry ) {
    row[0] = (*entry)->getLabel()->value();
    row[1] = (*entry)->hasChildren() ? "..." : "";
    addItem( (*entry)->getId(), row );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupMenu::~NCPopupMenu
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupMenu::~NCPopupMenu()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupMenu::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupMenu::wHandleInput( wint_t ch )
{
  NCursesEvent ret;
  switch ( ch ) {
  case KEY_RIGHT:
    if ( menu.itemList()[getCurrentItem()]->hasChildren() )
      ret = NCursesEvent::button;
    break;
  case KEY_LEFT:
    ret = NCursesEvent::cancel;
    ret.detail = NCursesEvent::CONTINUE;
    break;
  default:
    ret = NCPopup::wHandleInput( ch );
    break;
  }
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupMenu::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupMenu::postAgain()
{
  // dont mess up postevent.detail here
  bool again = false;
  int  selection = ( postevent == NCursesEvent::button ) ? getCurrentItem()
                                                         : -1;

  if ( selection != -1 ) {
    if ( menu.itemList()[selection]->hasChildren() ) {
      // post submenu
      wpos at( ScreenPos() + wpos( selection, inparent.Sze.W - 1 ) );
      NCPopupMenu dialog( at, *menu.itemList()[selection] );
      again = (dialog.post( &postevent ) == NCursesEvent::CONTINUE);
    } else {
      // store selection
      postevent.detail = menu.itemList()[selection]->getIndex();
    }
  }
  return again;
}
