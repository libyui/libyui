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

  File:	      QY2ListView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt widget - it can be used independently of YaST2.

/-*/

#include <qpixmap.h>
#include <qheader.h>

#include "QY2ListView.h"


QY2ListView::QY2ListView( QWidget *parent )
    : QListView( parent )
    , _mousePressedItem( 0 )
    , _mousePressedCol( -1 )
    , _mousePressedButton( NoButton )
    , _nextSerial( 0 )
{
    connect( header(),	SIGNAL( sizeChange		( int, int, int ) ),
	     this,	SLOT  ( columnWidthChanged	( int, int, int ) ) );
}


QY2ListView::~QY2ListView()
{
    // NOP
}


void
QY2ListView::selectSomething()
{
    QListViewItem * item = firstChild();

    if ( item )
	setSelected( item, true ); // emits signal, too
}


void
QY2ListView::clear()
{
    QListView::clear();
    restoreColumnWidths();
}


void
QY2ListView::updateToplevelItemStates()
{
    QListViewItem * item = firstChild();

    while ( item )
    {
	QY2ListViewItem * qy2_item = dynamic_cast<QY2ListViewItem *> ( item );

	if ( qy2_item )
	    qy2_item->updateStatus();

	item = item->nextSibling();
    }
}


void
QY2ListView::updateToplevelItemData()
{
    QListViewItem * item = firstChild();

    while ( item )
    {
	QY2ListViewItem * qy2_item = dynamic_cast<QY2ListViewItem *> ( item );

	if ( qy2_item )
	    qy2_item->updateData();

	item = item->nextSibling();
    }
}


void
QY2ListView::saveColumnWidths()
{
    _savedColumnWidth.clear();
    _savedColumnWidth.reserve( columns() );

    for ( int i = 0; i < columns(); i++ )
    {
	_savedColumnWidth.push_back( columnWidth( i ) );
    }
}


void
QY2ListView::restoreColumnWidths()
{
    if ( _savedColumnWidth.size() != (unsigned) columns() )
    {
	return;
    }

    for ( int i = 0; i < columns(); i++ )
    {
	setColumnWidth( i, _savedColumnWidth[ i ] );
    }
}


void
QY2ListView::contentsMousePressEvent( QMouseEvent * ev )
{
    QListViewItem * item = itemAt( contentsToViewport( ev->pos() ) );

    if ( item && item->isEnabled() )
    {
	_mousePressedItem	= item;
	_mousePressedCol	= header()->sectionAt( ev->pos().x() );
	_mousePressedButton	= ev->button();
    }
    else	// invalidate last click data
    {
	_mousePressedItem	= 0;
	_mousePressedCol	= -1;
	_mousePressedButton	= -1;
    }

    // Call base class method
    QListView::contentsMousePressEvent( ev );
}


void
QY2ListView::contentsMouseReleaseEvent( QMouseEvent * ev )
{
    QListViewItem * item = itemAt( contentsToViewport( ev->pos() ) );

    if ( item && item->isEnabled() && item == _mousePressedItem )
    {
	int col = header()->sectionAt( ev->pos().x() );

	if ( item == _mousePressedItem	&&
	     col  == _mousePressedCol	&&
	     ev->button() == _mousePressedButton )
	{
	    emit( columnClicked( ev->button(), item, col, ev->globalPos() ) );
	}

    }

    // invalidate last click data

    _mousePressedItem	= 0;
    _mousePressedCol	= -1;
    _mousePressedButton	= NoButton;

    // Call base class method
    QListView::contentsMouseReleaseEvent( ev );
}


void
QY2ListView::contentsMouseDoubleClickEvent( QMouseEvent * ev )
{
    QListViewItem * item = itemAt( contentsToViewport( ev->pos() ) );

    if ( item && item->isEnabled() )
    {
	int col = header()->sectionAt( ev->pos().x() );
	emit( columnDoubleClicked( ev->button(), (QY2ListViewItem *) item, col, ev->globalPos() ) );
    }

    // invalidate last click data

    _mousePressedItem	= 0;
    _mousePressedCol	= -1;
    _mousePressedButton	= NoButton;

    // Call base class method
    QListView::contentsMouseDoubleClickEvent( ev );
}


void
QY2ListView::columnWidthChanged( int, int, int )
{
    saveColumnWidths();
}






QY2ListViewItem::QY2ListViewItem( QY2ListView * parentListView,
				  bool sortByInsertionSequence )
    : QListViewItem( parentListView )
    , _sortByInsertionSequence( sortByInsertionSequence )
{
    _serial = 0;

    if ( _sortByInsertionSequence )
	_serial = parentListView->nextSerial();
}


