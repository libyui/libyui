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

  This is a pure Qt widget - it can be used independently of YaST2.

/-*/


#include "QY2DiskUsageList.h"
#include "YQi18n.h"


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
	addColumn( _( "Disk Usage" 	) );	_percentageBarCol	= numCol++;
	addColumn( "" 			  );	_percentageCol		= numCol++;
	addColumn( _( "Used"		) );	_usedSizeCol		= numCol++;
	addColumn( _( "Free"		) );	_freeSizeCol		= numCol++;
	addColumn( _( "Total"		) );	_totalSizeCol		= numCol++;
	addColumn( _( "Device"		) );	_deviceNameCol		= numCol++;
	
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
    init( true );
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
    if ( freeSizeCol()		>= 0 ) setText( usedSizeCol(),		freeSize() 	);

    if ( allFields )
    {
	if ( totalSizeCol()	>= 0 ) setText( totalSizeCol(), 	totalSize() 	);
	if ( nameCol()		>= 0 ) setText( nameCol(),		name()		);
	if ( deviceNameCol()	>= 0 ) setText( deviceNameCol(),	deviceName()	);
    }
}


void
QY2DiskUsageListItem::setText( int column, const FSize & size )
{
    QString sizeText = size.asString().c_str();
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

    if ( usedSize() != 0 )
	percent = 100 * ( totalSize() / usedSize() );

    if ( percent > 100 )
	percent = 100;
    
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
	    if ( this->usedPercent() < other->usedPercent() ) 	return -1;
	    if ( this->usedPercent() > other->usedPercent() ) 	return  1;
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
    QY2ListViewItem::paintCell( painter, colorGroup, column, width, alignment );
}






#include "QY2DiskUsageList.moc.cc"
