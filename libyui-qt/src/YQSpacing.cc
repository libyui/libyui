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

  File:	      YQSpacing.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include "YUIQt.h"
#include "YQSpacing.h"


YQSpacing::YQSpacing( QWidget * 	parent,
		      YWidgetOpt & 	opt,
		      float 		size,
		      bool 		horizontal,
		      bool 		vertical )
    : QWidget( parent )
    , YSpacing( opt, size, horizontal, vertical )
{
    setWidgetRep( this );
}


long YQSpacing::absoluteSize( YUIDimension dim, float relativeSize )
{
    /*
     * Rather than using actual font metrics, we try to get as close to
     * the character based NCurses UI as possible: Divide the dialog
     * default size ( 640*480 ) into 80*25 character cells.
     */

    float size;

    if ( dim==YD_HORIZ )	size = relativeSize * ( 640.0/80 );
    else			size = relativeSize * ( 480.0/25 );

    return (long) ( size + 0.5 );
}


#include "YQSpacing.moc.cc"
