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


#include <QPixmap>
#include <QHeaderView>
#include <QMouseEvent>
#include "QY2ListView.h"

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

QY2ListView::QY2ListView( QWidget * parent )
    : QTreeWidget( parent )
    , _mousePressedItem(0)
    , _mousePressedCol( -1 )
    , _mousePressedButton( Qt::NoButton )
    , _sortByInsertionSequence( false )
    , _nextSerial(0)
    , _mouseButton1PressedInHeader( false )
    , _finalSizeChangeExpected( false )
{
    //FIXME QTreeWidget::setShowToolTips( false );
    setRootIsDecorated(false);

#if FIXME_TOOLTIP
    _toolTip = new QY2ListViewToolTip( this );
#endif

     if ( header() )
       header()->installEventFilter( this );

#if FIXME
    connect( this,	SIGNAL( columnResized        ( int, int, int ) ),
	     this,	SLOT  ( columnWidthChanged( int, int, int ) ) );
#endif

    connect( this,      SIGNAL( itemExpanded ( QTreeWidgetItem *) ),
             this,      SLOT  ( treeExpanded ( QTreeWidgetItem *)  ) );

    connect( this,      SIGNAL( itemCollapsed ( QTreeWidgetItem *) ),
             this,      SLOT  ( treeCollapsed ( QTreeWidgetItem *)  ) );

}


QY2ListView::~QY2ListView()
{
#if FIXME_TOOLTIP
    if ( _toolTip )
	delete _toolTip;
#endif
}


void
QY2ListView::selectSomething()
{
    QTreeWidgetItemIterator it( this );

    while ( *it )
    {
	QY2ListViewItem * item = dynamic_cast<QY2ListViewItem *> (*it);

	if ( item && (item->flags() & Qt::ItemIsSelectable) )
	{
	    item->setSelected(true); // emits signal, too
	    return;
	}

	++it;
    }
}


void
QY2ListView::clear()
{
    QTreeWidget::clear();
    restoreColumnWidths();
}


void
QY2ListView::updateItemStates()
{
    QTreeWidgetItemIterator it( this );

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
    QTreeWidgetItemIterator it( this );

    while ( *it )
    {
	QY2ListViewItem * item = dynamic_cast<QY2ListViewItem *> (*it);

	if ( item )
	    item->updateData();

	++it;
    }
}


QString
QY2ListView::toolTip( QTreeWidgetItem * listViewItem, int column )
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
    _savedColumnWidth.reserve( columnCount() );

    for ( int i = 0; i < columnCount(); i++ )
    {
	_savedColumnWidth.push_back( columnWidth(i) );
    }
}


void
QY2ListView::restoreColumnWidths()
{
    if ( _savedColumnWidth.size() != (unsigned) columnCount() ) 	// never manually resized
    {
	for ( int i = 0; i < columnCount(); i++ )		// use optimized column width
	    resizeColumnToContents(i);
    }
    else						// stored settings after manual resizing
    {
	for ( int i = 0; i < columnCount(); i++ )
	{
	    setColumnWidth( i, _savedColumnWidth[ i ] ); // restore saved column width
	}
    }
}


void
QY2ListView::mousePressEvent( QMouseEvent * ev )
{
    //y2internal("POS is %d %d", ev->pos().x(), ev->pos().y() );
    QTreeWidgetItem * item = itemAt( ev->pos() );


    if ( item && ( item->flags() & Qt::ItemIsEnabled ) )
    {
	_mousePressedItem	= item;
                _mousePressedCol	= header()->logicalIndexAt( ev->pos().x() );
	_mousePressedButton	= ev->button();
    }
    else	// invalidate last click data
    {
	_mousePressedItem	= 0;
	_mousePressedCol	= -1;
	_mousePressedButton	= -1;
    }

    // Call base class method
    QTreeWidget::mousePressEvent( ev );
}


