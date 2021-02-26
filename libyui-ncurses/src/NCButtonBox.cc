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

   File:       NCButtonBox.cc

   Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCButtonBox.h"


NCButtonBox::NCButtonBox( YWidget * parent )
	: YButtonBox( parent )
	, NCWidget( parent )
{
    yuiDebug() << std::endl;
    wstate = NC::WSdumb;
}


NCButtonBox::~NCButtonBox()
{
    yuiDebug() << std::endl;
}


void NCButtonBox::setSize( int newWidth, int newHeight )
{
    wRelocate( wpos( 0 ), wsze( newHeight, newWidth ) );
    YButtonBox::setSize( newWidth, newHeight );
}


void NCButtonBox::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YButtonBox::setEnabled( do_bv );
}


void NCButtonBox::moveChild( YWidget * child, int newX, int newY )
{
    NCWidget * cw = dynamic_cast<NCWidget*>( child );

    if ( !( cw && IsParentOf( *cw ) ) )
    {
	yuiError() << DLOC << cw << " is not my child" << std::endl;
	return;
    }

    wMoveChildTo( *cw, wpos( newY, newX ) );
}
