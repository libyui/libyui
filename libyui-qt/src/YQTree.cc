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
#include "YTreeItem.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"

#define VERBOSE_TREE_ITEMS	0


YQTree::YQTree( YWidget * parent, const string & label )
    : QVBox( (QWidget *) parent->widgetRep() )
    , YTree( parent, label )
{
    setWidgetRep( this );

    setSpacing( YQWidgetSpacing );
    setMargin ( YQWidgetMargin  );

    _nextSerialNo = 0;

    _caption     = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    
    _qt_listView = new QListView( this );
    YUI_CHECK_NEW( _qt_listView );
    
    _qt_listView->addColumn( "" );
    _qt_listView->header()->hide();
    _qt_listView->setRootIsDecorated ( true );

    _caption->setBuddy ( _qt_listView );

    connect( _qt_listView,	SIGNAL( selectionChanged ( QListViewItem *	) ),
	     this, 		SLOT  ( slotSelected     ( QListViewItem *	) ) );

    connect( _qt_listView,	SIGNAL( spacePressed	 ( QListViewItem *	) ),
	     this, 		SLOT  ( slotActivated	 ( QListViewItem *	) ) );

    connect( _qt_listView,	SIGNAL( doubleClicked	 ( QListViewItem * 	) ),
	     this, 		SLOT  ( slotActivated	 ( QListViewItem *	) ) );
}


YQTree::~YQTree()
{
    // NOP
}


void YQTree::setLabel( const string & label )
{
    _caption->setText( label );
    YTree::setLabel( label );
}


void YQTree::rebuildTree()
{
    // y2debug( "Rebuilding tree" );
    
    YQSignalBlocker sigBlocker( _qt_listView );
    _qt_listView->clear();

    buildDisplayTree( 0, itemsBegin(), itemsEnd() );
}


void YQTree::buildDisplayTree( YQTreeItem * parentItem, YItemIterator begin, YItemIterator end )
{
    for ( YItemIterator it = begin; it < end; ++it )
    {
	YTreeItem * orig = dynamic_cast<YTreeItem *> (*it);
	YUI_CHECK_PTR( orig );

	YQTreeItem * clone;

	if ( parentItem )
	    clone = new YQTreeItem( this, parentItem, orig, _nextSerialNo++ );
	else
	    clone = new YQTreeItem( this, _qt_listView, orig, _nextSerialNo++ );

	YUI_CHECK_NEW( clone );

	if ( orig->hasChildren() )
	    buildDisplayTree( clone, orig->childrenBegin(), orig->childrenEnd() );
    }
}


void YQTree::selectItem( YItem * yItem, bool selected )
{
    YQSignalBlocker sigBlocker( _qt_listView );

    YTreeItem * treeItem = dynamic_cast<YTreeItem *> (yItem);
    YUI_CHECK_PTR( treeItem );

    YQTreeItem * yqTreeItem = (YQTreeItem *) treeItem->data();
    YUI_CHECK_PTR( yqTreeItem );

    if ( ! selected && yqTreeItem == _qt_listView->selectedItem() )
    {
	deselectAllItems();
    }
    else
    {
	_qt_listView->setSelected( yqTreeItem, true );
	openBranch( yqTreeItem );
	YTree::selectItem( treeItem, selected );
    }
}


void YQTree::selectItem( YQTreeItem * item )
{
    if ( item )
    {
	YQSignalBlocker sigBlocker( _qt_listView );

	_qt_listView->setSelected( item, true );
	openBranch( item );
	YTree::selectItem( item->origItem(), true );
	
	y2debug( "selected item: \"%s\"", item->origItem()->label().c_str() );
    }
}


void YQTree::openBranch( QListViewItem * item )
{
    while ( item )
    {
	item->setOpen( true ); // Takes care of origItem()->setOpen()
	item = item->parent();
    }
}


void YQTree::deselectAllItems()
{
    YQSignalBlocker sigBlocker( _qt_listView );

    YTree::deselectAllItems();
    _qt_listView->clearSelection();
}


void YQTree::deleteAllItems()
{
    YQSignalBlocker sigBlocker( _qt_listView );

    _qt_listView->clear();
    YTree::deleteAllItems();
}


void YQTree::slotSelected( QListViewItem * qItem )
{
    selectItem( dynamic_cast<YQTreeItem *> (qItem) );

    if ( notify() && ! YQUI::ui()->eventPendingFor( this ) )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
}


void YQTree::slotActivated( QListViewItem * qItem )
{
    selectItem( dynamic_cast<YQTreeItem *> (qItem) );

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}


int YQTree::preferredWidth()
{
    int hintWidth = _caption->isShown() ? _caption->sizeHint().width() : 0;
    return max( 200, hintWidth );
}


int YQTree::preferredHeight()
{
    // 300 is an arbitrary value.  Use a MinSize or MinHeight widget to set a
    // size that is useful for the application.

    int hintHeight = _caption->isShown() ? _caption->sizeHint().height() : 0;

    return 300 + hintHeight;
}


void YQTree::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void YQTree::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_listView->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


bool YQTree::setKeyboardFocus()
{
    _qt_listView->setFocus();

    return true;
}


/*============================================================================*/



YQTreeItem::YQTreeItem( YQTree	*	tree,
			QListView *	listView,
			YTreeItem *	orig,
			int		serial )
    : QListViewItem( listView )
{
    init( tree, orig, serial );
    
#if VERBOSE_TREE_ITEMS
    y2debug( "Creating toplevel tree item \"%s\"", orig->label().c_str() );
#endif
}


YQTreeItem::YQTreeItem( YQTree	*	tree,
			YQTreeItem *	parentItem,
			YTreeItem *	orig,
			int		serial )
    : QListViewItem( parentItem )
{
    init( tree, orig, serial );

#if VERBOSE_TREE_ITEMS
    y2debug( "Creating tree item \"%s\" as child of \"%s\"",
	     orig->label().c_str(),
	     parentItem->origItem()->label().c_str() );
#endif
}


void YQTreeItem::init( YQTree *		tree,
		       YTreeItem *	orig,
		       int		serial )
{
    YUI_CHECK_PTR( tree );
    YUI_CHECK_PTR( orig );

    _tree	= tree;
    _serialNo	= serial;
    _origItem	= orig;

    _origItem->setData( this );

    setText( 0, fromUTF8 ( _origItem->label() ) );
    setOpen( _origItem->isOpen() );

    if ( _origItem->hasIconName() )
    {
	string iconName = _tree->iconFullPath( _origItem );
	QPixmap icon( iconName.c_str() );

	if ( icon.isNull() )
	    y2warning( "Can't load icon %s", iconName.c_str() );
	else
	    setPixmap( 0, icon );
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
     * could do is sort by names (ASCII sort). Better let the (YCP)
     * application handle this.
     */

    QString strKey;
    strKey.sprintf( "%08d", _serialNo );

    return strKey;
}


#include "YQTree.moc"
