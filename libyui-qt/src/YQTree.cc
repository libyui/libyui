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

  File:	      YQTree.cc

  Author:     Stefan Hundhammer <sh@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qheader.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qlistview.h>
#include <qstring.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::min;
using std::max;

#include "YQUI.h"
#include "YEvent.h"
#include "utf8.h"
#include "YQTree.h"

#define ICON_DIR			ICONDIR "/icons/22x22/apps/"



YQTree::YQTree( QWidget * 		parent,
		const YWidgetOpt &	opt,
		const YCPString & 	label )
    : QVBox( parent )
    , YTree	( opt, label )
{
    setWidgetRep( this );

    setSpacing( YQWidgetSpacing );
    setMargin( YQWidgetMargin );

    _nextSerialNo = 0;

    _qt_label = new QLabel( fromUTF8( label->value() ), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YQUI::ui()->currentFont() );

    _listView = new QListView( this );
    _listView->setFont( YQUI::ui()->currentFont() );
    _listView->addColumn( "" );
    _listView->header()->hide();
    _listView->setRootIsDecorated ( true );

    _qt_label->setBuddy ( _listView );
    
    connect( _listView,	SIGNAL( selectionChanged ( void ) ),
	     this, 	SLOT  ( slotSelected     ( void ) ) );
    
    connect( _listView,	SIGNAL( spacePressed	 ( QListViewItem *	) ),
	     this, 	SLOT  ( slotActivated	 ( void 		) ) );
    
    connect( _listView,	SIGNAL( doubleClicked	 ( QListViewItem * 	) ),
	     this, 	SLOT  ( slotActivated	 ( void 		) ) );
}


void
YQTree::setLabel( const YCPString & label )
{
    _qt_label->setText( fromUTF8( label->value() ) );
    YTree::setLabel( label );
}


long
YQTree::nicesize( YUIDimension dim )
{
    if  ( dim == YD_HORIZ )  return max( 200, _qt_label->sizeHint().width() );
    else return 300 + _qt_label->sizeHint().height();
}


void
YQTree::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void
YQTree::setEnabling( bool enabled )
{
    _qt_label->setEnabled( enabled );
    _listView->setEnabled( enabled );
}


void
YQTree::rebuildTree()
{
    _listView->blockSignals( true );
    _listView->clear();

    for ( YTreeItemListIterator it = items.begin(); it < items.end(); ++it )
    {
	YQTreeItem * item = new YQTreeItem( this,
					    _listView,
					    *it,
					    _nextSerialNo++ );
	YTreeItemList itemList = ( *it)->itemList();

	if ( itemList.size() > 0 )
	{
	    item->buildSubTree( itemList, _nextSerialNo );
	}
    }
    
    _listView->blockSignals( false );
}


const YTreeItem *
YQTree::getCurrentItem() const
{
    YQTreeItem * it = ( YQTreeItem * ) _listView->selectedItem();

    return it ? it->origItem() : 0;
}


void
YQTree::setCurrentItem( YTreeItem * yit )
{
    _listView->blockSignals( true );
    
    YQTreeItem * it = findYQTreeItem( yit );

    if ( it )
    {
	// Select this item

	_listView->setSelected( it, true );


	// Open all parent items so the selected item isn't obscured
	// somewhere deep within the tree

	while ( ( it = ( YQTreeItem * ) it->parent() ) )
	{
	    if ( ! it->isOpen() )
	    {
		it->setOpen( true );
	    }
	}

	_listView->ensureItemVisible( it );
    }
    else
	_listView->clearSelection();
    
    _listView->blockSignals( false );
}


void
YQTree::registerItem( const YTreeItem *		orig,
		      const YQTreeItem *	clone )
{
    _yTreeItemToYQTreeItem.insert( ( void * ) orig, clone );
}


YQTreeItem *
YQTree::findYQTreeItem( const YTreeItem * orig ) const
{
    return _yTreeItemToYQTreeItem[ ( void * ) orig ];
}


void YQTree::slotSelected( void )
{
    if ( getNotify() && ! YQUI::ui()->eventPendingFor( this ) )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
}

void YQTree::slotActivated( void )
{
    if ( getNotify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}


bool
YQTree::setKeyboardFocus()
{
    _listView->setFocus();

    return true;
}


void YQTree::deleteAllItems()
{
    _listView->blockSignals( true );
    _listView->clear();
    YTree::deleteAllItems();
    _listView->blockSignals( false );
}


/*============================================================================*/



YQTreeItem::YQTreeItem( YQTree	*	tree,
			QListView *	parent,
			YTreeItem *	yTreeItem,
			int		serial )
    : QListViewItem( parent )
{
    init( tree, yTreeItem, serial );
}


YQTreeItem::YQTreeItem( YQTree	*	tree,
			YQTreeItem *	parent,
			YTreeItem *	yTreeItem,
			int		serial )
    : QListViewItem( parent )
{
    init( tree, yTreeItem, serial );
}


void
YQTreeItem::init( YQTree *	tree,
		  YTreeItem *	yTreeItem,
		  int		serial )
{
    _tree = tree;
    _tree->registerItem ( yTreeItem, this );
    _origItem = yTreeItem;
    _serialNo = serial;
    setText( 0, fromUTF8 ( _origItem->getText()->value() ) );
    setOpen( _origItem->isOpenByDefault() );

    if ( tree->hasIcons() )
    {
	QString iconName = _origItem->iconName()->value().c_str();
	iconName.stripWhiteSpace();

	if ( ! iconName.isEmpty() )
	{
	    if ( ! iconName.startsWith( "/" ) )
		iconName.prepend( ICON_DIR );

	    QPixmap icon( iconName );

	    if ( icon.isNull() )
		y2error( "Can't load icon %s", (const char *) iconName );

	    else
		setPixmap( 0, icon );
	}
    }
}


void
YQTreeItem::buildSubTree( YTreeItemList & items, int & nextSerialNo )
{
    for ( YTreeItemListIterator it = items.begin(); it < items.end(); ++it )
    {
	YQTreeItem * item = new YQTreeItem ( _tree,
					     this,
					     *it,
					     nextSerialNo++ );
	YTreeItemList itemList = ( *it)->itemList();

	if ( itemList.size() > 0 )
	{
	    item->buildSubTree ( itemList, nextSerialNo );
	}
    }
}


void
YQTreeItem::setOpen( bool open )
{
    QListViewItem::setOpen( open );
    _origItem->setOpen( open );
}



QString
YQTreeItem::key( int column, bool ascending ) const
{
    /*
     * Sorting key for QListView internal sorting:
     *
     * Always sort tree items by insertion order. The tree widget
     * cannot maintain a meaningful sorting order of its own: All it
     * could do is sort by names ( ASCII sort ). Better let the ( YCP )
     * application handle this.
     */

    QString strKey;
    strKey.sprintf( "%08d", _serialNo );

    return strKey;
}


#include "YQTree.moc"
