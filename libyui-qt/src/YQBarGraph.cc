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

  File:	      YQBarGraph.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "YQBarGraph.h"

#define HORIZONTAL_MARGIN	4
#define VERTICAL_MARGIN		4


YQBarGraph::YQBarGraph( QWidget * parent, YWidgetOpt & opt )
    : QWidget( parent )
    , YBarGraph( opt )
{
    setWidgetRep( this );
    _barGraph = new QY2BarGraph( this );
    _barGraph->setFont( YUIQt::ui()->currentFont() );
    _barGraph->move( HORIZONTAL_MARGIN, VERTICAL_MARGIN );
}


void YQBarGraph::doUpdate()
{
    QString msg;

    _barGraph->setSegments( segments() );

    for ( int i=0; i < segments(); i++ )
    {
	_barGraph->setValue( i, value(i) );
	_barGraph->setLabel( i, fromUTF8( label(i) ) );
    }

    _barGraph->update();
}


void YQBarGraph::setEnabling( bool enabled )
{
    _barGraph->setEnabled( enabled );
}


long YQBarGraph::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )	return _barGraph->sizeHint().width()  + 2*HORIZONTAL_MARGIN;
    else 			return _barGraph->sizeHint().height() + 2*VERTICAL_MARGIN;
}


void YQBarGraph::setSize( long newWidth, long newHeight )
{
    _barGraph->move( HORIZONTAL_MARGIN, VERTICAL_MARGIN );
    _barGraph->resize( newWidth  - 2*HORIZONTAL_MARGIN,
		       newHeight - 2*VERTICAL_MARGIN );
    resize( newWidth, newHeight );
}



#include "YQBarGraph.moc.cc"
