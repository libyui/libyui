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

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCReplacePoint.h"



NCReplacePoint::NCReplacePoint( YWidget * parent )
    : YReplacePoint( parent )
    , NCWidget( parent )
{
  yuiDebug() << endl;
  wstate = NC::WSdumb;
}



NCReplacePoint::~NCReplacePoint()
{
  yuiDebug() << endl;
}



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
