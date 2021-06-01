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

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <algorithm>

#include <yui/YEvent.h>

#include "utf8.h"
#include "YQLog.h"
#include "YQSignalBlocker.h"
#include "YQUI.h"
#include "YQDumbTab.h"
#include "YQAlignment.h"

#include <QEvent>
#include <QStackedWidget>
#include <QTabBar>


#define YQDumbTabSpacing	2
#define YQDumbTabFrameMargin	2

using std::endl;


YQDumbTab::YQDumbTab( YWidget *	parent )
    : QTabWidget( (QWidget *) parent->widgetRep() )
    , YDumbTab( parent )
{
    setWidgetRep( this );

    // The QTabWidget parent class creates an internal QTabBar for the tabs and
    // a QStackedWidget for the (future) pages. By default, it does all the
    // page switching internally; but we will do it manually here, using the
    // QTabBar and the QStackedWidget directly.

    _tabBar = QTabWidget::tabBar();

    // Use Qt's introspection to find the internal QStackedWidget of our
    // QTabWidget parent class: Unlike the QTabBar, it's not otherwise
    // accessible.

    _stackedWidget = findChild<QStackedWidget *>();
    Q_CHECK_PTR( _stackedWidget );

    // Create a first page in the QStackedWidget as a holder for our (future)
    // single child, usually a YQReplacePoint.
    //
    // Notice that there is no corresponding tab in the QTabBar for this: The
    // tabs will be created later when items are added. The application will
    // take care of exchanging the page content (using the ReplacePoint()) as
    // needed; this widget will only send a YMenuEvent to notify the
    // application that the user clicked on a tab.

    _firstPage = new QWidget();
    _stackedWidget->addWidget( _firstPage );

#if 0
    _tabBar->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) ); // hor/vert
    _tabBar->setExpanding( false );
    setFocusProxy( _tabBar );
    setFocusPolicy( Qt::TabFocus );
#endif
    _tabBar = tabBar();

    connect( _tabBar, &pclass( _tabBar )::currentChanged,
	     this,    &pclass( this )::slotSelected );
}


YQDumbTab::~YQDumbTab()
{
    // NOP
}


void YQDumbTab::childEvent( QChildEvent * event )
{
    bool handled = false;

    // All YQ* widgets call the QWidget parent class constructor with
    // YWidget::parent()->widgetRep() as their QWidget parent, but this is
    // wrong here: This YQDumbTab's widgetRep() is the QTabWidget; but we need
    // to use the _firstPage placeholder instead.
    //
    // So we catch this on the Qt level when the child is added (more exactly:
    // polished) so we can reparent any child widget (typically a
    // YQReplacePoint) to _firstPage.

    if ( event && event->type() == QEvent::ChildPolished )
    {
        // Not using QEvent::ChildAdded here because the child might not yet be
        // completely constructed, so it might not yet be safe to call its
        // YWidget::widgetRep() method; but Qt will send the ChildPolished
        // event for the same one later.

        YWidget * ywidget = dynamic_cast<YWidget *>( event->child() );

        if ( ywidget )
        {
            yuiDebug() << "Reparenting " << ywidget << " to _firstPage" << endl;
            event->child()->setParent( _firstPage );
            handled = true;
        }
        else
        {
            yuiDebug() << "Ignoring new " << event->child()->metaObject()->className() << endl;
        }
    }

    if ( ! handled )
        QTabWidget::childEvent( event );
}


void
YQDumbTab::addItem( YItem * item )
{
    YQSignalBlocker sigBlocker( _tabBar );
    YDumbTab::addItem( item );

    _tabBar->insertTab( item->index(), fromUTF8( item->label() ) );
    yuiDebug() << "Adding tab page [" << item->label() << "]" << endl;

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
    yuiDebug() << "Tab [" << item->label() << "] selected" << endl;
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
    QTabWidget::setEnabled( enabled );
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
    QTabWidget::resize( newWidth, newHeight );

    int remainingHeight = newHeight;
    int remainingWidth  = newWidth;
    int x_offset	= 0;
    int y_offset	= 0;

#if 0
    //
    // _tabBar (fixed height)
    //

    int tabBarHeight = _tabBar->sizeHint().height();

    if ( remainingHeight < tabBarHeight )
	tabBarHeight = remainingHeight;

    _tabBar->resize( newWidth, tabBarHeight );
    remainingHeight -= tabBarHeight;
#endif

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

#if 0
	QWidget * qChild = (QWidget *) firstChild()->widgetRep();
	qChild->move( x_offset, y_offset );
#endif
    }
}


void
YQDumbTab::activate()
{
    // Send an activation event for this widget

    if ( notify() )
        YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::Activated ) );
}
