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

   File:       NCLabel.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCLabel.h"


NCLabel::NCLabel( YWidget *      parent,
		  const string & nlabel,
		  bool           isHeading,
		  bool           isOutputField )
    : YLabel( parent, nlabel, isHeading, isOutputField )
    , NCWidget( parent )
    , heading( isHeading )
{
    yuiDebug() << endl;
    setText( nlabel );
    hotlabel = &label;
    wstate = NC::WSdumb;
}


NCLabel::~NCLabel()
{
    yuiDebug() << endl;
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


void NCLabel::setText( const string & nlabel )
{
    label  = NCstring( nlabel );
    yuiDebug() << "LABEL: " << NCstring( nlabel ) << " Longest line: " << label.width() << endl;
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
