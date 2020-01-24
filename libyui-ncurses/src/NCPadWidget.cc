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

   File:       NCPadWidget.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCPadWidget.h"


class NCScrollbar
{
public:

    enum orientation { HORZ, VERT };

private:

    chtype ch_forward;
    chtype ch_backward;
    chtype ch_barbeg;
    chtype ch_barend;
    chtype ch_barone;

    const NCWidget & parw;
    NCursesWindow *  win;
    orientation      type;
    unsigned	     len;

    unsigned total;
    unsigned visible;
    unsigned at;

private:

    void adjust()
    {
	if ( visible > total )
	{
	    yuiDebug() << "adjust visible " << visible << " > total " << total << std::endl;
	    visible = total;
	}

	if ( at + visible > total )
	{
	    yuiDebug() << "adjust at " << at << " + visible " << visible << " > total " << total << std::endl;
	    at = total - visible;
	}
    }

    void draw_line( unsigned p, unsigned l, chtype ch = 0 )
    {
	if ( !l )
	    return;

	if ( type == HORZ )
	    win->hline( 0, p, l, ch );
	else
	    win->vline( p, 0, l, ch );
    }

    void draw_bar( unsigned p, unsigned l )
    {
	if ( !l )
	    return;

	if ( l == 1 )
	{
	    if ( type == HORZ )
		win->addch( 0, p, ch_barone );
	    else
		win->addch( p, 0, ch_barone );

	    return;
	}

	unsigned e = p + l - 1; // barend

	l -= 2; // inner bar

	if ( type == HORZ )
	{
	    win->addch( 0, p, ch_barbeg );
	    win->addch( 0, e, ch_barend );

	    if ( l )
		win->hline( 0, p + 1, l );
	}
	else
	{
	    win->addch( p, 0, ch_barbeg );
	    win->addch( e, 0, ch_barend );

	    if ( l )
		win->vline( p + 1, 0, l );
	}
    }

    void draw()
    {
	if ( !win )
	    return;

	bool topvis = ( at == 0 );

	bool botvis = ( at + visible == total );

	const NCstyle::StWidget & style( parw.frameStyle() );

	win->bkgdset( style.plain );

	draw_line( 0, len );

	if ( topvis && botvis )
	{
	    // no scroll at all
	    return;
	}

	switch ( len )
	{
	    case 1:
		win->bkgdset( style.scrl );

		if ( topvis == botvis )
		{
		    // both == false; otherwise handled above
		    win->addch( 0, 0, ch_barone );
		}
		else
		{
		    win->addch( 0, 0, ( topvis ? ch_forward : ch_backward ) );
		}

		break;

	    case 2:
		win->bkgdset( style.scrl );

		if ( !topvis )
		    win->addch( 0, 0, ch_backward );

		if ( !botvis )
		    win->addch( win->maxy(), win->maxx(), ch_forward );

		break;

	    default:

	    case 3:
		{
		    unsigned blen;
		    unsigned bat;

		    if ( visible + 1 == total )
		    {
			// just one invisible line
			blen = len - 1;
			bat  = at ? 1 : 0;
		    }
		    else
		    {
			blen = ( visible * ( len - 2 ) / total ) + 1;

			if ( topvis == botvis )
			{
			    // both == false; otherwise handled above
			    bat = ( at * ( len - 2 ) / total ) + 1;
			}
			else
			{
			    bat = ( topvis ? 0 : len - blen );
			}
		    }

		    win->bkgdset( style.plain );

		    draw_line( 0, len );
		    win->bkgdset( style.scrl );
		    draw_bar( bat, blen );
		}

		break;
	}
    }

public:

    NCScrollbar( const NCWidget & parwid, NCursesWindow & par, wpos p, unsigned l, orientation orient )
	    : ch_forward( orient == HORZ ? ACS_RARROW : ACS_DARROW )
	    , ch_backward( orient == HORZ ? ACS_LARROW : ACS_UARROW )
	    , ch_barbeg( orient == HORZ ? ACS_LTEE : ACS_TTEE )
	    , ch_barend( orient == HORZ ? ACS_RTEE : ACS_BTEE )
	    , ch_barone( orient == HORZ ? ACS_VLINE : ACS_HLINE )
	    , parw( parwid )
	    , win( 0 )
	    , type( orient )
	    , len( l ? l : 1 )
	    , total( 0 )
	    , visible( 0 )
	    , at( 0 )
    {
	try
	{
	    win = new NCursesWindow( par,
				     ( type == HORZ ? 1 : len ), // height
				     ( type == VERT ? 1 : len ), // width
				     p.L, p.C,
				     'r' );
	}
	catch ( NCursesException & err )
	{
	    yuiDebug() << "NCScrollbar: " << err.message
	    << ": at " << p << " len " << len << " in " << par << std::endl;
	    return;
	}
    }

    virtual ~NCScrollbar()
    {
	delete win;
    }

public:

    void set( unsigned tot, unsigned vis, unsigned start )
    {
	total	= tot;
	visible = vis;
	at	= start;
	adjust();
	draw();
    }

};




NCPadWidget::NCPadWidget( NCWidget * myparent )
	: NCWidget( myparent )
	, padwin( 0 )
	, hsb( 0 )
	, vsb( 0 )
	, multidraw( false )
	, pad( 0 )
	, hasHeadline( false )
	, activeLabelOnly( false )
{
    yuiDebug() << std::endl;
    hotlabel = &label;
    defsze = wsze( 3, 10 ) + 2;
}


