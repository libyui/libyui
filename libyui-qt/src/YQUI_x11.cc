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

  File:	      	YUIQt_x11.cc

  Author:     	Stefan Hundhammer <sh@suse.de>
  Maintainer: 	Stefan Hundhammer <sh@suse.de>

/-*/

#define USE_QT_CURSORS		1
#define FORCE_UNICODE_FONT	0

#include <qcursor.h>
#include <qwidgetstack.h>
#include <qvbox.h>
#include <qwidgetlist.h>
#include <qtextcodec.h>

#include <X11/Xlib.h>

#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YQUI.h"
#include "YEvent.h"
#include "YQDialog.h"


int YQUI::getDisplayWidth()
{
    return desktop()->width();
}


int YQUI::getDisplayHeight()
{
    return desktop()->height();
}


int YQUI::getDisplayDepth()
{
    return QColor::numBitPlanes();
}


long YQUI::getDisplayColors()
{
    return 1L << QColor::numBitPlanes();
}


int YQUI::getDefaultWidth()
{
    return _default_size.width();
}


int YQUI::getDefaultHeight()
{
    return _default_size.height();
}


long YQUI::defaultSize(YUIDimension dim) const
{
    if ( haveWM() )
	return dim == YD_HORIZ ? _default_size.width() : _default_size.height();
    else
	return dim == YD_HORIZ ? desktop()->width() : desktop()->height();
}


void
YQUI::busyCursor( void )
{
#if USE_QT_CURSORS

    setOverrideCursor( waitCursor );

#else
    /**
     * There were versions of Qt where simply using
     * QApplication::setOverrideCursor( waitCursor ) didn't work any more:
     * We _need_ the WType_Modal flag for non-defaultsize dialogs (i.e. for
     * popups), but Qt unfortunately didn't let such dialogs have a clock
     * cursor.  :-(
     *
     * They might have their good reasons for this (whatever they are), so
     * let's simply make our own busy cursors and set them to all widgets
     * created thus far.
     **/

    QWidgetList * widget_list = allWidgets();
    QWidgetListIt it( *widget_list );

    while ( *it )
    {
	if ( ! (*it)->testWFlags( WType_Desktop ) )	// except desktop (root window)
	{
	    XDefineCursor( (*it)->x11Display(), (*it)->winId(), _busy_cursor->handle() );
	}
	++it;
    }

    if ( widget_list )
	delete widget_list;
#endif
}


void
YQUI::normalCursor( void )
{
#if USE_QT_CURSORS

    if ( _busy_cursor_timer.isActive() )
	_busy_cursor_timer.stop();

    while ( overrideCursor() )
	restoreOverrideCursor();
#else
    /**
     * Restore the normal cursor for all widgets (undo busyCursor() ).
     *
     * Fortunately enough, Qt widgets keep track of their normal cursor
     * (QWidget::cursor() ) so this can easily be restored - it's not always the
     * arrow cursor - e.g., input fields (QLineEdit) have the "I-beam" cursor.
     **/

    QWidgetList * widget_list = allWidgets();
    QWidgetListIt it( *widget_list );

    while ( *it )
    {
	if ( ! (*it)->testWFlags( WType_Desktop ) )	// except desktop (root window)
	{
	    XDefineCursor( (*it)->x11Display(), (*it)->winId(), (*it)->cursor().handle() );
	}
	++it;
    }

    if ( widget_list )
	delete widget_list;
#endif
}


const QFont &YQUI::currentFont()
{
    /**
     * Brute force approach to make sure we'll really get a complete Unicode font:
     * Explicitly load the one font that we made sure to contain all required
     * characters, including Latin1, Latin2, Japanese, Korean, and the
     * characters used for glyphs.
     *
     * There are many fonts that claim to be Unicode, but most of them contain
     * just a sorry excuse for a complete Unicode character set. Qt can't know
     * how complete a font is, so it chooses one that might be better in other
     * aspects, but lacks necessary characters.
     **/

    if ( ! _loaded_current_font )
    {
#if FORCE_UNICODE_FONT
	_current_font = QFont( "Helvetica", 12 );
	_current_font.setStyleHint( QFont::SansSerif, QFont::PreferBitmap );
	_current_font.setRawName( "-gnu-unifont-medium-r-normal--16-160-75-75-p-80-iso10646-1" );
	y2debug( "Loading default font: %s", (const char *) _current_font.rawName() );
#else
	_current_font = qApp->font();
#endif
	_loaded_current_font = true;
    }

    return _current_font;
}


const QFont &YQUI::headingFont()
{
    /**
     * Brute force load the heading font - see currentFont() above for more.
     **/

    if ( ! _loaded_heading_font )
    {
#if FORCE_UNICODE_FONT
	_heading_font = QFont( "Helvetica", 14, QFont::Bold );
	_heading_font.setStyleHint( QFont::SansSerif, QFont::PreferBitmap );
	_heading_font.setRawName( "-gnu-unifont-bold-r-normal--18-180-75-75-p-80-iso10646-1" );
	y2debug( "Loading heading font: %s", (const char *) _heading_font.rawName() );
#else
	_heading_font = QFont( "Helvetica", 14, QFont::Bold );
#endif
	_loaded_heading_font = true;
    }

    return _heading_font;
}



bool YQUI::close()
{
    sendEvent( new YCancelEvent() );
    return true;
}


bool YQUI::eventFilter( QObject * obj, QEvent * ev )
{

    if ( ev->type() == QEvent::Close )
    {
	emit wmClose();

	if ( ! _wm_close_blocked )
	{
	    // Don't simply close the application window, return from UserInput()
	    // with `id(`cancel) and let the YCP application decide how to handle
	    // that (e.g., ask for confirmation).

	    y2debug( "Caught window close event - returning with `cancel" );
	    sendEvent( new YCancelEvent() );
	}

	return true;	// Event processed
    }
    else if ( ev->type() == QEvent::Show )
    {
	if ( obj == _main_win )
	{
	    if ( _main_dialog_id > 0 )
	    {
		// Work around QWidgetStack bug: The last raiseWidget() call
		// (from closeDialog() ) might have failed if the widget was
		// invisible at that time, e.g., because the user had switched to
		// some other virtual desktop (bugzilla bug #11310)

		_widget_stack->raiseWidget( _main_dialog_id );
	    }
	}
	else
	{
	    return showEventFilter( obj, ev );
	}
    }

    return false;	// Don't stop event processing
}


bool YQUI::showEventFilter( QObject * obj, QEvent * ev )
{
    if ( ! haveWM() )
    {
	// Make sure newly opened windows get the keyboard focus even without a
	// window manager. Otherwise the app might be unusable without a mouse.

	QWidget * widget = dynamic_cast<QWidget *> (obj);

	if ( widget )
	    widget->setActiveWindow();
    }

    return false;	// Don't stop event processing
}


void YQUI::loadPredefinedQtTranslations()
{
    QString language = QTextCodec::locale();
    QString path = QT_LOCALEDIR;

    if ( path.isEmpty() )
    {
	y2warning( "Qt locale directory not set - "
		   "no translations for predefined Qt dialogs" );
	return;
    }

    _qtTranslations.load( QString( "qt_" ) + language, path );

    if ( _qtTranslations.isEmpty() )
    {
	y2warning( "Loading translations for predefined Qt dialogs failed" );
	return;
    }

    qApp->installTranslator( & _qtTranslations );
}


// EOF
