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
NCEmpty::NCEmpty( YWidget * parent )
    : YEmpty( parent )
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
void NCEmpty::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}

void NCEmpty::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YEmpty::setEnabled( do_bv );
}
