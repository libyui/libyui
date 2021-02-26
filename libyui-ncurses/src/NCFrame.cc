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

   File:       NCFrame.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCFrame.h"


NCFrame::NCFrame( YWidget * parent, const std::string & nlabel )
	: YFrame( parent, nlabel )
	, NCWidget( parent )
{
    yuiDebug() << std::endl;
    wstate = NC::WSdumb;
    framedim.Pos = wpos( 1 );
    framedim.Sze = wsze( 2 );
    setLabel( YFrame::label() );
    hotlabel = &label;
}


NCFrame::~NCFrame()
{
    yuiDebug() << std::endl;
}


int NCFrame::preferredWidth()
{
    defsze.W = hasChildren() ? firstChild()->preferredWidth() : 0;

    if ( label.width() > ( unsigned )defsze.W )
	defsze.W = label.width();

    defsze.W += framedim.Sze.W;

    return defsze.W;
}


int NCFrame::preferredHeight()
{
    defsze.H  = hasChildren() ? firstChild()->preferredHeight() : 0;
    defsze.H += framedim.Sze.H;

    return defsze.H;
}


void NCFrame::setSize( int newwidth, int newheight )
{
    wsze csze( newheight, newwidth );
    wRelocate( wpos( 0 ), csze );
    csze = wsze::max( 0, csze - framedim.Sze );

    if ( hasChildren() )
	firstChild()->setSize( csze.W, csze.H );
}


void NCFrame::setLabel( const std::string & nlabel )
{
    YFrame::setLabel( nlabel );
    label = NCstring( YFrame::label() );
    label.stripHotkey();
    Redraw();
}


void NCFrame::setEnabled( bool do_bv )
{
    // Use setEnabled() from the parent, it should work out (#256707) :-)
    NCWidget::setEnabled( do_bv );
    YFrame::setEnabled( do_bv );
}


bool NCFrame::gotBuddy()
{
    if ( !label.hasHotkey() )
	return false;

    for ( tnode<NCWidget*> * c = this->Next();
	  c && c->IsDescendantOf( this );
	  c = c->Next() )
    {
	if ( c->Value()->GetState() != NC::WSdumb )
	    return true;
    }

    return false;
}


void NCFrame::wRedraw()
{
    if ( !win )
	return;

    chtype bg = wStyle().dumb.text;
    win->bkgd( bg );
    win->box();

    if ( gotBuddy() )
	label.drawAt( *win, widgetStyle(), wpos( 0, 1 ),
		      wsze( 1, win->width() - 2 ), NC::TOPLEFT, false );
    else
	label.drawAt( *win, bg, bg, wpos( 0, 1 ),
		      wsze( 1, win->width() - 2 ), NC::TOPLEFT, false );
}

