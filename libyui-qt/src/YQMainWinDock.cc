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

  File:	      YQMainWinDock.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>
#include <QResizeEvent>
#include "YQDialog.h"
#include <YQUI.h>
#include <yui/YEvent.h>
#include "YQWizard.h"
#include "YQMainWinDock.h"

#define VERBOSE_RESIZE 1


YQMainWinDock *
YQMainWinDock::mainWinDock()
{
    static YQMainWinDock * mainWinDock = 0;

    if ( ! mainWinDock )
	mainWinDock = new YQMainWinDock();

    return mainWinDock;
}


YQMainWinDock::YQMainWinDock()
    : QWidget( 0, // parent, name
	       YQUI::ui()->noBorder() ?
	       Qt::FramelessWindowHint :
	       Qt::Window ),
      _sideBarWidth( 0 )
{
    setFocusPolicy( Qt::StrongFocus );

    resize( YQUI::ui()->defaultSize( YD_HORIZ ),
	    YQUI::ui()->defaultSize( YD_VERT  ) );

    yuiDebug() << "MainWinDock initial size: "
	       << size().width() << " x " << size().height()
	       << std::endl;
}


YQMainWinDock::~YQMainWinDock()
{
    // NOP
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
    for ( YQWidgetStack::reverse_iterator it = _widgetStack.rbegin(); it != _widgetStack.rend(); it++ )
    {
	YQDialog * dialog = *it;

        QRect rect = QRect( QPoint( 0, 0 ), size() );

        YQWizard * wizard = dialog->findWizard();

        if ( wizard )
	    yuiDebug() << dialog << " with " << wizard << " isSecondary: " << std::boolalpha << wizard->isSecondary() << std::endl;

        if ( wizard && wizard->isSecondary() )
	{
	    if ( QApplication::isLeftToRight() )
		rect.setLeft( 0 );
	    else
		rect.setWidth( rect.width() );
	}

	if ( dialog->rect() != rect )
	{
#if VERBOSE_RESIZE
	    yuiDebug() << "Resizing child dialog " << std::hex << ( (void *) dialog ) << std::dec
		       << " to " << rect.width() << " x " << rect.height()
		       << std::endl;
#endif
            dialog->setGeometry( rect );
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
        dialog->show();
    }
}


void
YQMainWinDock::add( YQDialog * dialog )
{
    YUI_CHECK_PTR( dialog );

    // Deactivate the next-lower dialog
    // (the one that currently still is the topmost on the _widgetStack)
    activateCurrentDialog( false );

    dialog->raise();
    dialog->show();

    yuiDebug() << "Adding dialog " << std::hex << (void *) dialog << std::dec
	       << "  to mainWinDock"
	       << std::endl;

    _widgetStack.push_back( dialog );
    resizeVisibleChild();

    show();
}


void
YQMainWinDock::activateCurrentDialog( bool active )
{
    if ( _widgetStack.empty() )
	return;

    // In the normal case, the (still or again) topmost dialog needs to be
    // activated or deactivated directly. Since this is done on the QWidget
    // level, its widgetRep() is needed -- which may or may not be the same as
    // the YQDialog.

    YQDialog * dialog = _widgetStack.back();
    QWidget  * widget = (QWidget *) dialog->widgetRep();


    // But then, there is also the exceptional case that this dialog contains a
    // wizard with a steps panel. In that case, the steps panel should remain
    // untouched; only the right side (the work area) of that wizard is to be
    // activated or deactivated.

    // probably no longer needed, now the windows (even with steps) fully overlap ??
    /*YQWizard * wizard = dialog->findWizard();

    if ( wizard && wizard->wizardMode() == YWizardMode_Steps )
    {
	QWidget * wizardWorkArea = wizard->workArea();

	if ( wizardWorkArea )
	    widget = wizardWorkArea;
	// else -> stick with dialog->widgetRep()
    }*/

    if ( widget )
	widget->setEnabled( active );
}


void
YQMainWinDock::showCurrentDialog()
{
    if ( ! _widgetStack.empty() )
    {
	QWidget * dialog = _widgetStack.back();
	yuiDebug() << "Showing dialog " << std::hex << (void *) dialog << std::dec << std::endl;
	dialog->raise();
	update();
    }
}


void
YQMainWinDock::remove( YQDialog * dialog )
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

	yuiDebug() << "Removing dialog " << std::hex << (void *) dialog << std::dec
		   <<" from mainWinDock"
		   << std::endl;
    }
    else // The less common (but more generic) case: Remove any dialog
    {
	YQMainWinDock::YQWidgetStack::iterator pos = findInStack( dialog );

	if ( pos == _widgetStack.end() )
	    return;

	yuiWarning() << "Found dialog somewhere in the middle of the widget stack" << std::endl;
	yuiDebug() << "Removing dialog " << std::hex << (void *) dialog << std::dec
		   << " from mainWinDock"
		   << std::endl;

	_widgetStack.erase( pos );
    }

    if ( _widgetStack.empty() )		// No more main dialog?
	hide();				// -> hide dock
    else
    {
	dialog = _widgetStack.back();	// Get the next dialog from the stack
	dialog->raise();		// and raise it
	activateCurrentDialog( true );
        dialog->show();
	resizeVisibleChild();
    }
}


YQMainWinDock::YQWidgetStack::iterator
YQMainWinDock::findInStack( YQDialog * dialog )
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


YQDialog *
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

    yuiMilestone() << "Caught window manager close event - returning with YCancelEvent" << std::endl;
    event->ignore();
    YQUI::ui()->sendEvent( new YCancelEvent() );
}


void
YQMainWinDock::paintEvent( QPaintEvent * event )
{
    // NOP
}


void
YQMainWinDock::setSideBarWidth( int width )
{
    if ( _sideBarWidth == width )
        return;

    _sideBarWidth = width;
    resizeVisibleChild();
}


#include "YQMainWinDock.moc"
