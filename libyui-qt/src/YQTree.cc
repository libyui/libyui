/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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

#include <QColorGroup>
#include <QHeaderView>
#include <QLabel>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QString>
#include <QPixmap>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

using std::min;
using std::max;

#include "YQUI.h"
#include "YEvent.h"
#include "utf8.h"
#include "YQTree.h"
#include "YTreeItem.h"
#include "YQSignalBlocker.h"
#include "YQWidgetCaption.h"
#include "YQApplication.h"


#define VERBOSE_TREE_ITEMS	0


YQTree::YQTree( YWidget * parent, const string & label )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YTree( parent, label )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin	);

    _nextSerialNo = 0;

    _caption	 = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_treeWidget = new QTreeWidget( this );
    YUI_CHECK_NEW( _qt_treeWidget );
    layout->addWidget( _qt_treeWidget );

     // _qt_treeWidget->setHeaderLabel("");
     // _qt_treeWidget->addColumn( "" );
     _qt_treeWidget->header()->hide();
     // _qt_treeWidget->setHeader(0L);
     _qt_treeWidget->setRootIsDecorated ( true );

	_qt_treeWidget->setContextMenuPolicy( Qt::CustomContextMenu );

    _caption->setBuddy ( _qt_treeWidget );

    connect( _qt_treeWidget,	SIGNAL( itemSelectionChanged () ),
	     this,		SLOT  ( slotSelectionChanged () ) );

    connect( _qt_treeWidget,	SIGNAL( itemDoubleClicked( QTreeWidgetItem *, int ) ),
	     this,		SLOT  ( slotActivated	 ( QTreeWidgetItem *	  ) ) );

    connect( _qt_treeWidget,	SIGNAL( itemExpanded	 ( QTreeWidgetItem * ) ),
	     this,		SLOT  ( slotItemExpanded ( QTreeWidgetItem * ) ) );

    connect( _qt_treeWidget,	SIGNAL( itemCollapsed	 ( QTreeWidgetItem * ) ),
	     this,		SLOT  ( slotItemCollapsed( QTreeWidgetItem * ) ) );

    connect( _qt_treeWidget,	SIGNAL( customContextMenuRequested ( const QPoint & ) ),
	     this,		SLOT  ( slotContextMenu ( const QPoint & ) ) );

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
    YQSignalBlocker sigBlocker( _qt_treeWidget );
    _qt_treeWidget->clear();

    buildDisplayTree( 0, itemsBegin(), itemsEnd() );
    _qt_treeWidget->resizeColumnToContents( 0 );
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
	    clone = new YQTreeItem( this, _qt_treeWidget, orig, _nextSerialNo++ );

	YUI_CHECK_NEW( clone );

	if ( orig->hasChildren() )
	    buildDisplayTree( clone, orig->childrenBegin(), orig->childrenEnd() );
    }
}


void YQTree::selectItem( YItem * yItem, bool selected )
{
    YQSignalBlocker sigBlocker( _qt_treeWidget );

    // yuiDebug() << "Selecting item \"" << yItem->label() << "\" " << boolalpha << selected << endl;
    YTreeItem * treeItem = dynamic_cast<YTreeItem *> (yItem);
    YUI_CHECK_PTR( treeItem );

    YQTreeItem * yqTreeItem = (YQTreeItem *) treeItem->data();
    YUI_CHECK_PTR( yqTreeItem );

    if ( selected )
    {
	selectItem( yqTreeItem );
    }
    else if ( yqTreeItem == _qt_treeWidget->currentItem() )
    {
	deselectAllItems();
    }
}


void YQTree::selectItem( YQTreeItem * item )
{
    if ( item )
    {
	YQSignalBlocker sigBlocker( _qt_treeWidget );

	_qt_treeWidget->setCurrentItem( item );
	item->setSelected( true );

	if ( item->parent() )
	    openBranch( (YQTreeItem *) item->parent() );

	YTree::selectItem( item->origItem(), true );

	// yuiDebug() << "selected item: \"" << item->origItem()->label() << "\"" << endl;
    }
}


void YQTree::openBranch( YQTreeItem * item )
{
    while ( item )
    {
	item->setOpen( true ); // Takes care of origItem()->setOpen()
	item = (YQTreeItem *) item->parent();
    }
}

