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

  File:	      	YQUI_core.cc

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

#include "YQUI.h"
#include "YEvent.h"
#include "YUISymbols.h"
#include "YQEBunny.h"
#include "utf8.h"

#include "YQDialog.h"
#include "QXEmbed.h"


#define BUSY_CURSOR_TIMEOUT	200	// milliseconds

YQUI * YQUI::_ui = 0;

static void qMessageHandler( QtMsgType type, const char * msg );


YQUI::YQUI( int argc, char **argv, bool with_threads, Y2Component *callback )
    : QApplication( argc, argv )
    , YUI( with_threads, callback )
    , _main_win( NULL )
    , _main_dialog_id(0)
    , _do_exit_loop( false )
    , _loaded_current_font( false )
    , _loaded_heading_font( false )
    , _wm_close_blocked( false )
    , _auto_activate_dialogs( true )
    , _running_embedded( false )
    , _argc( argc )
    , _argv( argv )
    , _with_threads( with_threads )

{
    _ui				= this;
    _fatal_error		= false;
    _have_wm			= true;
    _fullscreen			= false;
    _decorate_toplevel_window	= true;
    screenShotNameTemplate 	= "";

    qApp->installEventFilter( this );

    init();
}


void YQUI::init()
{
    processCommandLineArgs();

    if ( _fullscreen )
    {
	_default_size.setWidth ( desktop()->width()  );
	_default_size.setHeight( desktop()->height() );
	y2milestone( "-fullscreen: using %dx%d for `opt(`defaultsize)",
		     _default_size.width(), _default_size.height() );
    }
    else if ( _have_wm )
    {
	// Get _default_size via -geometry command line option

	QWidget * dummy = new QWidget();
	dummy->hide();
	setMainWidget(dummy);
	_default_size = dummy->size();


        // Set min defaultsize

	if ( _default_size.width()  < 640 ||
	     _default_size.height() < 480   )
	{
	    // 640x480 is the absolute minimum, but let's go for 800x600 if we can

	    if ( desktop()->width()  >= 800 &&
		 desktop()->height() >= 600  )
	    {
		_default_size.setWidth ( 800 );
		_default_size.setHeight( 600 );
	    }
	    else
	    {
		_default_size.setWidth ( 640 );
		_default_size.setHeight( 480 );
	    }

	    y2debug( "Assuming default size of %dx%d",
		     _default_size.width(), _default_size.height() );
	}
    }
    else	// ! _have_wm
    {
	_default_size.setWidth ( desktop()->width()  );
	_default_size.setHeight( desktop()->height() );
    }


    y2milestone( "Default size of %dx%d", _default_size.width(), _default_size.height() );

    // Create main window for `opt(`defaultsize) dialogs.
    //
    // We have to use something else than QWidgetStack since QWidgetStack
    // doesn't accept a WFlags arg which we badly need here.

    WFlags wflags = WType_TopLevel;

    if ( ! _decorate_toplevel_window )
    {
	y2debug( "Suppressing WM decorations for toplevel window" );
	wflags |= WStyle_Customize | WStyle_NoBorder;
    }
    
    // if we have a window already, delete it
    if (_main_win) 
	delete _main_win;

    _main_win = new QVBox( 0, 0, wflags ); // parent, name, wflags


    // Create widget stack for `opt(`defaultsize) dialogs

    _widget_stack = new QWidgetStack( _main_win );
    _widget_stack->setFocusPolicy( QWidget::StrongFocus );
    setMainWidget( _main_win );
    _main_win->installEventFilter( this );
    _main_win->resize( _default_size );

    if ( _fullscreen || ! _have_wm )
	_main_win->move( 0, 0 );

    _busy_cursor = new QCursor( WaitCursor );


    // Set window title

    if ( _kcontrol_id.isEmpty() )
    {
	QString title( "YaST2" );
	char hostname[ MAXHOSTNAMELEN+1 ];
	if ( gethostname( hostname, sizeof( hostname )-1 ) == 0 )
	{
	    hostname[ sizeof( hostname ) -1 ] = '\0'; // make sure it's terminated

	    if ( strlen( hostname ) > 0 &&
		 strcmp( hostname, "(none)" ) != 0 &&
		 strcmp( hostname, "linux"  ) != 0 )
	    {
		title += "@";
		title += hostname;
	    }
	}
	_main_win->setCaption( title );
	_kcontrol_id = title;
    }
    else // --_kcontrol_id in command line
    {
	_running_embedded = true;
	_main_win->setCaption( _kcontrol_id );
    }


    // Hide the main window unless we are running embedded. The first call to
    // UI::OpenDialog() on an `opt(`defaultSize) dialog will trigger a
    // showDialog() call that shows the main window - there is nothing to
    // display yet.
    //
    // In embedded mode, keep the main window open so the embedding application
    // (kcontrol) catches the main window as YaST2's first window and not some
    // popup window that may appear before this. An empty grey area for the
    // main window (that will appear for a while) is a lot better than a
    // "please wait" popup zoomed to near full screen that may be embedded -
    // with a large main window that opens somewhere else on the screen.

    if ( ! _running_embedded )
	_main_win->hide();
    else
    {
	_main_win->show();
	y2milestone( "Running in embedded mode - leaving main window open" );
    }


    //  Init other stuff

    setFont( currentFont() );
    QXEmbed::initialize();
    busyCursor();

    connect( & _user_input_timer,	SIGNAL( timeout()          ),
	     this,		  	SLOT  ( userInputTimeout() ) );
    
    connect( & _busy_cursor_timer,	SIGNAL( timeout()	),
	     this,			SLOT  ( busyCursor()	) );
    
    topmostConstructorHasFinished();
#if defined(QT_THREAD_SUPPORT)
    qApp->unlock();
#endif
}


