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


#include <qstring.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qheader.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::min;
using std::max;

#include "YUIQt.h"
#include "utf8.h"
#include "layoututils.h"
#include "YQTree.h"


YQTree::YQTree ( YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt, const YCPString & label )
    : QWidget	( parent )
    , YTree	( opt, label )
    , yuiqt	( yuiqt	)
{
    setWidgetRep(this);

    nextSerialNo = 0;

    qt_label = new QLabel ( fromUTF8 ( label->value() ), this );
    qt_label->setTextFormat ( QLabel::PlainText );
    qt_label->setFont ( yuiqt->currentFont() );

    listView = new QListView (this );
    listView->setFont ( yuiqt->currentFont() );
    listView->addColumn( "" );
    listView->header()->hide();
    listView->setRootIsDecorated ( true );

    qt_label->setBuddy ( listView );
    oneUponTheOther ( this, qt_label, listView );

    connect( listView, SIGNAL( selectionChanged ( void )), this, SLOT( slotSelected( void )));
}


void YQTree::setLabel ( const YCPString &label )
{
    qt_label->setText ( fromUTF8 ( label->value() ) );
    YTree::setLabel ( label );
}


long YQTree::nicesize ( YUIDimension dim )
{
    if  ( dim == YD_HORIZ )  return max ( 200, qt_label->sizeHint().width() );
    else return 300 + qt_label->sizeHint().height();
}


void YQTree::setSize ( long newwidth, long newheight )
{
    y2debug("Resizing Tree to %ld,%ld", newwidth, newheight );
    qt_label->resize ( min ( newwidth,  (long) ( qt_label->sizeHint().width() ) ),
		       min ( newheight, (long) ( qt_label->sizeHint().height() )	 ) );
    listView->resize ( newwidth, newheight - qt_label->height() );
    resize ( newwidth, newheight );
}


void YQTree::setEnabling ( bool enabled )
{
    qt_label->setEnabled ( enabled );
    listView->setEnabled ( enabled );
#if 0
    listView->triggerUpdate ();
#endif
}


void
YQTree::rebuildTree()
{
    listView->clear();

    for ( YTreeItemListIterator it = items.begin(); it < items.end(); ++it )
    {
	YQTreeItem *item = new YQTreeItem ( this,
					    listView,
					    *it,
					    nextSerialNo++ );
	YTreeItemList itemList = (*it)->itemList();

	if ( itemList.size() > 0 )
	{
	    item->buildSubTree ( itemList, nextSerialNo );
	}
    }
}


const YTreeItem *
YQTree::getCurrentItem() const
{
    YQTreeItem *it = (YQTreeItem *) listView->selectedItem();

    return it ? it->getOrigItem() : 0;
}


void
YQTree::setCurrentItem ( YTreeItem * yit )
{
    YQTreeItem *it = findYQTreeItem( yit );

    if ( it )
    {
	// Select this item

	listView->setSelected( it, true);


	// Open all parent items so the selected item isn't obscured
	// somewhere deep within the tree

	while ( ( it = (YQTreeItem *) it->parent() ) )
	{
	    if ( ! it->isOpen() )
		it->setOpen( true );
	}

	listView->ensureItemVisible( it );
    }
    else
	listView->clearSelection();
}


void
YQTree::registerItem ( const YTreeItem *	orig,
		       const YQTreeItem *	clone )
{
    yTreeItemToYQTreeItem.insert( (void *) orig, clone );
}


YQTreeItem *
YQTree::findYQTreeItem ( const YTreeItem *	orig ) const
{
    return yTreeItemToYQTreeItem[ (void *) orig ];
}


// slots

void YQTree::slotSelected ( void )
{
    if (getNotify())
	yuiqt->returnNow(YUIInterpreter::ET_WIDGET, this);
}


bool
YQTree::setKeyboardFocus()
{
    listView->setFocus();

    return true;
}


/*============================================================================*/



YQTreeItem::YQTreeItem ( YQTree	*		tree,
			 QListView *		parent,
			 const YTreeItem *	yTreeItem,
			 int			serial )
    : QListViewItem	( parent )
{
    init ( tree, yTreeItem, serial );
}


YQTreeItem::YQTreeItem ( YQTree	*		tree,
			 YQTreeItem *		parent,
			 const YTreeItem *	yTreeItem,
			 int			serial )
    : QListViewItem	( parent )
{
    init ( tree, yTreeItem, serial );
}


void
YQTreeItem::init ( YQTree *		yqtree,
		   const YTreeItem *	yTreeItem,
		   int			serial )
{
    tree = yqtree;
    tree->registerItem ( yTreeItem, this );
    origItem = yTreeItem;
    serialNo = serial;
    setText ( 0, fromUTF8 ( origItem->getText()->value() ) );
    setOpen ( origItem->isOpenByDefault() );
}


void
YQTreeItem::buildSubTree ( YTreeItemList &items, int &nextSerialNo )
{
    for ( YTreeItemListIterator it = items.begin(); it < items.end(); ++it )
    {
	YQTreeItem *item = new YQTreeItem ( tree,
					    this,
					    *it,
					    nextSerialNo++ );
	YTreeItemList itemList = (*it)->itemList();

	if ( itemList.size() > 0 )
	{
	    item->buildSubTree ( itemList, nextSerialNo );
	}
    }
}


QString
YQTreeItem::key ( int column, bool ascending ) const
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
    strKey.sprintf( "%08d", serialNo );

    return strKey;
}


#include "YQTree.moc.cc"
