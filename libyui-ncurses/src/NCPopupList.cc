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
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupList.h"

#include <ycp/YCPString.h>

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupList::NCPopupList
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupList::NCPopupList( const wpos at,
			  const YCPString & label,
			  const list<YCPString> & deflist,
			  int index )
    : NCPopupTable( at )
{
  vector<string> row( 1 );
  createList( row );
  for ( list<YCPString>::const_iterator entry = deflist.begin();
	entry != deflist.end(); ++entry ) {
    row[0] = (*entry)->value();
    addItem( YCPNull(), row );
  }
  setCurrentItem( index );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupList::~NCPopupList
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupList::~NCPopupList()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupList::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupList::postAgain()
{
  return NCPopupTable::postAgain();
}
