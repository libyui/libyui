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

   File:       NCPopupList.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCPopupList_h
#define NCPopupList_h

#include <iosfwd>
#include <list>

#include "NCPopupTable.h"


class NCPopupList : public NCPopupTable
{

    NCPopupList & operator=( const NCPopupList & );
    NCPopupList( const NCPopupList & );

protected:

    virtual bool postAgain();

public:

    NCPopupList( const wpos at,
		 const std::string & label,
		 const std::list<std::string> & deflist,
		 int index = 0 );

    virtual ~NCPopupList();

    void createEntries( const std::list<std::string> & deflist, int index );
};



#endif // NCPopupList_h
