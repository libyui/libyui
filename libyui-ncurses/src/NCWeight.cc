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

   File:       NCWeight.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCWeight.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWeight::NCWeight
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCWeight::NCWeight( NCWidget * parent, YUIDimension dim, long weight )
    : YWeight( dim, weight )
    , NCWidget( parent )
{
  WIDDBG << endl;
  wstate = NC::WSdumb;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWeight::~NCWeight
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCWeight::~NCWeight()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCWeight::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCWeight::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YWeight::setSize( newwidth, newheight );
}