void YQUI::processCommandLineArgs()
{
        for ( int i=1; i < _argc; i++ )
        {
            if ( strcmp( _argv[i], "--nothreads") == 0 ||
                 strcmp( _argv[i], "-nothreads" ) == 0   )
            {
                _with_threads = false;
                y2milestone( "Running Qt UI without threads." );
            }
        }

        // FIXME: The handler must be installed before calling the
        // constructor of QApplication
        qInstallMsgHandler( qMessageHandler );

    if ( _argv )
    {
	for( int i=0; i < _argc; i++ )
	{
	    QString opt = _argv[i];
	    
	    y2milestone ("Qt argument: %s", _argv[i]);

	    // Normalize command line option - accept "--xy" as well as "-xy"

	    if ( opt.startsWith( "--" ) )
		opt.remove(0, 1);

	    if      ( opt == QString( "-no-wm"	 	) )	_have_wm 			= false;
	    else if ( opt == QString( "-fullscreen"	) )	_fullscreen 			= true;
	    else if ( opt == QString( "-noborder" 	) )	_decorate_toplevel_window	= false;
	    else if ( opt == QString( "-kcontrol_id"	) )
	    {
		if ( i+1 >= _argc )
		{
		    y2error( "Missing arg for '--kcontrol_id'" );
		}
		else
		{
		    _kcontrol_id = _argv[++i];
		    y2milestone( "Starting with kcontrol_id='%s'",
				 (const char *) _kcontrol_id );
		}
	    }
	    else if ( opt == QString( "-macro"		) )
	    {
		if ( i+1 >= _argc )
		{
		    y2error( "Missing arg for '--macro'" );
		    fprintf( stderr, "y2base qt: Missing argument for --macro\n" );
		    raiseFatalError();
		}
		else
		{
		    const char * macro_file = _argv[++i];
		    y2milestone( "Playing macro '%s' from command line", macro_file );
		    playMacro( macro_file );
		}
	    }
	    else if ( opt == QString( "-help"  ) )
	    {
		fprintf( stderr,
			 "Command line options for the YaST2 Qt UI:\n"
			 "\n"
			 "--nothreads   run without additional UI threads\n"
			 "--no-wm       assume no window manager is running\n"
			 "--fullscreen  use full screen for `opt(`defaultsize) dialogs\n"
			 "--noborder    no window manager border for `opt(`defaultsize) dialogs\n"
			 "--help        this help text\n"
			 "\n"
			 "--macro <macro-file>        play a macro right on startup\n"
			 "--_kcontrol_id <ID-String>   set KDE control center identification\n"
			 "\n"
			 "-no-wm, -noborder etc. are accepted as well as --no-wm, --noborder\n"
			 "to maintain backwards compatibility.\n"
			 "\n"
			 );

		raiseFatalError();
	    }
	}
    }

    // Qt handles command line option "-reverse" for Arabic / Hebrew
    YUI::_reverseLayout = QApplication::reverseLayout();
}



YQUI::~YQUI()
{
    y2debug("Closing down Qt UI.");

    normalCursor();

    if ( _busy_cursor )
	delete _busy_cursor;
}


void YQUI::internalError( const char * msg )
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


void YQUI::idleLoop( int fd_ycp )
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


void YQUI::leaveIdleLoop( int )
{
    _leave_idle_loop = true;
}


void YQUI::sendEvent( YEvent * event )
{
    if ( event )
    {
	_event_handler.sendEvent( event );

	if ( _do_exit_loop )
	    exit_loop();
    }
}


YEvent * YQUI::userInput( unsigned long timeout_millisec )
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


YEvent * YQUI::pollInput()
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


void YQUI::userInputTimeout()
{
    if ( ! pendingEvent() )
	sendEvent( new YTimeoutEvent() );
}


YDialog * YQUI::createDialog( YWidgetOpt & opt )
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


void YQUI::showDialog( YDialog * dialog )
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


void YQUI::closeDialog( YDialog * dialog )
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


void YQUI::easterEgg()
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


QString YQUI::productName() const
{
    return fromUTF8( YUI::productName() );
}


YCPValue YQUI::evaluate( const YCPValue & command )
{
    if( command->isCode() )
    {
	return command->asCode()->evaluate();
    }
    
    y2error( "Don't know how to handle %s", command->toString().c_str() );

    return YCPVoid();
}


void YQUI::result( const YCPValue & )
{
}


void YQUI::setServerOptions( int argc, char **argv )
{
    y2milestone ("SetServerOptions, num: %d", argc);
    
    for (int i = 0 ; i < argc ; i++)
	y2milestone( "Arg %d: %s", i, argv[i] );

    _argc = argc;
    _argv = argv;
    
    
    // do a new initialization with the arguments
    init();
}


Y2Component *
YQUI::getCallback( void ) const
{
    return _callback;
}


void
YQUI::setCallback( Y2Component * callback )
{
    // interpreter not yet running, save the callback information
    // until first evaluate() call which starts the interpreter
    // and passes this information to it.
    _callback = callback;
    
    return;
}


static void
qMessageHandler( QtMsgType type, const char * msg )
{
    switch (type)
    {
	case QtDebugMsg:
	    y2debug ("qt-debug: %s\n", msg);
	    break;
	case QtWarningMsg:
	    y2warning ("qt-warning: %s\n", msg);
	    break;
	case QtFatalMsg:
	    y2internal ("qt-fatal: %s\n", msg);
	    exit (1);		// qt does the same
    }
}



#include "YQUI.moc"
