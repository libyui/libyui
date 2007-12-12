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
#include <QPointF>

#include "YQUI.h"
#include "YQMultiProgressMeter.h"



YQMultiProgressMeter::YQMultiProgressMeter( YWidget *			parent,
					    YUIDimension		dim,
					    const vector<float> &	maxValues )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YMultiProgressMeter( parent, dim, maxValues )
{
    _triangularShaped = false;
    init();
    setWidgetRep( this );
}


YQMultiProgressMeter::~YQMultiProgressMeter()
{
    // NOP
}


void YQMultiProgressMeter::init()
{
    _margin 		= 2;
    _segmentMinLength 	= 12;
    _triSpacing		= 1;

    if ( triangularShaped() )
    {
	_spacing	= 0;
	setTriThickness( -1 );
    }
    else
    {
	_spacing	= 2;
	setTriThickness( 4 );
    }
}


void YQMultiProgressMeter::setTriangularShaped( bool triangular )
{
    _triangularShaped = triangular;
    init();
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

//     if ( ! event->erased() )
// 	   painter.eraseRect( event->rect() );

    int totalLength 	= horizontal() ? width() : height();
    int thickness 	= horizontal() ? height() : width();

    totalLength 	-= 2 * margin() + spacing() * ( segments()-1 );
    thickness		-= 2 * margin();

    if ( triThickness() > 0 )
	thickness -= 2 * triThickness() + 2 * triSpacing();

    if ( totalLength < 1 || thickness < 1 || segments() < 1 )
	return;


    // Add up the total sum of all maxValues

    float totalSum = 0.0;

    for( int i=0; i < segments(); i++ )
	totalSum += maxValue( i );


    // Figure out minimal segment length

    int minLength = segmentMinLength();


    // Limit the minimum if there isn't even that much space

    if ( minLength * segments() > totalLength )
	minLength = totalLength / ( 2 * segments() );


    // First attempt of scaling factor from values to pixel coordinates

    if ( totalSum == 0.0 )
    {
	y2error( "Avoiding division by zero: totalSum" );
	return;
    }

    float scale = ( (float) totalLength ) / totalSum;
    float scaledMinLength = ( (float) minLength ) / scale;


    // Check how many segments would become smaller than the minimum

    int smallSegmentsCount = 0;
    float restSum = 0.0;

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

    if ( restSum == 0.0 )
    {
	y2error( "Avoiding division by zero: restSum" );
	return;
    }

    // Recompute scale to take small segments into account that now get screen
    // space disproportional to their real size (maxValue).
    scale = ( (float) distributableLength ) / ( restSum );


    // Calculate indentation

    int indent = triangularShaped() ? (int) ( thickness * 0.37 ) : 0;


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


void YQMultiProgressMeter::mouseDoubleClickEvent ( QMouseEvent * event )
{
    if ( event && event->button() == Qt::RightButton )
    {
	// Easter egg: Switch between rectangular and triangular shape

	y2milestone( "Switching shape" );
	setTriangularShaped( ! triangularShaped() );
	setSize( vertical()   ? preferredWidth()  : width(),
		 horizontal() ? preferredHeight() : height() );
	YQUI::ui()->evaluateRecalcLayout();
	QWidget::update();
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
    // Vertical MultiProgressMeters will be filled thermometer-like from bottom
    // to top, horizontal ones like normal progress bars from left to right,
    // i.e. just the opposite way.
    //

    int fillStart  = 0;
    int fillHeight = 0;
    int border = margin();

    if ( triThickness() > 0 )
	border += triThickness() + triSpacing();

    if ( currentValue( segment ) < maxValue( segment ) )
    {
	if ( maxValue( segment ) == 0.0 )
	{
	    y2error( "Avoiding division by zero: maxValue[%d]", segment );
	    return;
	}

	float emptyPart = 1.0 - ( currentValue( segment ) ) / ( maxValue( segment ) );
	fillStart  = (int) ( length * emptyPart );
	fillHeight = (int) ( indent * emptyPart );
    }

    thickness--; // We always deal with tickness-1 anyway, so let's cut this short

    if ( vertical() )	// fill thermometer-like from bottom to top
    {
	if ( fillStart < length )
	{
            static const QPointF points[4] = 
              { QPointF( offset + fillStart,	border + fillHeight ),
	        QPointF( offset + fillStart, 	border + thickness - fillHeight ),
	        QPointF( offset + length, 	border + thickness - indent ),
	        QPointF( offset + length, 	border + indent )
              };

	    painter.setBrush( palette().highlight() );
	    painter.setPen( Qt::NoPen );
	    painter.drawConvexPolygon( points, 4 );
	}
    }
    else	// horizontal - fill from left to right like a normal progress bar
    {
	if ( fillStart > 0 )
	{
            static const QPointF points[4] =
              { QPointF( offset, 		border + thickness ),
	        QPointF( offset, 		border ),
	        QPointF( offset + fillStart,	border + fillHeight ),
	        QPointF( offset + fillStart, 	border + thickness - fillHeight )
              };

	    painter.setBrush( palette().highlight() );
	    painter.setPen( Qt::NoPen );
	    painter.drawConvexPolygon( points, 4 );
	}
    }


    //
    // Draw outline
    //

    const QBrush & dark  = palette().dark();
    const QBrush & light = palette().light();

    // Draw arrow base (left)

    painter.setBrush( dark );
    painter.setPen( Qt::SolidLine );
    painter.drawLine( offset, border,
		      offset, border + thickness );


    // Draw upper outline

    painter.drawLine( offset, border,
		      offset + length - 1, border + indent );

    // Draw arrow point (right)

    painter.setBrush( light );
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

    const QBrush & color = palette().foreground();
    painter.setBrush( color );
    // painter.setBrush( NoBrush );


    // Draw upper marker triangle

    int tri = triThickness();
      
    static const QPointF points[3] = 
     { QPointF( offset - tri+1,	margin() ),		// top left (base)
       QPointF( offset,		margin() + tri-1 ),	// lower center (point)
       QPointF( offset + tri-1, 	margin() )		// top right (base)
     };

    painter.drawConvexPolygon( points, 3 );


    // Draw lower marker triangle

    int pointOffset = margin() + tri + thickness + 2 * triSpacing();

    static const QPointF points2[3] =
     { QPointF( offset,		pointOffset ),		// top center (point)
       QPointF( offset + tri-1,	pointOffset + tri-1 ),	// top right (base)
       QPointF( offset - tri+1,	pointOffset + tri-1 )	// bottom left (base)
     };

    painter.drawConvexPolygon( points2, 3 );
}


int YQMultiProgressMeter::thickness()
{
    int thickness = triangularShaped() ? 35 : 23;
    thickness += 2 * margin();

    if ( triThickness() > 0 )
	thickness += 2 * triThickness() + 2 * triSpacing();

    return thickness;
}


int YQMultiProgressMeter::length()
{
    int length = 70 * segments() + 2 * margin();

    return length;
}


void YQMultiProgressMeter::setTriThickness( int value )
{
    _triThickness = value;

    if ( _triThickness < 1 )
	setTriSpacing( 0 );
}


void YQMultiProgressMeter::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    QWidget::update();
    YWidget::setEnabled( enabled );
}


int YQMultiProgressMeter::preferredWidth()
{
    return horizontal() ? length() : thickness();
}


int YQMultiProgressMeter::preferredHeight()
{
    return horizontal() ? thickness() : length();
}


void YQMultiProgressMeter::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    doUpdate();
}


#include "YQMultiProgressMeter.moc"
