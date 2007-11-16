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

   File:       NCReplacePoint.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCReplacePoint.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCReplacePoint::NCReplacePoint
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCReplacePoint::NCReplacePoint( YWidget * parent )
    : YReplacePoint( parent )
    , NCWidget( parent )
{
  WIDDBG << endl;
  wstate = NC::WSdumb;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCReplacePoint::~NCReplacePoint
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCReplacePoint::~NCReplacePoint()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCReplacePoint::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCReplacePoint::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YReplacePoint::setSize( newwidth, newheight);
}

void NCReplacePoint::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YReplacePoint::setEnabled( do_bv );
}
