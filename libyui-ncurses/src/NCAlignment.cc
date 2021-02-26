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

   File:       NCAlignment.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCAlignment.h"



NCAlignment::NCAlignment( YWidget * parent,
			  YAlignmentType halign,
			  YAlignmentType valign )
    : YAlignment( parent, halign, valign )
    , NCWidget( parent )
{
  yuiDebug() << endl;
  wstate = NC::WSdumb;
}



NCAlignment::~NCAlignment()
{
  yuiDebug() << endl;
}



void NCAlignment::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YAlignment::setSize( newwidth, newheight);
}



void NCAlignment::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YAlignment::setEnabled( do_bv );
}



void NCAlignment::moveChild( YWidget * child, int newx, int newy )
{
  NCWidget * cw = dynamic_cast<NCWidget*>(child);

  if ( ! ( cw && IsParentOf( *cw ) ) ) {
    yuiError() << DLOC << cw << " is not my child" << endl;
    return;
  }

  wMoveChildTo( *cw, wpos( newy, newx ) );
}

