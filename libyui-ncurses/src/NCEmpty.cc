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

   File:       NCEmpty.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCEmpty.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCEmpty::NCEmpty
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCEmpty::NCEmpty( NCWidget * parent, const YWidgetOpt & opt )
    : YEmpty( opt )
    , NCWidget( parent )
{
  WIDDBG << endl;
  wstate = NC::WSdumb;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCEmpty::~NCEmpty
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCEmpty::~NCEmpty()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCEmpty::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCEmpty::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YEmpty::setSize( newwidth, newheight );
}
