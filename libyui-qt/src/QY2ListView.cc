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


QY2ListView::QY2ListView( QWidget * parent )
    : QListView( parent )
    , _mousePressedItem(0)
    , _mousePressedCol( -1 )
    , _mousePressedButton( NoButton )
    , _sortByInsertionSequence( false )
    , _nextSerial(0)
    , _mouseButton1PressedInHeader( false )
    , _finalSizeChangeExpected( false )
{
    QListView::setShowToolTips( false );
    _toolTip = new QY2ListViewToolTip( this );

    if ( header() )
	header()->installEventFilter( this );

    connect( header(),	SIGNAL( sizeChange        ( int, int, int ) ),
	     this,	SLOT  ( columnWidthChanged( int, int, int ) ) );
}


QY2ListView::~QY2ListView()
{
    if ( _toolTip )
	delete _toolTip;
}


void
QY2ListView::selectSomething()
{
    QListViewItemIterator it( this );

    while ( *it )
    {
	QY2ListViewItem * item = dynamic_cast<QY2ListViewItem *> (*it);

	if ( item && item->isSelectable() )
	{
	    setSelected( item, true ); // emits signal, too
	    return;
	}
	
	++it;
    }
}


void
QY2ListView::clear()
{
    QListView::clear();
    restoreColumnWidths();
}


void
QY2ListView::updateItemStates()
{
    QListViewItemIterator it( this );

    while ( *it )
    {
	QY2ListViewItem * item = dynamic_cast<QY2ListViewItem *> (*it);

	if ( item )
	    item->updateStatus();
	
	++it;
    }
}


void
QY2ListView::updateItemData()
{
    QListViewItemIterator it( this );

    while ( *it )
    {
	QY2ListViewItem * item = dynamic_cast<QY2ListViewItem *> (*it);

	if ( item )
	    item->updateData();
	
	++it;
    }
}


QString
QY2ListView::toolTip( QListViewItem * listViewItem, int column )
{
    if ( ! listViewItem )
	return QString::null;

    QString text;

#if 0
    text.sprintf( "Column %d:\n%s", column, (const char *) listViewItem->text( column ) );
#endif

    // Try known item classes

    QY2ListViewItem * item = dynamic_cast<QY2ListViewItem *> (listViewItem);

    if ( item )
	return item->toolTip( column );

    QY2CheckListItem * checkListItem = dynamic_cast<QY2CheckListItem *> (listViewItem);

    if ( checkListItem )
	return checkListItem->toolTip( column );

    return QString::null;
}


void
QY2ListView::saveColumnWidths()
{
    _savedColumnWidth.clear();
    _savedColumnWidth.reserve( columns() );

    for ( int i = 0; i < columns(); i++ )
    {
	_savedColumnWidth.push_back( columnWidth(i) );
    }
}


void
QY2ListView::restoreColumnWidths()
{
    if ( _savedColumnWidth.size() != (unsigned) columns() ) 	// never manually resized
    {
	for ( int i = 0; i < columns(); i++ )		// use optimized column width
	    adjustColumn( i );
    }
    else						// stored settings after manual resizing
    {
	for ( int i = 0; i < columns(); i++ )
	{
	    setColumnWidth( i, _savedColumnWidth[ i ] ); // restore saved column width
	}
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
    // Workaround for Qt bug:
    //
    // QHeader sends a sizeChange() signal for every size change, not only (as
    // documented) when the user resizes a header column manually. But we only
    // want to record the column widths if the user explicitly did that, so
    // ignore those signals if the mouse isn't pressed. There is also one final
    // sizeChange() signal immediately after the user releases the mouse button.

    if ( _mouseButton1PressedInHeader || _finalSizeChangeExpected )
    {
	saveColumnWidths();

	// Consume that one sizeChange() signal that is sent immediately after
	// the mouse button is released, but make sure to reset that flag only
	// when appropriate.

	if ( ! _mouseButton1PressedInHeader )
	    _finalSizeChangeExpected = false;
    }
}


bool
QY2ListView::eventFilter( QObject * obj, QEvent * event )
{
    if ( event && obj && obj == header() )
    {
	if ( event->type() == QEvent::MouseButtonPress )
	{
	    QMouseEvent * mouseEvent = (QMouseEvent *) event;

	    if ( mouseEvent->button() == 1 )
	    {
		_mouseButton1PressedInHeader = true;
		_finalSizeChangeExpected     = false;
	    }
	}
	else if ( event->type() == QEvent::MouseButtonRelease )
	{
	    QMouseEvent * mouseEvent = (QMouseEvent *) event;

	    if ( mouseEvent->button() == 1 )
	    {
		_finalSizeChangeExpected     = true;
		_mouseButton1PressedInHeader = false;
	    }
	}
    }

    return QListView::eventFilter( obj, event );
}


QSize
QY2ListView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}


