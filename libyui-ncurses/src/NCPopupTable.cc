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

   File:       NCPopupTable.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupTable.h"

#include "NCTable.h"
#include "YMenuButton.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTable::NCPopupTable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupTable::NCPopupTable( const wpos at )
    : NCPopup( at, false )
    , sellist( 0 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTable::~NCPopupTable
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupTable::~NCPopupTable()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTable::createList
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupTable::createList( vector<string> & row )
{
  if ( sellist )
    return;

  YWidgetOpt opt;
  opt.notifyMode.setValue( true );

  sellist = new NCTable( this, opt, row, false );
  sellist->setBigList( true );
  sellist->SetSepChar( ' ' );
  sellist->SetSepWidht( 0 );
  sellist->SetHotCol( 0 );

  addChild( sellist );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTable::addItem
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupTable::addItem( const YCPValue & id, vector<string> & row )
{
  if ( !sellist )
    return;

  sellist->addItem( id, row );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTable::setCurrentItem
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupTable::setCurrentItem( int index )
{
  if ( !sellist )
    return;

  sellist->setCurrentItem( index );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTable::getCurrentItem
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCPopupTable::getCurrentItem() const
{
  if ( !sellist )
    return -1;

  return sellist->getCurrentItem();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTable::wHandleHotkey
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupTable::wHandleHotkey( wint_t key )
{
  if ( key >= 0 && sellist->setItemByKey( key ) )
    return wHandleInput( KEY_RETURN );

  return NCursesEvent::none;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupTable::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupTable::postAgain()
{
  postevent.detail = (postevent == NCursesEvent::button) ? sellist->getCurrentItem()
							 : NCursesEvent::NODETAIL;
  return false;
}

void NCPopupTable::stripHotkeys()
{
    if(sellist) {
	sellist->stripHotkeys();
    }
}
