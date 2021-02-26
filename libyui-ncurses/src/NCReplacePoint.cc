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

   File:       NCReplacePoint.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCReplacePoint.h"


NCReplacePoint::NCReplacePoint( YWidget * parent )
	: YReplacePoint( parent )
	, NCWidget( parent )
{
    // yuiDebug() << std::endl;
    wstate = NC::WSdumb;
}


NCReplacePoint::~NCReplacePoint()
{
    // yuiDebug() << std::endl;
}


void NCReplacePoint::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
    YReplacePoint::setSize( newwidth, newheight );
}


void NCReplacePoint::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YReplacePoint::setEnabled( do_bv );
}
