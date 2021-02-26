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

   File:       NCtypes.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCtypes.h"

using std::ostream;


ostream & operator<<( ostream & str, NC::ADJUST obj )
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


ostream & operator<<( ostream & str, NC::WState obj )
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

