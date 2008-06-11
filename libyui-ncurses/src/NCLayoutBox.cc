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

   File:       NCLayoutBox.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCLayoutBox.h"


NCLayoutBox::NCLayoutBox( YWidget * parent,
			  YUIDimension dimension )
	: YLayoutBox( parent, dimension )
	, NCWidget( parent )
{
    yuiDebug() << endl;
    wstate = NC::WSdumb;
}


NCLayoutBox::~NCLayoutBox()
{
    yuiDebug() << endl;
}


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


void NCLayoutBox::moveChild( YWidget * child, int newx, int newy )
{
    NCWidget * cw = dynamic_cast<NCWidget*>( child );

    if ( !( cw && IsParentOf( *cw ) ) )
    {
	yuiError() << DLOC << cw << " is not my child" << endl;
	return;
    }

    wMoveChildTo( *cw, wpos( newy, newx ) );
}
