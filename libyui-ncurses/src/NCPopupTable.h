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

   File:       NCPopupTable.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCPopupTable_h
#define NCPopupTable_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "YTableItem.h"

class NCTable;

using std::vector;
using std::string;


class NCPopupTable : public NCPopup
{
private:

    NCPopupTable & operator=( const NCPopupTable & );
    NCPopupTable( const NCPopupTable & );

    NCTable * sellist;

protected:

    void createList( vector<string> & row );

    void addItem( YItem *yitem );

    void setCurrentItem( int index );
    int  getCurrentItem() const;
    YItem * getCurrentItemPointer( ) const;

    virtual NCursesEvent wHandleHotkey( wint_t ch );

protected:

    NCPopupTable( const wpos at );
    virtual ~NCPopupTable();

    virtual bool postAgain();

public:
    void stripHotkeys();
};



#endif // NCPopupTable_h