void YQTree::slotItemExpanded( QTreeWidgetItem * qItem )
{
    YQTreeItem * item = dynamic_cast<YQTreeItem *> (qItem);

    if ( item )
	item->setOpen( true );

    _qt_treeWidget->resizeColumnToContents( 0 );
}


void YQTree::slotItemCollapsed( QTreeWidgetItem * qItem )
{
    YQTreeItem * item = dynamic_cast<YQTreeItem *> (qItem);

    if ( item )
	item->setOpen( false );

    _qt_treeWidget->resizeColumnToContents( 0 );
}


void YQTree::deselectAllItems()
{
    YQSignalBlocker sigBlocker( _qt_treeWidget );

    YTree::deselectAllItems();
    _qt_treeWidget->clearSelection();
}


void YQTree::deleteAllItems()
{
    YQSignalBlocker sigBlocker( _qt_treeWidget );

    _qt_treeWidget->clear();
    YTree::deleteAllItems();
}


void YQTree::slotSelectionChanged( )
{
    QList<QTreeWidgetItem *> items = _qt_treeWidget->selectedItems ();

    if ( ! items.empty() )
    {
	QTreeWidgetItem *qItem = items.first();
	selectItem( dynamic_cast<YQTreeItem *> (qItem) );
    }

    if ( notify() && ! YQUI::ui()->eventPendingFor( this ) )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
}


void YQTree::slotActivated( QTreeWidgetItem * qItem )
{
    selectItem( dynamic_cast<YQTreeItem *> (qItem) );

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}


int YQTree::preferredWidth()
{
    int hintWidth = !_caption->isHidden() ? _caption->sizeHint().width() : 0;
    return max( 80, hintWidth );
}


int YQTree::preferredHeight()
{
    int hintHeight = !_caption->isHidden() ? _caption->sizeHint().height() : 0;

    // 80 is an arbitrary value.  Use a MinSize or MinHeight widget to set a
    // size that is useful for the application.

    return 80 + hintHeight;
}


void YQTree::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


void YQTree::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_treeWidget->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


bool YQTree::setKeyboardFocus()
{
    _qt_treeWidget->setFocus();

    return true;
}


void YQTree::slotContextMenu ( const QPoint & pos )
{
    if  ( ! _qt_treeWidget ||  ! _qt_treeWidget->viewport() )
	return;

    YQUI::yqApp()->setContextMenuPos( _qt_treeWidget->viewport()->mapToGlobal( pos ) );
    if ( notifyContextMenu() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ContextMenuActivated ) );
}

/*============================================================================*/



YQTreeItem::YQTreeItem( YQTree	*	tree,
			QTreeWidget *	listView,
			YTreeItem *	orig,
			int		serial )
    : QTreeWidgetItem( listView )
{
    init( tree, orig, serial );

#if VERBOSE_TREE_ITEMS
    yuiDebug() << "Creating toplevel tree item \"" << orig->label() << "\"" << endl;
#endif
}


YQTreeItem::YQTreeItem( YQTree	*	tree,
			YQTreeItem *	parentItem,
			YTreeItem *	orig,
			int		serial )
    : QTreeWidgetItem( parentItem )
{
    init( tree, orig, serial );
#if VERBOSE_TREE_ITEMS
    yuiDebug() << "Creating tree item \"" << orig->label()
	       << "\" as child of \"" << parentItem->origItem()->label() << "\""
	       << endl;

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
	    yuiWarning() << "Can't load icon " << iconName << endl;
	else
	    setData( 0, Qt::DecorationRole, icon );
    }
}


void
YQTreeItem::setOpen( bool open )
{
    QTreeWidgetItem::setExpanded( open );
    _origItem->setOpen( open );
}



QString
YQTreeItem::key( int column, bool ascending ) const
{
    /*
     * Sorting key for QListView internal sorting:
     *
     * Always sort tree items by insertion order. The tree widget cannot
     * maintain a meaningful sorting order of its own: All it could do is sort
     * by names (ASCII sort). Better let the application handle this.
     */

    QString strKey;
    strKey.sprintf( "%08d", _serialNo );

    return strKey;
}


#include "YQTree.moc"
