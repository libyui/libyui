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
