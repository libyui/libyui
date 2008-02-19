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

   File:       NCLayoutBox.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCLayoutBox.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLayoutBox::NCLayoutBox
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCLayoutBox::NCLayoutBox( YWidget * parent,
			  YUIDimension dimension )
    : YLayoutBox( parent, dimension )
    , NCWidget( parent )
{
  yuiDebug() << endl;
  wstate = NC::WSdumb;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLayoutBox::~NCLayoutBox
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCLayoutBox::~NCLayoutBox()
{
  yuiDebug() << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLayoutBox::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLayoutBox::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YLayoutBox::setSize( newwidth, newheight );
}

void NCLayoutBox::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YLayoutBox::setEnabled( do_bv );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCLayoutBox::moveChild
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCLayoutBox::moveChild( YWidget * child, int newx, int newy )
{
  NCWidget * cw = dynamic_cast<NCWidget*>(child);

  if ( ! ( cw && IsParentOf( *cw ) ) ) {
    yuiError() << DLOC << cw << " is not my child" << endl;
    return;
  }

  wMoveChildTo( *cw, wpos( newy, newx ) );
}

