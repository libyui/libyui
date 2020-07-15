/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

   File:       NCPopupTable.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCPopupTable_h
#define NCPopupTable_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include <yui/YTableItem.h>

class NCTable;


class NCPopupTable : public NCPopup
{
private:

    NCPopupTable & operator=( const NCPopupTable & );
    NCPopupTable( const NCPopupTable & );

    NCTable * sellist;

protected:

    void createList( std::vector<std::string> & row );

    void addItem( YItem *yitem );

    void setCurrentItem( int index );
    int  getCurrentItem() const;
    YItem * getCurrentItemPointer() const;

    virtual NCursesEvent wHandleHotkey( wint_t ch );

protected:

    NCPopupTable( const wpos & at );
    virtual ~NCPopupTable();

    virtual bool postAgain();

public:
    void stripHotkeys();
};



#endif // NCPopupTable_h