QY2ListViewItem::QY2ListViewItem(  QListViewItem * parentItem,
				   bool sortByInsertionSequence )
    : QListViewItem( parentItem )
    , _sortByInsertionSequence( sortByInsertionSequence )
{
    _serial = 0;

    if ( _sortByInsertionSequence )
    {
	QY2ListView * parentListView = dynamic_cast<QY2ListView *> ( listView() );

	if ( parentListView )
	    _serial = parentListView->nextSerial();
    }
}


QY2ListViewItem::~QY2ListViewItem()
{
    // NOP
}


/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
QY2ListViewItem::compare( QListViewItem *	otherListViewItem,
			  int			col,
			  bool			ascending ) const
{
    if ( sortByInsertionSequence() )
    {
	QY2ListViewItem * other = dynamic_cast<QY2ListViewItem *> (otherListViewItem);

	if ( other )
	{
	    if ( this->serial() < other->serial() ) return -1;
	    if ( this->serial() > other->serial() ) return  1;
	    return 0;
	}

	// Still here? Try the other version: QY2CheckListItem.

	QY2CheckListItem * otherCheckListItem = dynamic_cast<QY2CheckListItem *> (otherListViewItem);

	if ( otherCheckListItem )
	{
	    if ( this->serial() < otherCheckListItem->serial() ) return -1;
	    if ( this->serial() > otherCheckListItem->serial() ) return  1;
	    return 0;
	}

    }

    return QListViewItem::compare( otherListViewItem, col, ascending );
}


void
QY2ListViewItem::paintCell( QPainter *		painter,
			    const QColorGroup &	colorGroup,
			    int			column,
			    int			width,
			    int			alignment )
{
    QColorGroup cg = colorGroup;

    if ( _textColor.isValid() )		cg.setColor( QColorGroup::Text, _textColor );
    if ( _backgroundColor.isValid() )	cg.setColor( QColorGroup::Base, _backgroundColor );

    QListViewItem::paintCell( painter, cg, column, width, alignment );
}






QY2CheckListItem::QY2CheckListItem( QY2ListView * 		parentListView,
				    const QString &		text,
				    QCheckListItem::Type	type,
				    bool 			sortByInsertionSequence )
    : QCheckListItem( parentListView, text, type )
    , _sortByInsertionSequence( sortByInsertionSequence )
{
    _serial = 0;

    if ( _sortByInsertionSequence )
	_serial = parentListView->nextSerial();
}


QY2CheckListItem::QY2CheckListItem( QListViewItem * 		parentItem,
				    const QString &		text,
				    QCheckListItem::Type	type,
				    bool 			sortByInsertionSequence )
    : QCheckListItem( parentItem, text, type )
    , _sortByInsertionSequence( sortByInsertionSequence )
{
    _serial = 0;

    if ( _sortByInsertionSequence )
    {
	QY2ListView * parentListView = dynamic_cast<QY2ListView *> ( listView() );

	if ( parentListView )
	    _serial = parentListView->nextSerial();
    }
}


QY2CheckListItem::~QY2CheckListItem()
{
    // NOP
}


/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
QY2CheckListItem::compare( QListViewItem *	otherListViewItem,
			   int			col,
			   bool			ascending ) const
{
    if ( sortByInsertionSequence() )
    {
	QY2CheckListItem * other = dynamic_cast<QY2CheckListItem *> (otherListViewItem);

	if ( other )
	{
	    if ( this->serial() < other->serial() ) return -1;
	    if ( this->serial() > other->serial() ) return  1;
	    return 0;
	}


	// Still here? Try the other version: QY2ListViewItem.

	QY2ListViewItem * otherCheckListItem = dynamic_cast<QY2ListViewItem *> (otherListViewItem);

	if ( otherCheckListItem )
	{
	    if ( this->serial() < otherCheckListItem->serial() ) return -1;
	    if ( this->serial() > otherCheckListItem->serial() ) return  1;
	    return 0;
	}

    }

    return QListViewItem::compare( otherListViewItem, col, ascending );
}


void
QY2CheckListItem::paintCell( QPainter *			painter,
			     const QColorGroup &	colorGroup,
			     int			column,
			     int			width,
			     int			alignment )
{
    QColorGroup cg = colorGroup;

    if ( _textColor.isValid() )		cg.setColor( QColorGroup::Text, _textColor );
    if ( _backgroundColor.isValid() )	cg.setColor( QColorGroup::Base, _backgroundColor );

    QListViewItem::paintCell( painter, cg, column, width, alignment );
}



#include "QY2ListView.moc.cc"
