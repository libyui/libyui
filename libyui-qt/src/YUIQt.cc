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

  File:	      	YUIQt.cc

  Authors:	Mathias Kettner <kettner@suse.de>
		Stefan Hundhammer <sh@suse.de>
  Maintainer:	Stefan Hundhammer <sh@suse.de>

/-*/

#include <rpc/types.h>		// MAXHOSTNAMELEN

#include <qcursor.h>
#include <qmessagebox.h>
#include <qsocketnotifier.h>
#include <qvbox.h>
#include <qwidgetstack.h>

#include <ycp/YCPTerm.h>

#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "Y2QtComponent.h"
#include "YEvent.h"
#include "YUISymbols.h"
#include "YQEBunny.h"
#include "utf8.h"

#include "YQDialog.h"
#include "QXEmbed.h"


#define BUSY_CURSOR_TIMEOUT	200	// milliseconds

Y2QtComponent * Y2QtComponent::_ui = 0;


Y2QtComponent::~Y2QtComponent()
{
    y2debug("Closing down Qt UI.");

    normalCursor();

    if ( _busy_cursor )
	delete _busy_cursor;
}


void Y2QtComponent::internalError( const char * msg )
{
    normalCursor();
    int button = QMessageBox::critical( 0, "YaST2 Internal Error", msg,
					QMessageBox::Abort | QMessageBox::Default,
					0 ); // button1
    busyCursor();

    if ( button == QMessageBox::Abort )
    {
	raiseFatalError();
	abort();

	// exit() leaves a process running (WFM?), so this really seems to be
	// the only way to make sure we are really going down.
    }
}


void Y2QtComponent::idleLoop( int fd_ycp )
{
    _leave_idle_loop = false;

    // process Qt events until fd_ycp is readable.
    QSocketNotifier * notifier = new QSocketNotifier( fd_ycp, QSocketNotifier::Read );
    QObject::connect(notifier, SIGNAL( activated( int ) ), this, SLOT( leaveIdleLoop( int ) ) );
    notifier->setEnabled( true );

    while ( !_leave_idle_loop )
	processOneEvent();

    delete notifier;
}


void Y2QtComponent::leaveIdleLoop( int )
{
    _leave_idle_loop = true;
}


void Y2QtComponent::sendEvent( YEvent * event )
{
    if ( event )
    {
	_event_handler.sendEvent( event );

	if ( _do_exit_loop )
	    exit_loop();
    }
}


YEvent * Y2QtComponent::userInput( unsigned long timeout_millisec )
{
    YEvent * 	event  = 0;
    YQDialog *	dialog = dynamic_cast<YQDialog *> ( currentDialog() );

    if ( _user_input_timer.isActive() )
	_user_input_timer.stop();

    if ( dialog )
    {
	if ( timeout_millisec > 0 )
	    _user_input_timer.start( timeout_millisec, true ); // single shot

	dialog->activate( true );

	if ( qApp->focusWidget() )
	    qApp->focusWidget()->setFocus();

	normalCursor();
	_do_exit_loop = true; // should exit_loop() be called in sendEvent()?

	while ( ! pendingEvent() )
	{
	    enter_loop();
	}

	_do_exit_loop = false;
	event = _event_handler.consumePendingEvent();
	dialog->activate( false );

	// Display a busy cursor, but only if there is no other activity within
	// BUSY_CURSOR_TIMEOUT milliseconds (avoid cursor flicker)

	_busy_cursor_timer.start( BUSY_CURSOR_TIMEOUT, true ); // single shot
    }

    if ( _user_input_timer.isActive() )
	_user_input_timer.stop();

    return event;
}


YEvent * Y2QtComponent::pollInput()
{
    YEvent * event = 0;

    if ( _user_input_timer.isActive() )
	_user_input_timer.stop();

    if ( ! pendingEvent() )
    {
	YQDialog * dialog = dynamic_cast<YQDialog *> ( currentDialog() );

	if ( dialog )
	{
	    dialog->activate( true );
	    processEvents();
	    event = _event_handler.consumePendingEvent();
	    dialog->activate( false );
	}
    }

    if ( pendingEvent() )
	event = _event_handler.consumePendingEvent();

    return event;
}


