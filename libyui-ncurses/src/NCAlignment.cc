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
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCAlignment.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAlignment::NCAlignment
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCAlignment::NCAlignment( NCWidget * parent, YWidgetOpt & opt,
			  YAlignmentType halign,
			  YAlignmentType valign )
    : YAlignment( opt, halign, valign )
    , NCWidget( parent )
{
  WIDDBG << endl;
  wstate = NC::WSdumb;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAlignment::~NCAlignment
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCAlignment::~NCAlignment()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAlignment::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCAlignment::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YAlignment::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCAlignment::moveChild
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCAlignment::moveChild( YWidget * child, long newx, long newy )
{
  NCWidget * cw = dynamic_cast<NCWidget*>(child);

  if ( ! ( cw && IsParentOf( *cw ) ) ) {
    NCINT << DLOC << cw << " is not my child" << endl;
    return;
  }

  wMoveChildTo( *cw, wpos( newy, newx ) );
}

