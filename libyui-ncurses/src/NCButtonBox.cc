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

   File:       NCButtonBox.cc

   Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCButtonBox.h"


NCButtonBox::NCButtonBox( YWidget * parent )
	: YButtonBox( parent )
	, NCWidget( parent )
{
    yuiDebug() << endl;
    wstate = NC::WSdumb;
}


NCButtonBox::~NCButtonBox()
{
    yuiDebug() << endl;
}


void NCButtonBox::setSize( int newWidth, int newHeight )
{
    wRelocate( wpos( 0 ), wsze( newHeight, newWidth ) );
    YButtonBox::setSize( newWidth, newHeight );
}


void NCButtonBox::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YButtonBox::setEnabled( do_bv );
}


void NCButtonBox::moveChild( YWidget * child, int newX, int newY )
{
    NCWidget * cw = dynamic_cast<NCWidget*>( child );

    if ( !( cw && IsParentOf( *cw ) ) )
    {
	yuiError() << DLOC << cw << " is not my child" << endl;
	return;
    }

    wMoveChildTo( *cw, wpos( newY, newX ) );
}