void Y2QtComponent::userInputTimeout()
{
    if ( ! pendingEvent() )
	sendEvent( new YTimeoutEvent() );
}


YDialog * Y2QtComponent::createDialog( YWidgetOpt & opt )
{
    bool has_defaultsize = opt.hasDefaultSize.value();
    QWidget * qt_parent = _main_win;


    // Popup dialogs get the topmost other popup dialog as their parent since
    // some window managers (e.g., fvwm2 as used in the inst-sys) otherwise
    // tend to confuse the stacking order of popup dialogs.
    //
    // This _popup_stack handling would be better placed in showDialog(), but we
    // need the parent here for QWidget creation. libyui guarantees that each
    // createDialog() will be followed by showDialog() for the same dialog
    // without any chance for other dialogs to get in between.

    if ( ! has_defaultsize && ! _popup_stack.empty() )
	qt_parent = _popup_stack.back();

    YQDialog * dialog = new YQDialog( opt, qt_parent, has_defaultsize );
    CHECK_PTR( dialog );

    if ( ! has_defaultsize )
	_popup_stack.push_back( (QWidget *) dialog->widgetRep() );

    return dialog;
}


void Y2QtComponent::showDialog( YDialog * dialog )
{
    QWidget * qw = (QWidget *) dialog->widgetRep();

    if ( ! qw )
    {
	y2error( "No widgetRep() for dialog" );
	return;
    }

    if ( dialog->hasDefaultSize() )
    {
	_widget_stack->addWidget  ( qw, ++_main_dialog_id );
	_widget_stack->raiseWidget( qw ); // maybe this is not necessary (?)

	if ( ! _main_win->isVisible() )
	{
	    // y2milestone( "Showing main window" );
	    _main_win->resize( _default_size );

	    if ( ! _have_wm )
		_main_win->move( 0, 0 );

	    _main_win->show();
	    qw->setFocus();
	}
    }
    else	// non-defaultsize dialog
    {
	qw->setCaption( _kcontrol_id );
	qw->show();
    }

    ( (YQDialog *) dialog)->ensureOnlyOneDefaultButton();
    processEvents();
}


void Y2QtComponent::closeDialog( YDialog * dialog )
{
    QWidget * qw = (QWidget *) dialog->widgetRep();

    if ( ! qw )
    {
	y2error( "No widgetRep() for dialog" );
	return;
    }

    if ( dialog->hasDefaultSize() )
    {
	_widget_stack->removeWidget( qw );

	if ( --_main_dialog_id < 1 )	// nothing left on the stack
	{
	    if ( ! _running_embedded )
	    {
		// y2milestone( "Hiding main window" );
		_main_win->hide();
	    }
	    else
	    {
		y2milestone( "Running embedded - keeping (empty) main window open" );
	    }

	    _main_dialog_id = 0;	// this should not be necessary - but better be safe than sorry
	}
	else
	{
	    _widget_stack->raiseWidget( _main_dialog_id );
	}
    }
    else	// non-defaultsize dialog
    {
	qw->hide();

	// Clean up the popup stack. libyui guarantees that a dialog will be
	// deleted after closeDialog() so it is safe to pop that dialog from
	// the popup stack here.

	if ( ! _popup_stack.empty() && _popup_stack.back() == qw )
	    _popup_stack.pop_back();
	else
	    y2error( "Popup dialog stack corrupted!" );
    }
}


void Y2QtComponent::easterEgg()
{
    y2milestone( "Starting easter egg..." );
    

#if 0
    system( "sudo dd if=/dev/urandom bs=1024 count=1024 of=/dev/fb0" );
    sleep( 2 );
#endif

    YQEasterBunny::layEgg();
    y2milestone( "Done." );

    // desktop()->repaint() has no effect - we need to do it the hard way.
    system( "/usr/X11R6/bin/xrefresh" );
}


QString Y2QtComponent::productName() const
{
    return fromUTF8( Y2UIComponent::productName() );
}



#include "Y2QtComponent.moc"
