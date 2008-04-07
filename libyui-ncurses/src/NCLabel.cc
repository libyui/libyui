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

   File:       NCLabel.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCLabel.h"



NCLabel::NCLabel( YWidget * parent, const string & nlabel,
		  bool isHeading, bool isOutputField )
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



long NCLabel::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
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
  yuiDebug() << "LABEL: " << NCstring(nlabel) << " Longest line: " << label.width()<< endl;
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

