/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCEmpty.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCEmpty.h"


NCEmpty::NCEmpty( YWidget * parent )
	: YEmpty( parent )
	, NCWidget( parent )
{
    yuiDebug() << endl;
    wstate = NC::WSdumb;
}


NCEmpty::~NCEmpty()
{
    yuiDebug() << endl;
}


void NCEmpty::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


void NCEmpty::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YEmpty::setEnabled( do_bv );
}
