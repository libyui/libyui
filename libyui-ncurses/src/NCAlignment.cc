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

   File:       NCAlignment.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCAlignment.h"


NCAlignment::NCAlignment( YWidget * parent,
			  YAlignmentType halign,
			  YAlignmentType valign )
    : YAlignment( parent, halign, valign )
    , NCWidget( parent )
{
    yuiDebug() << endl;
    wstate = NC::WSdumb;
}


NCAlignment::~NCAlignment()
{
    yuiDebug() << endl;
}


void NCAlignment::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
    YAlignment::setSize( newwidth, newheight );
}


void NCAlignment::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YAlignment::setEnabled( do_bv );
}


void NCAlignment::moveChild( YWidget * child, int newx, int newy )
{
    NCWidget * cw = dynamic_cast<NCWidget*>( child );

    if ( !( cw && IsParentOf( *cw ) ) )
    {
	yuiError() << DLOC << cw << " is not my child" << endl;
	return;
    }

    wMoveChildTo( *cw, wpos( newy, newx ) );
}
