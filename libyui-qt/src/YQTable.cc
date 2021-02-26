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

  File:	      YQTable.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#include <QHeaderView>
#include <QVBoxLayout>
#include <QString>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQSignalBlocker.h"
#include "YUIException.h"

#include "QY2ListView.h"
#include "YQTable.h"
#include "YQApplication.h"



YQTable::YQTable( YWidget * parent, YTableHeader * tableHeader, bool multiSelectionMode )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YTable( parent, tableHeader, multiSelectionMode )
{
    setWidgetRep( this );
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setSpacing( 0 );
    setLayout( layout );

    layout->setMargin( YQWidgetMargin );

    _qt_listView = new QY2ListView( this );
    YUI_CHECK_NEW( _qt_listView );
    layout->addWidget( _qt_listView );
    _qt_listView->setAllColumnsShowFocus( true );
    _qt_listView->header()->setStretchLastSection( false );
    _qt_listView->sortItems( 0, Qt::AscendingOrder);


    setKeepSorting(  keepSorting() );

    if ( multiSelectionMode )
	_qt_listView->setSelectionMode( QAbstractItemView::ExtendedSelection );

    _qt_listView->setContextMenuPolicy( Qt::CustomContextMenu );

    //
    // Add columns
    //

    QStringList headers;
    _qt_listView->setColumnCount( columns() );

    for ( int i=0; i < columns(); i++ )
    {
        headers << fromUTF8( header(i) );
    }

    _qt_listView->setHeaderLabels( headers );
    _qt_listView->header()->setResizeMode( QHeaderView::Interactive );

    
    //
    // Connect signals and slots
    //

    connect( _qt_listView, 	SIGNAL( itemDoubleClicked ( QTreeWidgetItem *, int ) ),
	     this, 		SLOT  ( slotActivated	  ( QTreeWidgetItem * ) ) );

    connect( _qt_listView, 	SIGNAL( currentItemChanged ( QTreeWidgetItem *, QTreeWidgetItem * ) ),
	     this, 		SLOT  ( slotSelected	   ( QTreeWidgetItem * ) ) );

    connect( _qt_listView,      SIGNAL( customContextMenuRequested ( const QPoint & ) ),
             this,      	SLOT  ( slotContextMenu ( const QPoint & ) ) );


    if ( multiSelectionMode )
    {
	// This is the exceptional case - avoid performance drop in the normal case
	
	connect( _qt_listView, 	SIGNAL( itemSelectionChanged() ),
		 this,		SLOT  ( slotSelectionChanged() ) );
					
    }
}


YQTable::~YQTable()
{
    // NOP
}


void
YQTable::setKeepSorting( bool keepSorting )
{
    YTable::setKeepSorting( keepSorting );
    _qt_listView->setSortByInsertionSequence( keepSorting );
    _qt_listView->setSortingEnabled( ! keepSorting );
}


void
YQTable::addItem( YItem * yitem )
{
    addItem( yitem,
	     false, // batchMode
	     true); // resizeColumnsToContent
}


void
YQTable::addItem( YItem * yitem, bool batchMode, bool resizeColumnsToContent )
{
    YTableItem * item = dynamic_cast<YTableItem *> (yitem);
    YUI_CHECK_PTR( item );

    YTable::addItem( item );

    YQTableListViewItem * clone = new YQTableListViewItem( this, _qt_listView, item );
    YUI_CHECK_NEW( clone );

    if ( ! batchMode && item->selected() )
    {
	// YTable enforces single selection, if appropriate
	
	YQSignalBlocker sigBlocker( _qt_listView );
	YQTable::selectItem( YSelectionWidget::selectedItem(), true );
    }


    //
    // Set column alignment
    //

    for ( int col=0; col < columns(); col++ )
    {
	switch ( alignment( col ) )
	{
	    case YAlignBegin:	clone->setTextAlignment( col, Qt::AlignLeft   | Qt::AlignVCenter );	break;
	    case YAlignCenter:	clone->setTextAlignment( col, Qt::AlignCenter | Qt::AlignVCenter );	break;
	    case YAlignEnd:	clone->setTextAlignment( col, Qt::AlignRight  | Qt::AlignVCenter );	break;

	    case YAlignUnchanged: break;
	}
    }

    if ( ! batchMode )
	_qt_listView->sortItems( 0, Qt::AscendingOrder);
    
    if ( resizeColumnsToContent )
    {
        for ( int i=0; i < columns(); i++ )
	    _qt_listView->resizeColumnToContents( i );
	/* NOTE: resizeColumnToContents(...) is performance-critical ! */
    }
}


void
YQTable::addItems( const YItemCollection & itemCollection )
{
    YQSignalBlocker sigBlocker( _qt_listView );

    for ( YItemConstIterator it = itemCollection.begin();
	  it != itemCollection.end();
	  ++it )
    {
	addItem( *it,
		 true,    // batchMode
		 false ); // resizeColumnsToContent
	/* NOTE: resizeToContents=true would cause a massive performance drop !
	         => resize columns to content only one time at the end of this 
	            function                                                 */
    }

    YItem * sel = YSelectionWidget::selectedItem();

    if ( sel )
	YQTable::selectItem( sel, true );
    
    for ( int i=0; i < columns(); i++ )
	_qt_listView->resizeColumnToContents( i );
}


void
YQTable::selectItem( YItem * yitem, bool selected )
{
    YQSignalBlocker sigBlocker( _qt_listView );

    YTableItem * item = dynamic_cast<YTableItem *> (yitem);
    YUI_CHECK_PTR( item );

    YQTableListViewItem * clone = (YQTableListViewItem *) item->data();
    YUI_CHECK_PTR( clone );


    if ( ! selected && clone == _qt_listView->currentItem() )
    {
	deselectAllItems();
    }
    else
    {
	if ( ! hasMultiSelection() )
	    _qt_listView->setCurrentItem( clone ); // This deselects all other items!
	
	clone->setSelected( true );
	YTable::selectItem( item, selected );
    }
}


