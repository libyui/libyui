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

   File:       NCPopupMenu.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupMenu_h
#define NCPopupMenu_h

#include <iosfwd>

#include <list>
using namespace std;

#include "NCPopupTable.h"

class YMenuItem;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupMenu
//
//	DESCRIPTION :
//
class NCPopupMenu : public NCPopupTable {

  NCPopupMenu & operator=( const NCPopupMenu & );
  NCPopupMenu            ( const NCPopupMenu & );

  private:

    YMenuItem & menu;

  protected:

    virtual NCursesEvent wHandleInput( wint_t ch );
    virtual bool postAgain();

  public:

    NCPopupMenu( const wpos at, YMenuItem & menuitem );
    virtual ~NCPopupMenu();
//    virtual void itemAdded( const YCPString & string,
//			    int index,
//			    bool selected );

};

///////////////////////////////////////////////////////////////////


#endif // NCPopupMenu_h
