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

   File:       NCSplit.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCSplit.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSplit::NCSplit
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCSplit::NCSplit( NCWidget * parent, YWidgetOpt & opt,
		  YUIDimension dimension )
    : YSplit( opt, dimension )
    , NCWidget( parent )
{
  WIDDBG << endl;
  wstate = NC::WSdumb;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSplit::~NCSplit
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCSplit::~NCSplit()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSplit::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCSplit::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YSplit::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSplit::moveChild
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCSplit::moveChild( YWidget * child, long newx, long newy )
{
  NCWidget * cw = dynamic_cast<NCWidget*>(child);

  if ( ! ( cw && IsParentOf( *cw ) ) ) {
    NCINT << DLOC << cw << " is not my child" << endl;
    return;
  }

  wMoveChildTo( *cw, wpos( newy, newx ) );
}

