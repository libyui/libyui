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

  File:	      YQMultiProgressMeter.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include <qevent.h>

#include "YQUI.h"
#include "YQMultiProgressMeter.h"


YQMultiProgressMeter::YQMultiProgressMeter( QWidget *		parent,
					    const YWidgetOpt & 	opt,
					    bool 		horizontal,
					    const YCPList &	maxValues )
    : QWidget( parent )
    , YMultiProgressMeter( opt, horizontal, maxValues )
    , _margin( 2 )
    , _segmentMinLength( 15 )
{
    setWidgetRep( this );
}


void YQMultiProgressMeter::doUpdate()
{
    QWidget::update();
}


void YQMultiProgressMeter::paintEvent ( QPaintEvent * event )
{
    if ( ! event )
	return;

    QPainter painter( this );

    if ( ! event->erased() )
	painter.eraseRect( event->rect() );

    int totalLength 	= horizontal() ? width() : height();
    int totalThickness 	= horizontal() ? height() : width();

    totalLength 	-= 2 * margin();
    totalThickness	-= 2 * margin();

    if ( totalLength < 1 || totalThickness < 1 || segments() < 1 )
	return;


    // Add up the total sum of all maxValues

    int totalSum = 0;

    for( int i=0; i < segments(); i++ )
	totalSum += maxValue( i );


    // Calculate minimal segment length

    int minLength = segmentMinLength();


    // Limit the minimum if there isn't even that much space

    if ( minLength * segments() > totalLength )
	minLength = totalLength / ( 2 * segments() );


    // First attempt of scaling factor from values to pixel coordinates

    if ( totalSum == 0 )
    {
	y2error( "Avoiding division by zero: totalSum" );
	return;
    }

    double scale = ( (double) totalLength ) / ( (double) totalSum );
    int scaledMinLength = (int) ( minLength / scale );

    
    // Check how many segments would become smaller than the minimum

    int smallSegmentsCount = 0;
    int restSum = 0;

    for ( int i=0; i < segments(); i++ )
    {
	if ( maxValue( i ) < scaledMinLength )
	    smallSegmentsCount++;
	else
	    restSum += maxValue( i );
    }


    // Small segments that get at least minLength pixels consume more screen
    // space than initially planned, so recompute what is left for the others

    int distributableLength = totalLength - smallSegmentsCount * minLength;

    if ( restSum == 0 )
    {
	y2error( "Avoiding division by zero: restSum" );
	return;
    }

    // Recompute scale to take small segments into account that now get screen
    // space disproportional to their real size (maxValue).
    scale = ( (double) distributableLength ) / ( (double) restSum );


    // Calculate thickness and indentation

    int thickness = horizontal() ? height() : width();
    int indent    = (int) ( thickness * 0.37 );


    // Set up painter

    if ( vertical() )
    {
	painter.rotate( 90 );
	painter.scale( 1.0, -1.0 );
    }

    painter.setBrush( palette().active().highlight() );
    int offset = margin();


    // Draw each segment in turn

    for ( int i=0; i < segments(); i++ )
    {
	int length;

	if ( maxValue( i ) < scaledMinLength )
	    length = minLength;
	else
	    length = (int) ( maxValue( i ) * scale + 0.5 );

	drawSegment( i, painter, offset, length, thickness, indent );
	offset += length;
    }
}


void YQMultiProgressMeter::drawSegment( int segment,
					QPainter & painter,
					int offset,
					int length,
					int thickness,
					int indent )
{
    //
    // Fill segment
    //

    int fillStart  = 0;
    int fillHeight = 0;

    if ( currentValue( segment ) < maxValue( segment ) )
    {
	if ( maxValue( segment ) == 0 )
	{
	    y2error( "Avoiding division by zero: maxValue[%d]", segment );
	    return;
	}

	double emptyPart = 1.0 - ( (double) currentValue( segment ) ) / ( (double) maxValue( segment ) );
	fillStart  = (int) ( length * emptyPart );
	fillHeight = (int) ( indent * emptyPart );
    }

    if ( fillStart < length )
    {
	QPointArray points( 4 );
	int p=0;
	points.setPoint( p++, offset + fillStart,	margin() + fillHeight );
	points.setPoint( p++, offset + fillStart, 	thickness - margin() - fillHeight );
	points.setPoint( p++, offset + length, 		thickness - margin() - indent );
	points.setPoint( p++, offset + length, 		margin() + indent );

	painter.setPen( NoPen );
	painter.drawConvexPolygon( points );
    }


    //
    // Draw outline
    //

    const QColor & dark  = palette().active().dark();
    const QColor & light = palette().active().light();

    // Draw arrow base (left)

    painter.setPen( dark );
    painter.setPen( SolidLine );
    painter.drawLine( offset, margin(),
		      offset, thickness - margin() );


    // Draw upper outline

    painter.drawLine( offset, margin(),
		      offset + length, margin() + indent );

    // Draw arrow point (right)

    painter.setPen( light );
    painter.drawLine( offset + length, margin() + indent,
		      offset + length, thickness - margin() - indent );

    // Draw lower outline

    painter.drawLine( offset, thickness - margin(),
		      offset + length, thickness - margin() - indent );

}


void YQMultiProgressMeter::setEnabling( bool enabled )
{
    QWidget::setEnabled( enabled );
    QWidget::update();
}


long YQMultiProgressMeter::nicesize( YUIDimension dim )
{
    int thickness = 35 + 2 * margin();
    int length    = 70 * segments() + 2 * margin();

    if ( dim == YD_HORIZ )	return horizontal() ? length : thickness;
    else 			return horizontal() ? thickness : length;
}


void YQMultiProgressMeter::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
    doUpdate();
}



#include "YQMultiProgressMeter.moc"
