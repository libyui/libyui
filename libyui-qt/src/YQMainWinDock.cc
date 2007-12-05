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
	mainWinDock = new YQMainWinDock();

    return mainWinDock;
}


YQMainWinDock::YQMainWinDock()
    : QWidget( 0, 0,		 // parent, name
	       YQUI::ui()->noBorder() ?
	       Qt::WStyle_Customize | Qt::WStyle_NoBorder :
	       Qt::WType_TopLevel ) 
{
    setCaption( "YaST2" );

    setFocusPolicy( QWidget::StrongFocus );

    resize( YQUI::ui()->defaultSize( YD_HORIZ ),
	    YQUI::ui()->defaultSize( YD_VERT  ) );

    y2debug( "MainWinDock initial size: %d x %d",
	     size().width(), size().height() );
}


YQMainWinDock::~YQMainWinDock()
{
    // NOP
}


void
YQMainWinDock::childEvent( QChildEvent * event )
{
    if ( event )
    {
	QWidget * widget = dynamic_cast<QWidget *> ( event->child() );

	if ( widget && event->inserted() )
	{
	    add( widget );
	}
    }
    
    QWidget::childEvent( event );
}


void
YQMainWinDock::resizeEvent( QResizeEvent * event )
{
    if ( event )
    {
	resize( event->size() );
	resizeVisibleChild();
    }
}


void
YQMainWinDock::resizeVisibleChild()
{
    if ( ! _widgetStack.empty() )
    {
	QWidget * dialog = _widgetStack.back();

	if ( dialog->size() != size() )
	{
	    // y2debug( "Resizing child dialog %p to %d x %d", dialog, size().width(), size().height() );
	    dialog->resize( size() );
	}
    }
}


void
YQMainWinDock::show()
{
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

    if ( ! dialog->isShown() )
	dialog->show();

    y2debug( "Adding dialog %p to mainWinDock", dialog );
    _widgetStack.push_back( dialog );
    resizeVisibleChild();
    
    if ( ! isShown() )
	show();
}


void
YQMainWinDock::showCurrentDialog()
{
    if ( ! _widgetStack.empty() )
    {
	QWidget * dialog = _widgetStack.back();
	y2debug( "Showing dialog %p", dialog );
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

	y2debug( "Removing dialog %p from mainWinDock", dialog );
    }
    else // The less common (but more generic) case: Remove any dialog
    {
	YQMainWinDock::YQWidgetStack::iterator pos = findInStack( dialog );

	if ( pos == _widgetStack.end() )
	    return;

	y2warning( "Found dialog somewhere in the middle of the widget stack" );
	y2debug( "Removing dialog %p from mainWinDock", dialog );
	
	_widgetStack.erase( pos );
    }

    if ( _widgetStack.empty() )		// No more main dialog?
	hide();				// -> hide dock
    else
    {
	dialog = _widgetStack.back();	// Get the next dialog from the stack
	dialog->raise();		// and raise it
	resizeVisibleChild();
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

    y2milestone( "Caught window manager close event - returning with YCancelEvent" );
    event->ignore();
    YQUI::ui()->sendEvent( new YCancelEvent() );
}


void
YQMainWinDock::paintEvent( QPaintEvent * event )
{
    // NOP
}


#include "YQMainWinDock.moc"
