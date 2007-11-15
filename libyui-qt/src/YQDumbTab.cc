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


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include <qtabbar.h>
#include <algorithm>

#include "QY2LayoutUtils.h"
#include "utf8.h"
#include "YQUI.h"
#include "YQDumbTab.h"
#include "YQAlignment.h"
#include "YEvent.h"

#define YQDumbTabSpacing	4


YQDumbTab::YQDumbTab( YWidget *	parent )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YDumbTab( parent )
{
    setWidgetRep( this );
    addVSpacing( this, YQDumbTabSpacing );

    //
    // Tab bar
    //

    _tabBar = new QTabBar( this );
    CHECK_PTR( _tabBar );

    _tabBar->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) ); // hor/vert
    setFocusProxy( _tabBar );
    setFocusPolicy( TabFocus );

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
    YDumbTab::addItem( item );
    
    QTab * tab = new QTab( fromUTF8( item->label() ) );
    YUI_CHECK_NEW( tab );
    y2debug( "Adding tab page [%s]", item->label().c_str() );

    tab->setIdentifier( item->index() );
    _tabBar->addTab( tab );
    item->setData( tab );
}


void
YQDumbTab::selectItem( YItem * item, bool selected )
{
    if ( selected )
    {
	// Don't try to suppress any signals sent here with a YQSignalBlocker,
	// otherwise the application code that handles the event will never be executed.
	
	QTab * tab = (QTab *) item->data();

	if ( tab )
	    _tabBar->setCurrentTab( tab );
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
	QTab * tab = (QTab *) (*it)->data();

	if ( tab )
	    _tabBar->removeTab( tab );
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
    
    return max( tabBarWidth, childWidth );
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
	
	if ( remainingHeight >= YQDumbTabSpacing )
	    remainingHeight -= YQDumbTabSpacing;
	else
	    remainingHeight = 0;

	//
	// Client area
	//
	
	firstChild()->setSize( newWidth, remainingHeight );

	QWidget * qChild = (QWidget *) firstChild()->widgetRep();
	qChild->move( 0, newHeight - remainingHeight );
    }
}



#include "YQDumbTab.moc"
