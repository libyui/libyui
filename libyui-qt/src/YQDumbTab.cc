/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:	      YQDumbTab.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>
#include <qtabbar.h>
#include <qevent.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <algorithm>

#include "YQSignalBlocker.h"
#include "utf8.h"
#include "YQUI.h"
#include "YQDumbTab.h"
#include "YQAlignment.h"
#include <yui/YEvent.h>

#define YQDumbTabSpacing	2
#define YQDumbTabFrameMargin	2


YQDumbTab::YQDumbTab( YWidget *	parent )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YDumbTab( parent )
{
    setWidgetRep( this );

    //
    // Tab bar
    //

    _tabBar = new QTabBar( this );
    Q_CHECK_PTR( _tabBar );

    _tabBar->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) ); // hor/vert
    setFocusProxy( _tabBar );
    setFocusPolicy( Qt::TabFocus );

    connect( _tabBar, &pclass(_tabBar)::currentChanged,
	     this,    &pclass(this)::slotSelected );
}


YQDumbTab::~YQDumbTab()
{
    // NOP
}


void
YQDumbTab::addItem( YItem * item )
{
    YQSignalBlocker sigBlocker( _tabBar );
    YDumbTab::addItem( item );

    _tabBar->insertTab( item->index(), fromUTF8( item->label() ) );
    yuiDebug() << "Adding tab page [" << item->label() << "]" << std::endl;

    if ( item->selected() )
	_tabBar->setCurrentIndex( item->index() );
}


void
YQDumbTab::selectItem( YItem * item, bool selected )
{
    if ( selected )
    {
	// Don't try to suppress any signals sent here with a YQSignalBlocker,
	// otherwise the application code that handles the event will never be executed.

        _tabBar->setCurrentIndex( item->index() );
    }

    YDumbTab::selectItem( item, selected );
}


void
YQDumbTab::deleteAllItems()
{
    for ( YItemConstIterator it = itemsBegin();
	  it != itemsEnd();
	  ++it )
    {
        _tabBar->removeTab( ( *it )->index() );
    }

    YDumbTab::deleteAllItems();
}


void
YQDumbTab::deselectAllItems()
{
    YDumbTab::deselectAllItems();
}


void
YQDumbTab::slotSelected( int index )
{
    YItem * item = itemAt( index );
    YUI_CHECK_PTR( item );
    yuiDebug() << "Tab [" << item->label() << "] selected" << std::endl;
    YDumbTab::selectItem( item );

    YQUI::ui()->sendEvent( new YMenuEvent( item ) );
}


void
YQDumbTab::shortcutChanged()
{
    // Any of the items might have its keyboard shortcut changed, but we don't
    // know which one. So let's simply set all tab labels again.

    for ( YItemConstIterator it = itemsBegin();
	  it != itemsEnd();
	  ++it )
    {
	YItem * item = *it;
	_tabBar->setTabText( item->index(), fromUTF8( item->label() ) );
    }
}


void
YQDumbTab::setEnabled( bool enabled )
{
    _tabBar->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int
YQDumbTab::preferredWidth()
{
    int tabBarWidth = _tabBar->sizeHint().width();
    int childWidth  = hasChildren() ? firstChild()->preferredWidth() : 0;

    return std::max( tabBarWidth, childWidth );
}


int
YQDumbTab::preferredHeight()
{
    int tabBarHeight = _tabBar->sizeHint().height();
    int childHeight  = hasChildren() ? firstChild()->preferredHeight() : 0;

    return tabBarHeight + YQDumbTabSpacing + childHeight;
}


void
YQDumbTab::setSize( int newWidth, int newHeight )
{
    QWidget::resize( newWidth, newHeight );
    int remainingHeight = newHeight;
    int remainingWidth  = newWidth;
    int x_offset	= 0;
    int y_offset	= 0;

    //
    // _tabBar (fixed height)
    //

    int tabBarHeight = _tabBar->sizeHint().height();

    if ( remainingHeight < tabBarHeight )
	tabBarHeight = remainingHeight;

    _tabBar->resize( newWidth, tabBarHeight );
    remainingHeight -= tabBarHeight;

    if ( hasChildren() )
    {
	//
	// Spacing between tabBar and client area
	//

	remainingHeight -= YQDumbTabSpacing;
	y_offset = newHeight - remainingHeight;

	//
	// 3D border
	//

	remainingHeight -= 2 * YQDumbTabFrameMargin;
	remainingWidth  -= 2 * YQDumbTabFrameMargin;
	x_offset += YQDumbTabFrameMargin;
	y_offset += YQDumbTabFrameMargin;

	if ( remainingHeight < 0 )
	    remainingHeight = 0;

	if ( remainingWidth < 0 )
	    remainingWidth = 0;

	//
	// Client area
	//


	firstChild()->setSize( remainingWidth, remainingHeight );

	QWidget * qChild = (QWidget *) firstChild()->widgetRep();
	qChild->move( x_offset, y_offset );
    }
}




