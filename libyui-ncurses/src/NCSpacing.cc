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

   File:       NCSpacing.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCSpacing.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSpacing::NCSpacing
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCSpacing::NCSpacing( NCWidget * parent, YWidgetOpt & opt,
		      float size, bool horizontal, bool vertical )
    : YSpacing( opt, size, horizontal, vertical )
    , NCWidget( parent )
{
  WIDDBG << endl;
  if ( horizontal && vertical ) {
    l = "NC(B)Spacing";
  }
  else if ( horizontal ) {
    l = "NC(H)Spacing";
  }
  else if ( vertical ) {
    l = "NC(V)Spacing";
  }
  else {
    l = "NC(N)Spacing";
  }
  wstate = NC::WSdumb;
  skipNoDimWin = true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSpacing::~NCSpacing
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCSpacing::~NCSpacing()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSpacing::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCSpacing::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YSpacing::setSize( newwidth, newheight );
}