void
YQTable::deselectAllItems()
{
    YQSignalBlocker sigBlocker( _qt_listView );

    YTable::deselectAllItems();
    _qt_listView->clearSelection();
}


void
YQTable::deleteAllItems()
{
    _qt_listView->clear();
    YTable::deleteAllItems();
}


void
YQTable::cellChanged( const YTableCell * cell )
{
    YTableItem * item = cell->parent();
    YUI_CHECK_PTR( item );

    YQTableListViewItem * clone = (YQTableListViewItem *) item->data();
    YUI_CHECK_PTR( clone );

    clone->updateCell( cell );
}


void
YQTable::selectOrigItem( QTreeWidgetItem * listViewItem )
{
    if ( listViewItem )
    {
	YQTableListViewItem * tableListViewItem = dynamic_cast<YQTableListViewItem *> (listViewItem);
	YUI_CHECK_PTR( tableListViewItem );

	YTable::selectItem( tableListViewItem->origItem(), true );
    }
}


void
YQTable::slotSelected( QTreeWidgetItem * listViewItem  )
{
    if ( listViewItem && _qt_listView->selectedItems().count() > 0 )
	selectOrigItem( listViewItem );
    else
    {
	// Qt might select nothing if a user clicks outside the items in the widget

	if ( hasItems() && YSelectionWidget::hasSelectedItem() )
	    YQTable::selectItem( YSelectionWidget::selectedItem(), true );
    }

    if ( immediateMode() )
    {
	if ( ! YQUI::ui()->eventPendingFor( this ) )
	{
	    // Avoid overwriting a (more important) Activated event with a SelectionChanged event

	    yuiDebug() << "Sending SelectionChanged event" << endl;
	    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
	}
    }
}


void
YQTable::slotSelectionChanged()
{
    YSelectionWidget::deselectAllItems();
    yuiDebug() << endl;

    QList<QTreeWidgetItem *> selItems = _qt_listView->selectedItems();

    for ( QList<QTreeWidgetItem *>::iterator it = selItems.begin();
	  it != selItems.end();
	  ++it )
    {
	YQTableListViewItem * tableListViewItem = dynamic_cast<YQTableListViewItem *> (*it);

	if ( tableListViewItem )
	{
	    tableListViewItem->origItem()->setSelected( true );

	    yuiDebug() << "Selected item: " << tableListViewItem->origItem()->label() << endl;
	}
    }

    if ( immediateMode() )
    {
	if ( ! YQUI::ui()->eventPendingFor( this ) )
	{
	    // Avoid overwriting a (more important) Activated event with a SelectionChanged event

	    yuiDebug() << "Sending SelectionChanged event" << endl;
	    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
	}
    }
}


void
YQTable::slotActivated( QTreeWidgetItem * listViewItem )
{
    selectOrigItem( listViewItem );

    if ( notify() )
    {
	yuiDebug() << "Sending Activated event" << endl;
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
    }
}


void
YQTable::setEnabled( bool enabled )
{
    _qt_listView->setEnabled( enabled );
    //FIXME _qt_listView->triggerUpdate();
    YWidget::setEnabled( enabled );
}



int
YQTable::preferredWidth()
{
    // Arbitrary value.
    // Use a MinSize widget to set a size that is useful for the application.

    return 30;
}


int
YQTable::preferredHeight()
{
    // Arbitrary value.
    // Use a MinSize widget to set a size that is useful for the application.

    return 30;
}


void
YQTable::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool
YQTable::setKeyboardFocus()
{
    _qt_listView->setFocus();

    return true;
}


void
YQTable::slotContextMenu ( const QPoint & pos )
{
    if  ( ! _qt_listView ||  ! _qt_listView->viewport() )
	return;

    YQUI::yqApp()->setContextMenuPos( _qt_listView->viewport()->mapToGlobal( pos ) );
    if ( notifyContextMenu() )
        YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ContextMenuActivated ) );
}





YQTableListViewItem::YQTableListViewItem( YQTable *	table,
					  QY2ListView * parent,
					  YTableItem *	origItem )
    : QY2ListViewItem( parent )
    , _table( table )
    , _origItem( origItem )
{
    YUI_CHECK_PTR( _table );
    YUI_CHECK_PTR( _origItem );

    _origItem->setData( this );

    for ( YTableCellIterator it = _origItem->cellsBegin();
	  it != _origItem->cellsEnd();
	  ++it )
    {
	updateCell( *it );
    }
}


void
YQTableListViewItem::updateCell( const YTableCell * cell )
{
    if ( ! cell )
	return;

    int column = cell->column();

    //
    // Set label text
    //

    setText( column, fromUTF8( cell->label() ) );


    //
    // Set icon (if specified)
    //

    if ( cell->hasIconName() )
    {
	// _table is checked against 0 in the constructor

	string	iconName = _table->iconFullPath( cell->iconName() );
	QPixmap	icon	 = QPixmap( iconName.c_str() );

	if ( icon.isNull() )
	    yuiWarning() << "Can't load icon " << iconName << endl;
	else
	    setData( column, Qt::DecorationRole, icon );
    }
    else // No pixmap name
    {
	if ( ! data( column, Qt::DecorationRole ).isNull() ) // Was there a pixmap before?
	{
	    setData( column, Qt::DecorationRole, QPixmap() ); // Set empty pixmap
	}
    }
}


#include "YQTable.moc"