void
QY2ListView::setSortByInsertionSequence( bool sortByInsertionSequence )
{
    _sortByInsertionSequence = sortByInsertionSequence;
    sort();
}






QY2ListViewItem::QY2ListViewItem( QY2ListView * 	parentListView,
				  const QString &	text )
    : QListViewItem( parentListView, text )
{
    _serial = parentListView->nextSerial();
}


QY2ListViewItem::QY2ListViewItem( QListViewItem * 	parentItem,
				  const QString &	text )
    : QListViewItem( parentItem, text )
{
    _serial = 0;

    QY2ListView * parentListView = dynamic_cast<QY2ListView *> ( listView() );

    if ( parentListView )
	_serial = parentListView->nextSerial();
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
    bool sortByInsertionSequence = false;
    QY2ListView * parentListView = dynamic_cast<QY2ListView *> (listView());

    if ( parentListView )
	sortByInsertionSequence = parentListView->sortByInsertionSequence();
    
    if ( sortByInsertionSequence )
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
				    QCheckListItem::Type	type )
    : QCheckListItem( parentListView, text, type )
{
    _serial = parentListView->nextSerial();
}


QY2CheckListItem::QY2CheckListItem( QListViewItem * 		parentItem,
				    const QString &		text,
				    QCheckListItem::Type	type )
    : QCheckListItem( parentItem, text, type )
{
    _serial = 0;
    QY2ListView * parentListView = dynamic_cast<QY2ListView *> ( listView() );

    if ( parentListView )
	_serial = parentListView->nextSerial();
}


QY2CheckListItem::QY2CheckListItem( QCheckListItem * 		parentItem,
				    const QString &		text,
				    QCheckListItem::Type	type )
    : QCheckListItem( parentItem, text, type )
{
    _serial = 0;
    QY2ListView * parentListView = dynamic_cast<QY2ListView *> ( listView() );

    if ( parentListView )
	_serial = parentListView->nextSerial();
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
    bool sortByInsertionSequence = false;
    QY2ListView * parentListView = dynamic_cast<QY2ListView *> (listView());

    if ( parentListView )
	sortByInsertionSequence = parentListView->sortByInsertionSequence();
    
    if ( sortByInsertionSequence )
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

    QCheckListItem::paintCell( painter, cg, column, width, alignment );
}


void
QY2ListViewToolTip::maybeTip( const QPoint & pos )
{
    QHeader *       header        = _listView->header();
    QListViewItem * item          = _listView->itemAt( pos );

    if ( ! item )
	return;

    int x      = _listView->viewportToContents( pos ).x();
    int column = header->sectionAt(x);
    int indent = 0;

    if ( column == 0 )
    {
	indent  =  item->depth() + ( _listView->rootIsDecorated() ? 1 : 0 );
	indent *=  _listView->treeStepSize();

	if ( pos.x() < indent )
	    column = -1;
    }

    QString text = _listView->toolTip( item, column );

    if ( ! text.isEmpty() )
    {
	QRect rect( _listView->itemRect( item ) );

	if ( column < 0 )
	{
	    rect.setX(0);
	    rect.setWidth( indent );
	}
	else
	{
	    QPoint topLeft( header->sectionPos( column ), 0 );
	    topLeft = _listView->contentsToViewport( topLeft );
 	    rect.setX( topLeft.x() );
	    rect.setWidth( header->sectionSize( column ) );
	}

	tip( rect, text );
    }
}


#include "QY2ListView.moc"
