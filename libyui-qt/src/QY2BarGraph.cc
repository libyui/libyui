/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|			   contributed Qt widgets		       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      QY2BarGraph.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt widget - it can be used independently of YaST2.

/-*/


#include <qpainter.h>
#include <qnamespace.h>
#include "QY2BarGraph.h"

using std::max;


#define noUSE_3D_SEGMENTS


#define LabelHorizontalMargin	1
#define LabelVerticalMargin	2
#define MinWidth		80
#define MinHeight		30



#define INDEX_INVALID(I) index_invalid(i, __FUNCTION__)


QY2BarGraph::QY2BarGraph( QWidget *parent, const char *name )
    : QFrame(parent, name)
{
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    setLineWidth( 2 );
    setMidLineWidth( 2 );
}


void QY2BarGraph::addSegment( const QString &label, int value )
{
    _values.resize( _values.size()+1 );
    _labels.resize( segments() );

    _values.push_back( value );
    _labels.push_back( label );
}


void QY2BarGraph::setSegments( int segmentCount )
{
    if ( segmentCount < 0 )
    {
	qWarning( "QY2BarGraph::setSegments(): "
		  "Invalid segment count %d", segmentCount );
	return;
    }

    _values.resize( segmentCount );
    _labels.resize( segmentCount );

    for ( int i=0; i < segments(); i++ )
    {
	_values[i] = 0;
	_labels[i] = QString();
    }
}


void QY2BarGraph::setValue( int i, int newValue )
{
    if ( INDEX_INVALID(i) ) return;
    _values[i] = newValue;
}


void QY2BarGraph::setLabel( int i, const QString &newLabel )
{
    if ( INDEX_INVALID(i) ) return;
    _labels[i] = newLabel;
}


int QY2BarGraph::value( int i ) const
{
    if ( INDEX_INVALID(i) ) return -1;
    return _values[i];
}


const QString QY2BarGraph::label( int i ) const
{
    if ( INDEX_INVALID(i) ) return QString();
    return _labels[i];
}


int QY2BarGraph::sumValues() const
{
    int sum = 0;

    for ( int i=0; i < segments(); i++ )
    {
	sum += _values[i];
    }

    return sum;
}


void QY2BarGraph::drawContents( QPainter *p )
{
    int totalWidth = contentsRect().width();
    int segHeight  = contentsRect().height();
    int x_off = 0;
    int y_off = 0;
    int sum = sumValues();

    if ( sum == 0 )
	return;

    for ( int i=0; i < segments(); i++ )
    {
	int segWidth = ( (long) totalWidth * _values[i] ) / sum;

	if ( i == segments()-1 )
	{
	    // Compensate for rounding errors:
	    // The last segment gets all leftover pixels from the previous ones.

	    segWidth = contentsRect().width() - x_off;
	}

	// Fill the segment

	p->setBrush( segmentBackgroundColor(i) );
	p->setPen( NoPen );
	p->drawRect( x_off, y_off, segWidth+2, segHeight+2 );


#ifdef USE_3D_SEGMENTS

	// Experimental:
	// Draw 3D borders to the right of the segment for all but the rightmost.

	if ( i < segments()-1 )
	{
	    p->setPen( SolidLine );

	    p->setPen( segmentBackgroundColor(i).light().light() );
	    p->drawLine( x_off + segWidth-2, y_off,
			 x_off + segWidth-2, y_off + segHeight+1 );

	    p->setPen( segmentBackgroundColor(i+1).dark().dark() );
	    p->drawLine( x_off+1 + segWidth-2, y_off,
			 x_off+1 + segWidth-2, y_off + segHeight+2 );
	}
#endif


	// Draw the label

	p->setPen( SolidLine );
	p->setPen( segmentForegroundColor(i) );
	QString txt = _labels[i];

	if ( txt.contains( "%1" ) )
	{
	    txt = txt.arg( _values[i] );		// substitute variable
	}
	p->drawText( x_off + LabelHorizontalMargin,
		     y_off + LabelVerticalMargin,
		     segWidth  - 2 * LabelHorizontalMargin + 1,
		     segHeight - 2 * LabelVerticalMargin   + 1,
		     AlignCenter, txt );

	// Prepare for the next segment

	x_off += segWidth;
    }
}


const QColor QY2BarGraph::segmentBackgroundColor( int i )
{
    switch( i % 8 )
    {
	case 0:	return QColor(   0,   0, 128 );	// dark blue
	case 1:	return QColor(  64, 200, 255 );	// medium blue
	case 2:	return QColor( 255, 255, 255 ); // white
	case 3:	return QColor(   0, 153, 153 );	// cadet blue
	case 4:	return QColor( 150, 255, 255 ); // cyan
	case 5:	return QColor( 100, 100, 100 ); // medium grey
	case 6:	return QColor(   0, 200, 100 ); // medium green
	case 7:	return QColor(   0, 100,  76 ); // dark green
    }

    return( Qt::white ); // just to make gcc happy
}


const QColor QY2BarGraph::segmentForegroundColor( int i )
{
    switch( i % 8 )
    {
	case 0:	return Qt::white;
	case 1:	return Qt::black;
	case 2:	return Qt::black;
	case 3:	return Qt::black;
	case 4:	return Qt::black;
	case 5:	return Qt::white;
	case 6:	return Qt::black;
	case 7:	return Qt::white;
    }

    return( Qt::black ); // just to make gcc happy
}


bool QY2BarGraph::index_invalid( int i, const char *function_name ) const
{
    if ( i < 0 || i >= segments() )
    {
	qWarning( "QY2BarGraph::%s: Index %d out of range", function_name, i );
	return true;
    }
    else
	return false;
}


QSize QY2BarGraph::sizeHint() const
{
    int		width  = 0;
    int		height = MinHeight;
    QFontMetrics metrics = fontMetrics();

    for ( int i=0; i < segments(); i++ )
    {
	QString txt;
	txt.sprintf( _labels[i], _values[i] );
	QSize segSize = metrics.size( 0, txt );
	width += segSize.width();
	height = max( height, segSize.height() );
    }

    height += 2 * LabelVerticalMargin;
    height += frameWidth();
    height  = max( height, MinHeight );

    width  += 2 * LabelHorizontalMargin;
    width  += frameWidth();
    width   = max( width, MinWidth );

    return QSize( width, height );
}


#include "QY2BarGraph.moc.cc"
