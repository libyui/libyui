/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPopupList.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCPopupList_h
#define NCPopupList_h

#include <iosfwd>
#include <list>

#include "NCPopupTable.h"

using std::list;


class NCPopupList : public NCPopupTable
{

    NCPopupList & operator=( const NCPopupList & );
    NCPopupList( const NCPopupList & );

protected:

    virtual bool postAgain();

public:

    NCPopupList( const wpos at,
		 const string & label,
		 const list<string> & deflist,
		 int index = 0 );
    
    virtual ~NCPopupList();

    void createEntries( const list<string> & deflist, int index );
};



#endif // NCPopupList_h
