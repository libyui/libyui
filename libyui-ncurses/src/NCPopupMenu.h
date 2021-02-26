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

   File:       NCPopupMenu.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCPopupMenu_h
#define NCPopupMenu_h

#include <iosfwd>
#include <list>

#include "NCPopupTable.h"
#include "NCMenuButton.h"

using std::list;


class NCPopupMenu : public NCPopupTable
{
private:

    NCPopupMenu & operator=( const NCPopupMenu & );
    NCPopupMenu( const NCPopupMenu & );

    std::map<YTableItem *, YMenuItem *> itemsMap;

protected:

    virtual NCursesEvent wHandleInput( wint_t ch );
    virtual bool postAgain();

public:

    NCPopupMenu( const wpos at,
		 YItemIterator begin,
		 YItemIterator end );

    virtual ~NCPopupMenu();

};



#endif // NCPopupMenu_h
