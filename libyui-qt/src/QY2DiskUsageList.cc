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

  File:	      QY2DiskUsageList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

  This is a pure Qt widget - it can be used independently of YaST2.


/-*/


#include "QY2DiskUsageList.h"
#include "YQi18n.h"
#include "qpainter.h"


QY2DiskUsageList::QY2DiskUsageList( QWidget * parent, bool addStdColumns )
    : QY2ListView( parent )
{
    _nameCol		= -42;
    _percentageBarCol	= -42;
    _percentageCol	= -42;
    _usedSizeCol	= -42;
    _freeSizeCol	= -42;
    _totalSizeCol	= -42;
    _deviceNameCol	= -42;

    if ( addStdColumns )
    {
	int numCol = 0;
	addColumn( _( "Name" 		) );	_nameCol 		= numCol++;

	// Translators: Please keep this short!
	addColumn( _( "Disk Usage" 	) );	_percentageBarCol	= numCol++;
	addColumn( "" 			  );	_percentageCol		= numCol++;
	addColumn( _( "Used" 		) );	_usedSizeCol		= numCol++;
	addColumn( _( "Free" 		) );	_freeSizeCol		= numCol++;
	addColumn( _( "Total" 		) );	_totalSizeCol		= numCol++;
#if 0
	addColumn( _( "Device" 		) );	_deviceNameCol		= numCol++;
#endif

	setColumnAlignment( percentageCol(),	Qt::AlignRight );
	setColumnAlignment( usedSizeCol(),	Qt::AlignRight );
	setColumnAlignment( freeSizeCol(), 	Qt::AlignRight );
	setColumnAlignment( totalSizeCol(), 	Qt::AlignRight );

	setSorting( percentageBarCol() );
    }

    saveColumnWidths();
    setSelectionMode( QListView::NoSelection );
}


QY2DiskUsageList::~QY2DiskUsageList()
{
}






QY2DiskUsageListItem::QY2DiskUsageListItem( QY2DiskUsageList * parent )
    : QY2ListViewItem( parent )
    , _diskUsageList( parent )
{
}




QY2DiskUsageListItem::~QY2DiskUsageListItem()
{
    // NOP
}




void
QY2DiskUsageListItem::init( bool allFields )
{
    if ( percentageCol()	>= 0 )
    {
	QString percentageText;
	percentageText.sprintf( "%d%%", usedPercent() );
	setText( percentageCol(), percentageText );
    }

    if ( usedSizeCol()		>= 0 ) setText( usedSizeCol(),		usedSize() 	);
    if ( freeSizeCol()		>= 0 ) setText( freeSizeCol(),		freeSize() 	);

    if ( allFields )
    {
	if ( totalSizeCol()	>= 0 ) setText( totalSizeCol(), 	totalSize() 	);
	if ( nameCol()		>= 0 ) setText( nameCol(),		" " + name()	);
	if ( deviceNameCol()	>= 0 ) setText( deviceNameCol(),	deviceName()	);
    }
}


void
QY2DiskUsageListItem::setText( int column, const FSize & size )
{
    QString sizeText = size.form( 0, 1, true ).c_str();
    sizeText += " ";
    setText( column, sizeText );
}


FSize
QY2DiskUsageListItem::freeSize() const
{
    return totalSize() - usedSize();
}


int
QY2DiskUsageListItem::usedPercent() const
{
    int percent = 0;

    if ( totalSize() != 0 )
	percent = ( 100 * usedSize() ) / totalSize();

    return percent;
}


void
QY2DiskUsageListItem::updateStatus()
{
    init( false );
}


void
QY2DiskUsageListItem::updateData()
{
    init( true );
}





/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
QY2DiskUsageListItem::compare( QListViewItem *	otherListViewItem,
			       int		col,
			       bool		ascending ) const
{
    QY2DiskUsageListItem * other = dynamic_cast<QY2DiskUsageListItem *> (otherListViewItem);

    if ( other )
    {
	if ( col == percentageCol()    ||
	     col == percentageBarCol()   )
	{
	    // Intentionally reverting sort order: Fullest first

	    if ( this->usedPercent() < other->usedPercent() ) 	return  1;
	    if ( this->usedPercent() > other->usedPercent() ) 	return -1;
	    return 0;
	}
	else if ( col == usedSizeCol() )
	{
	    if ( this->usedSize() < other->usedSize() ) 	return -1;
	    if ( this->usedSize() > other->usedSize() ) 	return  1;
	    return 0;
	}
	else if ( col == freeSizeCol() )
	{
	    if ( this->freeSize() < other->freeSize() ) 	return -1;
	    if ( this->freeSize() > other->freeSize() ) 	return  1;
	    return 0;
	}
	else if ( col == totalSizeCol() )
	{
	    if ( this->totalSize() < other->totalSize() ) 	return -1;
	    if ( this->totalSize() > other->totalSize() ) 	return  1;
	    return 0;
	}
    }

    return QY2ListViewItem::compare( otherListViewItem, col, ascending );
}


