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

#define TRIANGLE_SHAPE	0


YQMultiProgressMeter::YQMultiProgressMeter( QWidget *		parent,
					    const YWidgetOpt & 	opt,
					    bool 		horizontal,
					    const YCPList &	maxValues )
    : QWidget( parent )
    , YMultiProgressMeter( opt, horizontal, maxValues )
{
    _margin 		= 2;
    _spacing		= 2;
    _segmentMinLength 	= 12;
    _triSpacing		= 1;
    setTriThickness( 4 );

#if TRIANGLE_SHAPE
    _spacing		= 0;
    setTriThickness( -1 );
#endif

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
    int thickness 	= horizontal() ? height() : width();

    totalLength 	-= 2 * margin() + spacing() * ( segments()-1 );
    thickness		-= 2 * margin();

    if ( triThickness() > 0 )
	thickness -= 2 * triThickness() + 2 * triSpacing();

    if ( totalLength < 1 || thickness < 1 || segments() < 1 )
	return;


    // Add up the total sum of all maxValues

    int totalSum = 0;

    for( int i=0; i < segments(); i++ )
	totalSum += maxValue( i );


    // Figure out minimal segment length

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


    // Calculate indentation

#if TRIANGLE_SHAPE
    int indent = (int) ( thickness * 0.37 );
#else
    int indent = 0;
#endif


    // Set up painter

    if ( vertical() )
    {
	painter.rotate( 90 );
	painter.scale( 1.0, -1.0 );
    }

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

	if ( i > 0 )
	    drawMarkers( painter, offset, thickness );

	offset += length + spacing();
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
    int border = margin();

    if ( triThickness() > 0 )
	border += triThickness() + triSpacing();

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

    thickness--; // We always deal with tickness-1 anyway, so let's cut this short

    if ( fillStart < length )
    {
	QPointArray points( 4 );
	int p=0;
	points.setPoint( p++, offset + fillStart,	border + fillHeight );
	points.setPoint( p++, offset + fillStart, 	border + thickness - fillHeight );
	points.setPoint( p++, offset + length, 		border + thickness - indent );
	points.setPoint( p++, offset + length, 		border + indent );

	painter.setBrush( palette().active().highlight() );
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
    painter.drawLine( offset, border,
		      offset, border + thickness );


    // Draw upper outline

    painter.drawLine( offset, border,
		      offset + length - 1, border + indent );

    // Draw arrow point (right)

    painter.setPen( light );
    painter.drawLine( offset + length - 1, border + indent,
		      offset + length - 1, border + thickness - indent );

    // Draw lower outline

    painter.drawLine( offset, border + thickness,
		      offset + length - 1, border + thickness - indent );

}


void YQMultiProgressMeter::drawMarkers( QPainter & painter, int offset, int thickness )
{
    if ( triThickness() < 1 )
	return;

    offset -= spacing() / 2 + 1; 	// integer division rounds down!

    const QColor & color = palette().active().foreground();
    painter.setPen( color );
    painter.setPen( SolidLine );
    painter.setBrush( color );
    // painter.setBrush( NoBrush );
    

    // Draw upper marker triangle

    int tri = triThickness();
    QPointArray points( 3 );

    int p=0;
    points.setPoint( p++, offset - tri+1,	margin() );		// top left (base)
    points.setPoint( p++, offset,		margin() + tri-1 );	// lower center (point)
    points.setPoint( p++, offset + tri-1, 	margin() );		// top right (base)

    painter.drawConvexPolygon( points );


    // Draw lower marker triangle

    int pointOffset = margin() + tri + thickness + 2 * triSpacing();

    p=0;
    points.setPoint( p++, offset,		pointOffset );		// top center (point)
    points.setPoint( p++, offset + tri-1, 	pointOffset + tri-1 );	// top right (base)
    points.setPoint( p++, offset - tri+1,	pointOffset + tri-1 );	// bottom left (base)

    painter.drawConvexPolygon( points );
}


void YQMultiProgressMeter::setEnabling( bool enabled )
{
    QWidget::setEnabled( enabled );
    QWidget::update();
}


long YQMultiProgressMeter::nicesize( YUIDimension dim )
{
    int length = 70 * segments() + 2 * margin();

#if TRIANGLE_SHAPE
    int thickness = 35;
#else
    int thickness = 23;
#endif
    thickness += 2 * margin();

    if ( triThickness() > 0 )
	thickness += 2 * triThickness() + 2 * triSpacing();

    if ( dim == YD_HORIZ )	return horizontal() ? length : thickness;
    else 			return horizontal() ? thickness : length;
}


void YQMultiProgressMeter::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
    doUpdate();
}


void YQMultiProgressMeter::setTriThickness( int value )
{
    _triThickness = value;

    if ( _triThickness < 1 )
	setTriSpacing( 0 );
}


#include "YQMultiProgressMeter.moc"
