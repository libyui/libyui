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

   File:       NCPopupList.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCPopupList.h"
#include "YTableItem.h"




NCPopupList::NCPopupList( const wpos at,
			  const string & label,
			  const list<string> & deflist,
			  int index )
    : NCPopupTable( at )
{
    createEntries( deflist, index);
}



NCPopupList::~NCPopupList()
{

}

void NCPopupList::createEntries( const list<string> & deflist, int index )
{
    vector<string> row( 1 );
    createList( row );
    for ( list<string>::const_iterator entry = deflist.begin();
	  entry != deflist.end(); ++entry ) {
        YItem *item = new YTableItem( (*entry ));
	addItem( item );
    }
    setCurrentItem( index );
}



bool NCPopupList::postAgain()
{
  return NCPopupTable::postAgain();
}

