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

   File:       NCtypes.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCtypes.h"


std::ostream & operator<<( std::ostream & str, NC::ADJUST obj )
{
#define PRT(a) case NC::a: return str << #a

    switch ( obj )
    {
	PRT( CENTER );
	PRT( TOP );
	PRT( BOTTOM );
	PRT( LEFT );
	PRT( RIGHT );
	PRT( TOPLEFT );
	PRT( TOPRIGHT );
	PRT( BOTTOMLEFT );
	PRT( BOTTOMRIGHT );
    }

    return str << "NC::ADJUST";

#undef PRT
}


std::ostream & operator<<( std::ostream & str, NC::WState obj )
{
#define PRT(a) case NC::a: return str << #a

    switch ( obj )
    {
	PRT( WSdumb );
	PRT( WSnormal );
	PRT( WSactive );
	PRT( WSdisabeled );
    }

    return str << "NC::WState";

#undef PRT
}

