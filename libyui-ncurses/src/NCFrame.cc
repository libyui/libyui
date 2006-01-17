/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCFrame.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCFrame.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFrame::NCFrame
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCFrame::NCFrame( NCWidget * parent, const YWidgetOpt & opt,
		  const YCPString & nlabel )
    : YFrame( opt, nlabel )
    , NCWidget( parent )
{
  WIDDBG << endl;
  wstate = NC::WSdumb;
  framedim.Pos = wpos( 1 );
  framedim.Sze = wsze( 2 );
  setLabel( getLabel() );
  hotlabel = &label;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFrame::~NCFrame
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCFrame::~NCFrame()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFrame::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCFrame::nicesize( YUIDimension dim )
{
  defsze = wsze( YContainerWidget::child(0)->nicesize( YD_VERT ),
		 YContainerWidget::child(0)->nicesize( YD_HORIZ ) );
  if ( label.width() > (unsigned)defsze.W )
    defsze.W = label.width();
  defsze += framedim.Sze;

  return dim == YD_HORIZ ? defsze.W : defsze.H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFrame::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCFrame::setSize( long newwidth, long newheight )
{
  wsze csze( newheight, newwidth );
  wRelocate( wpos( 0 ), csze );
  csze = wsze::max( 0, csze - framedim.Sze );
  YContainerWidget::child(0)->setSize( csze.W, csze.H );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFrame::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCFrame::setLabel( const YCPString & nlabel )
{
  YFrame::setLabel( nlabel );
  label = NCstring( getLabel() );
  label.stripHotkey();
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFrame::setEnabling
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCFrame::setEnabling( bool do_bv )
{
  enabled = do_bv; // in YWidget
  for ( tnode<NCWidget*> * c = this->Next();
	c && c->IsDescendantOf( this );
	c = c->Next() ) {
    if ( c->Value()->GetState() != NC::WSdumb )
      c->Value()->setEnabling( enabled );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFrame::gotBuddy
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCFrame::gotBuddy()
{
  if ( !label.hasHotkey() )
    return false;

  for ( tnode<NCWidget*> * c = this->Next();
	c && c->IsDescendantOf( this );
	c = c->Next() ) {
    if ( c->Value()->GetState() != NC::WSdumb )
      return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFrame::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCFrame::wRedraw()
{
  if ( !win )
    return;

  chtype bg = wStyle().dumb.text;
  win->bkgd( bg );
  win->box();
  if ( gotBuddy() )
    label.drawAt( *win, widgetStyle(), wpos(0,1),
		  wsze(1,win->width()-2), NC::TOPLEFT, false );
  else
    label.drawAt( *win, bg, bg, wpos(0,1),
		  wsze(1,win->width()-2), NC::TOPLEFT, false );
}

