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


YQDumbTab::YQDumbTab( QWidget *			parent,
		      const YWidgetOpt & 	opt )
    : QVBox( parent )
    , YDumbTab( opt )
{
    setWidgetRep( this );
    setFont( YQUI::ui()->currentFont() );
    addVSpacing( this, 4 );
    _doingResize = false;

    //
    // Tab bar
    //

    _tabBar = new QTabBar( this );
    CHECK_PTR( _tabBar );

    _tabBar->setFont( YQUI::ui()->currentFont() );
    _tabBar->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert
    setFocusProxy( _tabBar );
    setFocusPolicy( TabFocus );

    connect( _tabBar, SIGNAL( selected( int ) ),
	     this,    SLOT  ( sendTabSelectedEvent( int ) ) );


    //
    // Client area for tab page contents
    //

    _clientArea = new QVBox( this );

    CHECK_PTR( _clientArea );

#if 0
    _clientArea->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
    _clientArea->setLineWidth( 1 );
    _clientArea->setMidLineWidth( 1 );
#endif
    _clientArea->setFrameStyle( QFrame::TabWidgetPanel | QFrame::Raised );
    // _clientArea->setFrameStyle( QFrame::TabWidgetPanel | QFrame::Sunken );
    _clientArea->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert


    //
    // HVCenter for contents
    //

    YWidgetOpt widgetOpt;
    _contents = new YQAlignment( _clientArea, widgetOpt, YAlignUnchanged, YAlignUnchanged);
    CHECK_PTR( _contents );

    addChild( _contents );

    _contents->setParent( this );
    _contents->installEventFilter( this );
}


void YQDumbTab::setEnabling( bool enabled )
{
    QVBox::setEnabled( enabled );
}


long YQDumbTab::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )
    {
	return max( (long) sizeHint().width(), _contents->nicesize( YD_HORIZ ) );
    }
    else
    {
	return sizeHint().height() + _contents->nicesize( YD_VERT );
    }
}


void YQDumbTab::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
    resizeClientArea();
}


bool YQDumbTab::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::Resize && obj == _contents && ! _doingResize )
    {
	// Propagate resize event to YWidget derived children
	resizeClientArea();
	return true;		// Event handled
    }

    return QWidget::eventFilter( obj, ev );
}


void YQDumbTab::resizeClientArea()
{
    // Prevent endless recursion if this triggers another resize event
    _doingResize = true;
    
    if ( _contents )
    {
	// Propagate resize event to YWidget derived children
	QRect contentsRect = _clientArea->contentsRect();
	_contents->setSize( contentsRect.width(), contentsRect.height() );
    }
    
    _doingResize = false;
}



void YQDumbTab::addChild( YWidget * child )
{
    if ( child == _contents )
    {
	YContainerWidget::addChild( child );
    }
    else if ( _contents->numChildren() < 1 )
    {
	//
	// Reparent any other child: Make it a child of _contents (YAlignment)
	//

	// y2debug( "Reparenting %s %s", child->widgetClass(), child->debugLabel().c_str() );

	// Reparent for Qt

	QWidget * qChild = (QWidget *) child->widgetRep();
	qChild->reparent( (QWidget *) _contents->widgetRep(), QPoint( 0, 0 ) );

	// Reparent for libyui

	child->setParent( _contents );
	_contents->addChild( child );
    }
    else
    {
	y2error( "Ignoring unwanted %s %s", child->widgetClass(), child->debugLabel().c_str() );
    }
}


void YQDumbTab::addTab( const YCPString & label )
{
    QTab * tab = new QTab( fromUTF8( label->value() ) );
    CHECK_PTR( tab );

    tab->setIdentifier( _tabBar->count() );
    _tabBar->addTab( tab );
}



int YQDumbTab::getSelectedTabIndex()
{
    return _tabBar->currentTab();
}



void YQDumbTab::setSelectedTab( int index )
{
    _tabBar->setCurrentTab( index );
}


void YQDumbTab::sendTabSelectedEvent( int index )
{
    if ( index >= 0 && (unsigned) index < _tabs.size() )
    {
	YCPValue id = _tabs[ index ].id();

	y2debug( "Switching to tab \"%s\" (ID %s)",
		 _tabs[ index ].label()->value().c_str(),
		 id->toString().c_str() );

	YQUI::ui()->sendEvent( new YMenuEvent( id ) );
    }
    else
    {
	y2error( "Tab index %d out of range (0..%u)", index, _tabs.size() );
    }
}



#include "YQDumbTab.moc"
