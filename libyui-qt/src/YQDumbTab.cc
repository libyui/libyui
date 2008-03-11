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

  File:	      YQDumbTab.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include "YUILog.h"
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
#include "YEvent.h"

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

    connect( _tabBar, SIGNAL( selected    ( int ) ),
	     this,    SLOT  ( slotSelected( int ) ) );
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



#include "YQDumbTab.moc"
