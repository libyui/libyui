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

  File:	      YQMainWinDock.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include <qwidgetstack.h>
#include <qtimer.h>

#include <YDialog.h>
#include <YQUI.h>
#include <YEvent.h>
#include "YQMainWinDock.h"



YQMainWinDock *
YQMainWinDock::mainWinDock()
{
    static YQMainWinDock * mainWinDock = 0;
    
    if ( ! mainWinDock )
    {
	mainWinDock = new YQMainWinDock();
	mainWinDock->show();
    }

    return mainWinDock;
}


YQMainWinDock::YQMainWinDock()
    : QVBox()
{
    _qWidgetStack = new QWidgetStack( this );
    
    resize( YQUI::ui()->defaultSize( YD_HORIZ ),
	    YQUI::ui()->defaultSize( YD_VERT  ) );
}


YQMainWinDock::~YQMainWinDock()
{
    // NOP
}


void
YQMainWinDock::show()
{
    y2debug( "Showing" );
    
    QWidget::show();
    
    if ( ! _widgetStack.empty() )
    {
	QWidget * dialog = _widgetStack.back();
	dialog->raise();
	
	if ( ! dialog->isShown() )
	    dialog->show();
    }
}


void
YQMainWinDock::add( QWidget * dialog )
{
    YUI_CHECK_PTR( dialog );

#if 0
    if ( dialog->parent() )
    {
	y2error( "Dialog already has a parent" );
	return;
    }
#endif

    if ( ! dialog->isShown() )
	dialog->show();
    
    _widgetStack.push_back( dialog );
    _qWidgetStack->addWidget( dialog );
    _qWidgetStack->raiseWidget( dialog );
    
    if ( ! isShown() )
	show();

#if 0
    QTimer::singleShot( 3*1000, this, SLOT( showCurrentDialog() ) );
#endif
}


void
YQMainWinDock::showCurrentDialog()
{
    if ( ! _widgetStack.empty() )
    {
	QWidget * dialog = _widgetStack.back();
	y2debug( "Showing dialog %p", dialog );
#if 0
	_qWidgetStack->raiseWidget( dialog );
#endif
	dialog->raise();
	update();
    }
}


void
YQMainWinDock::remove( QWidget * dialog )
{
    if ( _widgetStack.empty() )
	return;

    if ( ! dialog )
	dialog = _widgetStack.back();

    if ( dialog == _widgetStack.back() )
    {
	// The most common case:
	// The topmost dialog is to be removed

	_widgetStack.pop_back();
	_qWidgetStack->removeWidget( dialog );

	y2debug( "removing dialog %p", dialog );
    }
    else // The less common (but more generic) case: Remove any dialog
    {
	YQMainWinDock::YQWidgetStack::iterator pos = findInStack( dialog );

	if ( pos == _widgetStack.end() )
	    return;

	y2warning( "Found dialog somewhere in the middle of the widget stack" );

	_widgetStack.erase( pos );
	_qWidgetStack->removeWidget( dialog );

	y2debug( "removing dialog %p", dialog );
    }

    if ( _widgetStack.empty() )
	hide();
    else
    {
	_qWidgetStack->raiseWidget( _widgetStack.back() );
    }
}


YQMainWinDock::YQWidgetStack::iterator
YQMainWinDock::findInStack( QWidget * dialog )
{
    for ( YQMainWinDock::YQWidgetStack::iterator it = _widgetStack.begin();
	  it != _widgetStack.end();
	  ++it )
    {
	if ( *it == dialog )
	    return it;
    }

    return _widgetStack.end();
}


QWidget *
YQMainWinDock::topmostDialog() const
{
    if ( _widgetStack.empty() )
	return 0;
    else
	return _widgetStack.back();
}


bool
YQMainWinDock::couldDock()
{
    YDialog * topDialog = YDialog::topmostDialog( false ); // don't throw

    if ( ! topDialog )	// No dialog at all?
	return true;	// Can dock the next one without problems

    // The next dialog can be docked if there is no popup dialog currently open.
    // This is equivalent to the topmost dialog on the YDialog stack being the
    // same dialog as the topmost dialog of this MainWinDock.

    return topDialog->widgetRep() == this->topmostDialog();
}


void
YQMainWinDock::closeEvent( QCloseEvent * event )
{
    // The window manager "close window" button (and WM menu, e.g. Alt-F4) will be
    // handled just like the user had clicked on the `id`( `cancel ) button in
    // that dialog. It's up to the YCP application to handle this (if desired).

    y2debug( "Caught window manager close event - returning with YCancelEvent" );
    event->ignore();
    YQUI::ui()->sendEvent( new YCancelEvent() );
}


void
YQMainWinDock::paintEvent( QPaintEvent * event )
{
    // NOP
}


#include "YQMainWinDock.moc"
