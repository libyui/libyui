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
NCFrame::NCFrame( YWidget * parent, const string & nlabel )
    : YFrame( parent, nlabel )
    , NCWidget( parent )
{
  WIDDBG << endl;
  wstate = NC::WSdumb;
  framedim.Pos = wpos( 1 );
  framedim.Sze = wsze( 2 );
  //setLabel( getLabel() );
  setLabel( YFrame::label() );
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
  defsze = wsze( firstChild()->preferredWidth(),
		 firstChild()->preferredHeight() );
    
  if ( label.width() > (unsigned)defsze.W )
    defsze.W = label.width();
  defsze += framedim.Sze;

  return dim == YD_HORIZ ? defsze.W : defsze.H;
}

int NCFrame::preferredWidth()
{
    defsze.W = firstChild()->preferredWidth();
    
    if ( label.width() > (unsigned)defsze.W )
	defsze.W = label.width();
    defsze.W += framedim.Sze.W;
  
    return defsze.W;
}

int NCFrame::preferredHeight()
{
    defsze.H = firstChild()->preferredHeight() + framedim.Sze.H;
    
    return defsze.H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFrame::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCFrame::setSize( int newwidth, int newheight )
{
  wsze csze( newheight, newwidth );
  wRelocate( wpos( 0 ), csze );
  csze = wsze::max( 0, csze - framedim.Sze );
  firstChild()->setSize( csze.W, csze.H );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFrame::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCFrame::setLabel( const string & nlabel )
{
  YFrame::setLabel( nlabel );
  //label = NCstring( getLabel() );
  label = NCstring( YFrame::label() );
  label.stripHotkey();
  Redraw();
}

void NCFrame::setEnabled( bool do_bv )
{
    //Use setEnabled() from the parent, it should work out (#256707) :-)
    NCWidget::setEnabled( do_bv );
    YFrame::setEnabled( do_bv );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCFrame::setEnabling
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
//void NCFrame::setEnabling( bool do_bv )
//{
//  enabled = do_bv; // in YWidget
//  for ( tnode<NCWidget*> * c = this->Next();
//	c && c->IsDescendantOf( this );
//	c = c->Next() ) {
//    if ( c->Value()->GetState() != NC::WSdumb )
//      c->Value()->setEnabling( enabled );
//  }
//}

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

