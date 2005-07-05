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

  File:	      YQAlignment.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include <qpixmap.h>
#include "YQAlignment.h"

using std::string;


YQAlignment::YQAlignment( QWidget *	  	parent,
			  const YWidgetOpt &	opt,
			  YAlignmentType 	horAlign,
			  YAlignmentType 	vertAlign )
    : QWidget( parent )
    , YAlignment( opt, horAlign, vertAlign )
{
    setWidgetRep( this );
}


void YQAlignment::setEnabling( bool enabled )
{
    QWidget::setEnabled( enabled );
}


void YQAlignment::moveChild( YWidget * child, long newX, long newY )
{
    QWidget * qw = (QWidget *) ( child->widgetRep() );
    qw->move( newX, newY );
}


void YQAlignment::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
    YAlignment::setSize( newWidth, newHeight );
}


void YQAlignment::setBackgroundPixmap( string pixmapName )
{
    YAlignment::setBackgroundPixmap( pixmapName );	// Prepend path etc.
    pixmapName = YAlignment::backgroundPixmap();

    if ( pixmapName.empty() )	// Delete any old background pixmap
    {
	unsetPalette();
    }
    else			// Set a new background pixmap
    {
	QPixmap pixmap( pixmapName.c_str() );

	if ( pixmap.isNull() )
	{
	    y2error( "Can't load background pixmap %s", pixmapName.c_str() );
	}
	else
	{
	    setPaletteBackgroundPixmap( pixmap );
	}
    }
}

#include "YQAlignment.moc"
