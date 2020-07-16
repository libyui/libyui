/*
  Copyright (C) 2020 SUSE LLC
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

   File:       NCPulldownMenu.cc

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define	 YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPulldownMenu.h"

using std::string;
using std::endl;


NCPulldownMenu::NCPulldownMenu( NCWidget * parent, const string & label )
    : NCWidget( parent )
{
    // yuiDebug() << label << endl;

    setLabel( label );
    hotlabel = &_label; // hotlabel is inherited from NCWidget
}


NCPulldownMenu::~NCPulldownMenu()
{
    // yuiDebug() << endl;
}


int
NCPulldownMenu::preferredWidth()
{
    return wGetDefsze().W;
}


int
NCPulldownMenu::preferredHeight()
{
    return wGetDefsze().H;
}


void
NCPulldownMenu::resizeToContent()
{
    wRelocate( wpos( 0, 0 ), wGetDefsze() );
}


void
NCPulldownMenu::setLabel( const string & label )
{
    _label = NCstring( label );
    _label.stripHotkey();
    defsze = wsze( _label.height(), _label.width() + 2 );
    Redraw();
}


void
NCPulldownMenu::wRedraw()
{
    if ( !win )
	return;

    const NCstyle::StWidget & style( widgetStyle() );

    win->bkgd( style.plain );
    win->clear();

#if 0
    win->printw( 0, 0, "[" );
    win->printw( 0, win->maxx(), "]" );
#else
    win->printw( 0, 0, " " );
    win->printw( 0, win->maxx(), " " );
#endif

    _label.drawAt( *win,
                   style,
                   wpos( 0, 1 ),
                   wsze( -1, win->width() - 2 ),
                   NC::LEFT );
}


void
NCPulldownMenu::setEnabled( bool enabled )
{
    NCWidget::setEnabled( enabled );
}
