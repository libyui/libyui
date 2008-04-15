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

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCPopupTable.h"

#include "NCTable.h"
#include "YMenuButton.h"



NCPopupTable::NCPopupTable( const wpos at )
    : NCPopup( at, false )
    , sellist( 0 )
{
}



NCPopupTable::~NCPopupTable()
{
}



void NCPopupTable::createList( vector<string> & row )
{
  if ( sellist )
    return ;

  YTableHeader * tableHeader = new YTableHeader();
  // sellist =  new NCTable( this, row.size() );
  sellist = new NCTable( this, tableHeader );
  YUI_CHECK_NEW( sellist );
  
  sellist->setBigList( true );
  sellist->SetSepChar( ' ' );
  sellist->SetSepWidth( 0 );
  sellist->SetHotCol( 0 );
  sellist->setNotify( true );


}



void NCPopupTable::addItem( YItem *yitem )
{
  if ( !yitem )
    return;

  sellist->addItem( yitem );

  // Calling sellist->addItem() resets the hotcol because
  // NCTableStyle's constructor sets hotcol to -1. 
  // Set hot coll again:
  sellist->SetHotCol( 0 );
}



void NCPopupTable::setCurrentItem( int index )
{
  if ( !sellist )
    return;

  sellist->setCurrentItem( index );
}



int NCPopupTable::getCurrentItem() const
{
  if ( !sellist )
    return -1;

  return sellist->getCurrentItem();
}

YItem * NCPopupTable::getCurrentItemPointer( ) const
{
    if ( !sellist )
	return 0;
    
    return sellist->getCurrentItemPointer( );
}



NCursesEvent NCPopupTable::wHandleHotkey( wint_t key )
{
  if ( key >= 0 && sellist->setItemByKey( key ) )
    return wHandleInput( KEY_RETURN );

  return NCursesEvent::none;
}



bool NCPopupTable::postAgain()
{
    if ( sellist ) {
	postevent.detail = (postevent == NCursesEvent::button) ?
	    sellist->getCurrentItem() : NCursesEvent::NODETAIL;
    }
    return false;
}

void NCPopupTable::stripHotkeys()
{
    if(sellist) {
	sellist->stripHotkeys();
    }
}
