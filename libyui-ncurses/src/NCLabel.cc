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

#define AUTO_WRAP_WIDTH         10
#define AUTO_WRAP_HEIGHT        1

using std::string;


NCLabel::NCLabel( YWidget *      parent,
		  const string & nlabel,
		  bool           isHeading,
		  bool           isOutputField )
    : YLabel( parent, nlabel, isHeading, isOutputField )
    , NCWidget( parent )
    , heading( isHeading )
{
    // yuiDebug() << std::endl;
    setText( nlabel );
    hotlabel = &label;
    wstate = NC::WSdumb;
}


NCLabel::~NCLabel()
{
    // yuiDebug() << std::endl;
}


int NCLabel::preferredWidth()
{
    int width;

    if ( autoWrap() )
    {
        if ( layoutPass() == 2 )
        {
            // Use the width passed down to us from the parent layout manager
            // in the last setSize() call. This is the definitive width that
            // will be used after taking all other children of the layout into
            // consideration, also including making widgets smaller due to size
            // restrictions, or redistributing excess space.
            //
            // Since this widget can auto-wrap its contents, we accept
            // basically any width; we just need to adapt the preferred height
            // accordingly.
            width = wrapper.lineWidth();
        }
        else
        {
            // Use a preliminary width. Typically, this widget should be
            // wrapped into a MinSize or MinWidth which hopefully gives us a
            // much more useful width than this.
            //
            // We would also just use 0 here, but that would make debugging
            // really hard since the widget might completly disappear.
            //
            // The real width that will be used will be set in the setSize()
            // call following the recursive preferredWidth() /
            // preferredHeight() calls in the widget tree.
            width = AUTO_WRAP_WIDTH;
        }
    }
    else  // ! autoWrap()
    {
        width = wGetDefsze().W;
    }

    return width;
}


int NCLabel::preferredHeight()
{
    int height;

    if ( autoWrap() )
    {
        if ( layoutPass() == 2 )
        {
            // This is where the magic happens:
            //
            // setSize() in the first layout pass gave us the real width which
            // we stored in as the wrapper's lineWidth. We can now let the
            // wrapper wrap the text into that width and calculate the height
            // (the number of lines of the wrapped text) that is really needed
            // based on that width.
            height = wrapper.lines();
            label  = NCstring( wrapper.wrappedText() );
        }
        else
        {
            height = AUTO_WRAP_HEIGHT;
        }
    }
    else  // ! autoWrap()
    {
        height = wGetDefsze().H;
    }

    return height;
}


void NCLabel::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YLabel::setEnabled( do_bv );
}


void NCLabel::setSize( int newWidth, int newHeight )
{
    if ( autoWrap() && layoutPass() == 1 )
        wrapper.setLineWidth( newWidth );

    wRelocate( wpos( 0 ), wsze( newHeight, newWidth ) );
}


void NCLabel::setText( const string & newLabel )
{
    label = NCstring( newLabel );
    defsze = label.size();
    YLabel::setText( newLabel );
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

    if ( autoWrap() )
        label = NCstring( wrapper.wrappedText() );

    label.drawAt( *win, bg, bg );
}


void NCLabel::setAutoWrap( bool autoWrap )
{
    YLabel::setAutoWrap( autoWrap );

    if ( autoWrap )
    {
        wrapper.setText( NCstring( text() ).str() );
        // Delay setting 'label' until the line width for wrapping is set
    }
    else
    {
        // This will probably never happen since the default for autoWrap is
        // 'false' and nobody will ever set an auto-wrapping label back to
        // non-autowrapping programatically.
        //
        // But anyway, just in case: Let's revert the values to the initial
        // defaults.

        label = NCstring( text() );
        defsze = label.size();
        wrapper.clear();
    }
}

