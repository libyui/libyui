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
#include "Y2QtComponent.h"
#include "YQBarGraph.h"


YQBarGraph::YQBarGraph( QWidget * parent, YWidgetOpt & opt )
    : QWidget( parent )
    , YBarGraph( opt )
{
    setWidgetRep( this );
    _barGraph = new QY2BarGraph( this );
    _barGraph->setFont( Y2QtComponent::ui()->currentFont() );
    _barGraph->move( YQWidgetMargin, YQWidgetMargin );
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
    if ( dim == YD_HORIZ )	return _barGraph->sizeHint().width()  + 2*YQWidgetMargin;
    else 			return _barGraph->sizeHint().height() + 2*YQWidgetMargin;
}


void YQBarGraph::setSize( long newWidth, long newHeight )
{
    _barGraph->move( YQWidgetMargin, YQWidgetMargin );
    _barGraph->resize( newWidth  - 2*YQWidgetMargin,
		       newHeight - 2*YQWidgetMargin );
    resize( newWidth, newHeight );
}



#include "YQBarGraph.moc.cc"
