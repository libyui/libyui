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

   File:       NCPopupList.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupList_h
#define NCPopupList_h

#include <iosfwd>

#include <list>
using namespace std;

#include "NCPopupTable.h"

class YCPString;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupList
//
//	DESCRIPTION :
//
class NCPopupList : public NCPopupTable {

  NCPopupList & operator=( const NCPopupList & );
  NCPopupList            ( const NCPopupList & );

  protected:

    virtual bool postAgain();

  public:

    NCPopupList( const wpos at,
		 const YCPString & label,
		 const list<YCPString> & deflist,
		 int index = 0 );
    virtual ~NCPopupList();

};

///////////////////////////////////////////////////////////////////


#endif // NCPopupList_h