NCPadWidget::NCPadWidget( YWidget * myparent )
	: NCWidget( myparent )
	, padwin( 0 )
	, hsb( 0 )
	, vsb( 0 )
	, multidraw( false )
	, pad( 0 )
	, hasHeadline( false )
	, activeLabelOnly( false )
{
    yuiDebug() << std::endl;
    hotlabel = &label;
    defsze = wsze( 3, 10 ) + 2;
}


NCPadWidget::~NCPadWidget()
{
    delete pad;

    if ( padwin != win )
	delete padwin;

    yuiDebug() << std::endl;
}


void NCPadWidget::wCreate( const wrect & newrect )
{
    NCWidget::wCreate( newrect );

    if ( win )
    {
	wrect padrect( newrect.inside() );

	if ( hasHeadline )
	{
	    if ( padrect.Sze.H > 1 )
	    {
		padrect.Pos.L += 1;
		padrect.Sze.H -= 1;
	    }
	    else
		hasHeadline = false;
	}

	padwin = new NCursesWindow( *win,

				    padrect.Sze.H, padrect.Sze.W,
				    padrect.Pos.L, padrect.Pos.C,
				    'r' );

        // Scrollbar has to be at least one character large otherwise ncurses window
        // for the scrollbar will fail to create and we end with an exception which
        // crashes whole UI consequently.
        //
        // scrollbar size is lowered by -2 bcs there is an overhead for frames etc.
        if(win->width() > 2)
        {
            hsb = new NCScrollbar( *this, *win, wpos( win->maxy(), 1 ), win->width() - 2, NCScrollbar::HORZ );
        }
        else
        {
            // no space no scrollbar, scrolling still works using arrows
            hsb = 0;
        }

        if(win->height() - 2 > 0)
        {
            // we have enough space for vertical scrollbar
            vsb = new NCScrollbar( *this, *win, wpos( 1, win->maxx() ), win->height() - 2, NCScrollbar::VERT );
        }
        else
        {
            // no space no scrollbar, scrolling still works using arrows
            vsb = 0;
        }
    }
    else
    {
	padwin = 0;
	hsb = 0;
	vsb = 0;
    }
}


void NCPadWidget::wDelete()
{
    if ( pad )
    {
	pad->Destwin( 0 );
    }

    if ( padwin )
    {
	if ( padwin != win )
	{
	    delete padwin;
	}

	padwin = 0;
    }

    delete hsb;

    hsb = 0;
    delete vsb;
    vsb = 0;
    NCWidget::wDelete();
}


void NCPadWidget::InitPad()
{
    if ( pad )
	return;

    pad = CreatePad();
    pad->SendSchrollCB( this );
    AdjustPad( wsze( pad->height(), pad->width() ) );
    DrawPad();
}


void NCPadWidget::AdjustPadSize( wsze & minsze )
{
    minPadSze = minsze;
    minsze    = wsze::max( minsze, defPadSze() );
}


void NCPadWidget::AdjustPad( wsze nsze )
{
    if ( !pad )
	return;

    pad->resize( nsze );
}


void NCPadWidget::DelPad()
{
    delete pad;
    pad = 0;
}


void NCPadWidget::setLabel( const NClabel & nlabel )
{
    label = nlabel;
    label.stripHotkey();
    Redraw();
}


void NCPadWidget::wRedraw()
{
    if ( !win )
	return;

    yuiDebug() << "wRedraw called for: " << win << std::endl;

    const NCstyle::StWidget & style( frameStyle() );

    win->bkgdset( style.plain );

    win->box();

    if ( !activeLabelOnly || GetState() == NC::WSactive )
	label.drawAt( *win, style, wpos( 0, 1 ),
		      wsze( 1, win->width() - 2 ), NC::TOPLEFT, false );

    InitPad();

    if ( !pad->Destwin() )
    {
	AdjustPad( minPadSze );
	pad->Destwin( padwin );
    }
    else
    {
	pad->setDirty();
	pad->setpos();
	pad->SendHead();
    }
}


void NCPadWidget::wRecoded()
{
    if ( pad )
	pad->wRecoded();

    wRedraw();
}


void NCPadWidget::HScroll( unsigned total, unsigned visible, unsigned start )
{
    // horizontal scroll bar might got disabled bcs of lack of space. See
    // NCPadWidget::wCreate
    if(hsb)
    {
        hsb->set( total, visible, start );
    }
}


void NCPadWidget::VScroll( unsigned total, unsigned visible, unsigned start )
{
    // vertical scroll bar might got disabled bcs of lack of space. See
    // NCPadWidget::wCreate
    if(vsb)
    {
        vsb->set( total, visible, start );
    }
}


void NCPadWidget::ScrollHead( NCursesWindow & w, unsigned ccol )
{
    if ( hasHeadline && win )
    {
	w.copywin( *win, 0, ccol, 1, 1, 1, win->width() - 2, false );
    }
}


NCPad * NCPadWidget::CreatePad()
{
    wsze    psze( defPadSze() );
    NCPad * npad = new NCPad( psze.H, psze.W, *this );
    npad->bkgd( wStyle().list.item.plain );

    return npad;
}


void NCPadWidget::DrawPad()
{
    if ( pad && !inMultidraw() )
    {
	pad->update();
	Redraw();
    }
}


bool NCPadWidget::handleInput( wint_t key )
{
    return pad->handleInput( key );
}