void
QY2ListView::mouseReleaseEvent( QMouseEvent * ev )
{
    //y2internal("REPOS is %d %d", ev->pos().x(), ev->pos().y() );
    QTreeWidgetItem * item = itemAt( ev->pos() );

    if ( item && ( item->flags() & Qt::ItemIsEnabled ) && item == _mousePressedItem )
    {
 	int col = header()->logicalIndexAt( ev->pos().x() );
        //y2internal("COL %d", col);
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
    _mousePressedButton	= Qt::NoButton;

    // Call base class method
    QTreeWidget::mouseReleaseEvent( ev );
}


void
QY2ListView::mouseDoubleClickEvent( QMouseEvent * ev )
{
    QTreeWidgetItem * item = itemAt( mapToGlobal( ev->pos() ) );

    if ( item && ( item->flags() & Qt::ItemIsEnabled ) )
    {
 	int col = header()->logicalIndexAt( ev->pos().x() );
 	emit( columnDoubleClicked( ev->button(), (QY2ListViewItem *) item, col, ev->globalPos() ) );
     }

     // invalidate last click data

     _mousePressedItem	= 0;
     _mousePressedCol	= -1;
     _mousePressedButton	= Qt::NoButton;

     // Call base class method
     QTreeWidget::mouseDoubleClickEvent( ev );
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

    return QTreeWidget::eventFilter( obj, event );
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
    //FIXME sort();
    header()->setClickable( ! _sortByInsertionSequence );

}

#if 0
class QY2ListViewItemDelegate : public QItemDelegate
{
public:
    QY2ListViewItemDelegate( QTreeWidget *parent ) : QItemDelegate( parent ) {}

    virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        painter->save();

        QY2ListViewItem *item = dynamic_cast<QY2ListViewItem *>(_view->itemFromIndex(index));
        if ( item )
        {
        }
        painter->restore();
    }
};
#endif

QY2ListViewItem::QY2ListViewItem( QY2ListView * 	parentListView,
				  const QString &	text )
    : QTreeWidgetItem( parentListView, QStringList(text), 1)
{
    _serial = parentListView->nextSerial();
}


QY2ListViewItem::QY2ListViewItem( QTreeWidgetItem * 	parentItem,
				  const QString &	text )
    : QTreeWidgetItem( parentItem, QStringList(text), 1 )
{
    _serial = 0;

    QY2ListView * parentListView = dynamic_cast<QY2ListView *> ( treeWidget() );

    if ( parentListView )
	_serial = parentListView->nextSerial();
}


QY2ListViewItem::~QY2ListViewItem()
{
    // NOP
}

bool
QY2ListViewItem::operator< ( const QTreeWidgetItem & otherListViewItem ) const
{
    bool sortByInsertionSequence = false;
    QY2ListView * parentListView = dynamic_cast<QY2ListView *> (treeWidget());

    if ( parentListView )
	sortByInsertionSequence = parentListView->sortByInsertionSequence();

    if ( sortByInsertionSequence )
    {
	const QY2ListViewItem * other = dynamic_cast<const QY2ListViewItem *> (&otherListViewItem);

	if ( other )
	{
            return ( this->serial() < other->serial() );
	}

	// Still here? Try the other version: QY2CheckListItem.

	const QY2CheckListItem * otherCheckListItem = dynamic_cast<const QY2CheckListItem *> (&otherListViewItem);

	if ( otherCheckListItem )
	{
	    return ( this->serial() < otherCheckListItem->serial() );
	}

    }

    return QTreeWidgetItem::operator<(otherListViewItem);
}

QY2CheckListItem::QY2CheckListItem( QY2ListView * 		parentListView,
				    const QString &		text )
    : QY2ListViewItem( parentListView, text)
{
    setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    setCheckState(0, Qt::Unchecked);
    _serial = parentListView->nextSerial();
}


QY2CheckListItem::QY2CheckListItem( QTreeWidgetItem * 		parentItem,
				    const QString &		text )
    : QY2ListViewItem( parentItem, text)
{
    _serial = 0;
    QY2ListView * parentListView = dynamic_cast<QY2ListView *> ( treeWidget() );

    setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    setCheckState(0, Qt::Unchecked);

    if ( parentListView )
	_serial = parentListView->nextSerial();
}

QY2CheckListItem::~QY2CheckListItem()
{
    // NOP
}



#if FIXME_ITEM_COLORS
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

    QTreeWidgetItem::paintCell( painter, cg, column, width, alignment );
}
#endif


#if FIXME_TOOLTIP
void
QY2ListViewToolTip::maybeTip( const QPoint & pos )
{
    Q3Header *       header        = _listView->header();
    QTreeWidgetItem * item          = _listView->itemAt( pos );

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

#endif


void QY2ListView::treeExpanded( QTreeWidgetItem * listViewItem )
{
    if ( columnCount() == 1 && header() && header()->isHidden() )
	resizeColumnToContents( 0 );
}


void QY2ListView::treeCollapsed( QTreeWidgetItem * listViewItem )
{
    if ( columnCount() == 1  && header() && header()->isHidden())
	resizeColumnToContents( 0 );
}




#include "QY2ListView.moc"
