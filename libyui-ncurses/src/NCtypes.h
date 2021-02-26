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

   File:       NCtypes.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCtypes_h
#define NCtypes_h

#include <iosfwd>

namespace NC
{
    enum ADJUST
    {
	CENTER = 0x00,
	TOP    = 0x01,
	BOTTOM = 0x02,
	LEFT   = 0x10,
	RIGHT  = 0x20,
	TOPLEFT = TOP | LEFT,
	TOPRIGHT = TOP | RIGHT,
	BOTTOMLEFT = BOTTOM | LEFT,
	BOTTOMRIGHT = BOTTOM | RIGHT,
    };

    enum WState
    {
	WSdumb,
	WSnormal,
	WSactive,
	WSdisabeled
    };

};

extern std::ostream & operator<<( std::ostream & str, NC::ADJUST obj );
extern std::ostream & operator<<( std::ostream & str, NC::WState obj );


#endif // NCtypes_h
