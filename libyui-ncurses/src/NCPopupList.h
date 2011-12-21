/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/

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