void
QY2DiskUsageListItem::paintCell( QPainter *		painter,
				 const QColorGroup &	colorGroup,
				 int			column,
				 int			width,
				 int			alignment )
{
    if ( column == percentageBarCol() )
    {
	QColor background = colorGroup.base();
	painter->setBackgroundColor( background );

	paintPercentageBar( usedPercent(),
			    painter,
			    _diskUsageList->treeStepSize() * depth(),
			    width,
			    interpolateColor( usedPercent(),
					      60, 95,
					      QColor( 0, 0x80, 0 ),	// Medium dark green
					      QColor( 0xFF, 0, 0 ) ),	// Bright red
			    background.dark( 115 ) );

    }
    else
    {
	QColorGroup cg( colorGroup );

	if ( usedSize() > totalSize() )
	    cg.setColor( QColorGroup::Text, Qt::red );		// Set red text foreground

	// Intentionally bypassing the direct parent class method, use the grandparent's:
	// Don't let QY2ListViewItem::_textColor / _backgroundColor interfere with our colors.

	QListViewItem::paintCell( painter, cg, column, width, alignment );
    }
}


/**
 * Stolen from KDirStat::KDirTreeView with the author's permission.
 **/
void
QY2DiskUsageListItem::paintPercentageBar( float			percent,
					  QPainter *		painter,
					  int			indent,
					  int			width,
					  const QColor &	fillColor,
					  const QColor &	barBackground )
{
    if ( percent > 100.0 )	percent = 100.0;
    if ( percent < 0.0   )	percent = 0.0;
    int penWidth = 2;
    int extraMargin = 3;
    int x = _diskUsageList->itemMargin();
    int y = extraMargin;
    int w = width    - 2 * _diskUsageList->itemMargin();
    int h = height() - 2 * extraMargin;
    int fillWidth;

    painter->eraseRect( 0, 0, width, height() );
    w -= indent;
    x += indent;

    if ( w > 0 )
    {
	QPen pen( painter->pen() );
	pen.setWidth(0);
	painter->setPen( pen );
	painter->setBrush( NoBrush );
	fillWidth = (int) ( ( w - 2 * penWidth ) * percent / 100.0 );


	// Fill bar background.

	painter->fillRect( x + penWidth, y + penWidth,
			   w - 2 * penWidth + 1, h - 2 * penWidth + 1,
			   barBackground );
	/*
	 * Notice: The Xlib XDrawRectangle() function always fills one
	 * pixel less than specified. Altough this is very likely just a
	 * plain old bug, it is documented that way. Obviously, Qt just
	 * maps the fillRect() call directly to XDrawRectangle() so they
	 * inherited that bug ( although the Qt doc stays silent about
	 * it ). So it is really necessary to compensate for that missing
	 * pixel in each dimension.
	 *
	 * If you don't believe it, see for yourself.
	 * Hint: Try the xmag program to zoom into the drawn pixels.
	 **/

	// Fill the desired percentage.

	painter->fillRect( x + penWidth, y + penWidth,
			   fillWidth+1, h - 2 * penWidth+1,
			   fillColor );


	// Draw 3D shadows.

	pen.setColor( contrastingColor ( Qt::black,
					 painter->backgroundColor() ) );
	painter->setPen( pen );
	painter->drawLine( x, y, x+w, y );
	painter->drawLine( x, y, x, y+h );

	pen.setColor( contrastingColor( barBackground.dark(),
					painter->backgroundColor() ) );
	painter->setPen( pen );
	painter->drawLine( x+1, y+1, x+w-1, y+1 );
	painter->drawLine( x+1, y+1, x+1, y+h-1 );

	pen.setColor( contrastingColor( barBackground.light(),
					painter->backgroundColor() ) );
	painter->setPen( pen );
	painter->drawLine( x+1, y+h, x+w, y+h );
	painter->drawLine( x+w, y, x+w, y+h );

	pen.setColor( contrastingColor( Qt::white,
					painter->backgroundColor() ) );
	painter->setPen( pen );
	painter->drawLine( x+2, y+h-1, x+w-1, y+h-1 );
	painter->drawLine( x+w-1, y+1, x+w-1, y+h-1 );
    }
}


/**
 * Stolen from KDirStat::KDirTreeView with the author's permission.
 **/
QColor
QY2DiskUsageListItem::contrastingColor( const QColor & desiredColor,
					const QColor & contrastColor )
{
    if ( desiredColor != contrastColor )
    {
	return desiredColor;
    }

    if ( contrastColor != contrastColor.light() )
    {
	// try a little lighter
	return contrastColor.light();
    }
    else
    {
	// try a little darker
	return contrastColor.dark();
    }
}


QColor
QY2DiskUsageListItem::interpolateColor( int 		val,
					int 		minVal,
					int 		maxVal,
					const QColor & 	minColor,
					const QColor & 	maxColor )
{
    int minH, maxH;
    int minS, maxS;
    int minV, maxV;

    minColor.hsv( &minH, &minS, &minV );
    maxColor.hsv( &maxH, &maxS, &maxV );

    return QColor( interpolate( val, minVal, maxVal, minH, maxH ),
		   interpolate( val, minVal, maxVal, minS, maxS ),
		   interpolate( val, minVal, maxVal, minV, maxV ),
		   QColor::Hsv );
}


int
QY2DiskUsageListItem::interpolate( int from,
				   int minFrom, int maxFrom,
				   int minTo, 	int maxTo 	)
{
    if ( minFrom > maxFrom )
    {
	// Swap min/max values

	int tmp = maxFrom;
	maxFrom = minFrom;
	minFrom = tmp;
    }

    long x = from - minFrom;
    x *= maxTo - minTo;
    x /= maxFrom - minFrom;
    x += minTo;

    if ( minTo < maxTo )
    {
	if ( x < minTo )	x = minTo;
	if ( x > maxTo )	x = maxTo;
    }
    else
    {
	if ( x < maxTo )	x = maxTo;
	if ( x > minTo )	x = minTo;
    }

    return (int) x;
}



#include "QY2DiskUsageList.moc"
