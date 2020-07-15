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

   File:       NCLayoutBox.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCLayoutBox.h"


NCLayoutBox::NCLayoutBox( YWidget * parent,
			  YUIDimension dimension )
	: YLayoutBox( parent, dimension )
	, NCWidget( parent )
{
    // yuiDebug() << std::endl;
    wstate = NC::WSdumb;
}


NCLayoutBox::~NCLayoutBox()
{
    // yuiDebug() << std::endl;
}


void NCLayoutBox::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
    YLayoutBox::setSize( newwidth, newheight );
}


void NCLayoutBox::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YLayoutBox::setEnabled( do_bv );
}


void NCLayoutBox::moveChild( YWidget * child, int newx, int newy )
{
    NCWidget * cw = dynamic_cast<NCWidget*>( child );

    if ( !( cw && IsParentOf( *cw ) ) )
    {
	yuiError() << DLOC << cw << " is not my child" << std::endl;
	return;
    }

    wMoveChildTo( *cw, wpos( newy, newx ) );
}
