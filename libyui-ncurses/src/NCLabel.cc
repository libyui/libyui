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

   File:       NCLabel.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCLabel.h"


NCLabel::NCLabel( YWidget *      parent,
		  const std::string & nlabel,
		  bool           isHeading,
		  bool           isOutputField )
    : YLabel( parent, nlabel, isHeading, isOutputField )
    , NCWidget( parent )
    , heading( isHeading )
{
    yuiDebug() << std::endl;
    setText( nlabel );
    hotlabel = &label;
    wstate = NC::WSdumb;
}


NCLabel::~NCLabel()
{
    yuiDebug() << std::endl;
}


int NCLabel::preferredWidth()
{
    return wGetDefsze().W;
}


int NCLabel::preferredHeight()
{
    return wGetDefsze().H;
}


void NCLabel::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YLabel::setEnabled( do_bv );
}


void NCLabel::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


void NCLabel::setText( const std::string & nlabel )
{
    label  = NCstring( nlabel );
    yuiDebug() << "LABEL: " << NCstring( nlabel ) << " Longest line: " << label.width() << std::endl;
    defsze = label.size();
    YLabel::setText( nlabel );
    Redraw();
}


void NCLabel::wRedraw()
{
    if ( !win )
	return;

    chtype bg = heading ? wStyle().dumb.title
		: wStyle().dumb.text;

    win->bkgd( bg );
    win->clear();
    label.drawAt( *win, bg, bg );
}
