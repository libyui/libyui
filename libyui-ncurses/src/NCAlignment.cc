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

   File:       NCAlignment.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCAlignment.h"


NCAlignment::NCAlignment( YWidget * parent,
			  YAlignmentType halign,
			  YAlignmentType valign )
    : YAlignment( parent, halign, valign )
    , NCWidget( parent )
{
    // yuiDebug() << std::endl;
    wstate = NC::WSdumb;
}


NCAlignment::~NCAlignment()
{
    // yuiDebug() << std::endl;
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
	yuiError() << DLOC << cw << " is not my child" << std::endl;
	return;
    }

    wMoveChildTo( *cw, wpos( newy, newx ) );
}
