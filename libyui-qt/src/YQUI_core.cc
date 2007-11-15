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

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#include <rpc/types.h>		// MAXHOSTNAMELEN
#include <dlfcn.h>

#include <qcursor.h>
#include <qmessagebox.h>
#include <qsocketnotifier.h>
#include <qvbox.h>
#include <qwidgetstack.h>

#include <ycp/YCPTerm.h>
#include <ycp/YCPCode.h>

#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YQUI.h"
#include "YQApplication.h"
#include "YQWidgetFactory.h"
#include "YQOptionalWidgetFactory.h"
#include "YEvent.h"
#include "YUISymbols.h"
#include "YQEBunny.h"
#include "utf8.h"

#include "YQDialog.h"
#include "QY2Settings.h"


#define BUSY_CURSOR_TIMEOUT	200	// milliseconds
#define KYAST_EMBEDDING		0


YQUI * YQUI::_ui = 0;


static void qMessageHandler( QtMsgType type, const char * msg );

YQUI::YQUI( int argc, char **argv, bool with_threads, const char * macro_file )
    : QObject()
    , YUI( with_threads )
    , _main_win( NULL )
    , _main_dialog_id(0)
    , _do_exit_loop( false )
    , _font_family( "Sans Serif" )
    , _lang_fonts( 0 )
    , _loaded_current_font( false )
    , _loaded_bold_font( false )
    , _loaded_heading_font( false )
    , _auto_fonts( false )
    , _auto_normal_font_size( -1 )
    , _auto_heading_font_size( -1 )
    , _wm_close_blocked( false )
    , _auto_activate_dialogs( true )
{
    _ui				= this;
    _fatal_error		= false;
    _have_wm			= true;
    _fullscreen			= false;
    _decorate_toplevel_window	= true;
    _usingVisionImpairedPalette	= false;
    _leftHandedMouse		= false;
    _askedForLeftHandedMouse	= false;
    screenShotNameTemplate 	= "";

    qInstallMsgHandler( qMessageHandler );

    new QApplication( argc, argv );
    loadPredefinedQtTranslations();
    _normalPalette = qApp->palette();

    // Qt keeps track to a global QApplication in qApp.
    CHECK_PTR( qApp );

    qApp->installEventFilter( this );
    processCommandLineArgs( argc, argv );
    calcDefaultSize();


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
    qApp->setMainWidget( _main_win );
    _main_win->installEventFilter( this );
    _main_win->resize( _default_size );

    if ( _fullscreen || ! _have_wm )
	_main_win->move( 0, 0 );


    // Set window title

    QString title( "YaST2" );
    char hostname[ MAXHOSTNAMELEN+1 ];

    if ( gethostname( hostname, sizeof( hostname )-1 ) == 0 )
    {
	hostname[ sizeof( hostname ) -1 ] = '\0'; // make sure it's terminated

	if ( strlen( hostname ) > 0 )
	{
	    if ( ( strcmp( hostname, "(none)" ) != 0 &&
		   strcmp( hostname, "linux"  ) != 0 ) )
	    {
		title += "@";
		title += hostname;
	    }
	}
    }

    _main_win->setCaption( title );


    // Hide the main window for now. The first call to UI::OpenDialog() on an
    // `opt(`defaultSize) dialog will trigger a showDialog() call that shows
    // the main window - there is nothing to display yet.

    _main_win->hide();


    // Ugly hack as a workaround of bug #121872 (Segfault at program exit
    // if no Qt style defined):
    //
    // Qt does not seem to be designed for use in plugin libs. It loads some
    // add-on libs dynamically with dlopen() and unloads them at program exit
    // (QGPluginManager). Unfortunately, since they all depend on the Qt master
    // lib (libqt-mt) themselves, when they are unloading the last call to
    // dlclose() for them causes the last reference to libqt-mt to vanish as
    // well. Since libqt-mt is already in the process of destruction there is
    // no more reference from the caller of libqt-mt, and the GLIBC decides
    // that libqt-mt is not needed any more (zero references) and unmaps
    // libqt-mt. When the static destructor of libqt-mt that triggered the
    // cleanup in QGPluginManager returns, the code it is to return to is
    // already unmapped, causing a segfault.
    //
    // Workaround: Keep one more reference to libqt-mt open - dlopen() it here
    // and make sure there is no corresponding dlclose().

    QString qt_lib_name = QString( QTLIBDIR "/libqt-mt.so.%1" ).arg( QT_VERSION >> 16 );;
    void * qt_lib = dlopen( (const char *) qt_lib_name, RTLD_GLOBAL );
    y2milestone( "Forcing %s open %s", (const char *) qt_lib_name,
		 qt_lib ? "successful" : "failed" );


    //  Init other stuff

    qApp->setFont( currentFont() );
    busyCursor();

    connect( & _user_input_timer,	SIGNAL( timeout()          ),
	     this,		  	SLOT  ( userInputTimeout() ) );

    connect( & _busy_cursor_timer,	SIGNAL( timeout()	),
	     this,			SLOT  ( busyCursor()	) );

    if ( macro_file )
	playMacro( macro_file );

    topmostConstructorHasFinished();
}


void YQUI::processCommandLineArgs( int argc, char **argv )
{
    if ( argv )
    {
	for( int i=0; i < argc; i++ )
	{
	    QString opt = argv[i];

	    y2milestone ("Qt argument: %s", argv[i]);

	    // Normalize command line option - accept "--xy" as well as "-xy"

	    if ( opt.startsWith( "--" ) )
		opt.remove(0, 1);

	    if      ( opt == QString( "-no-wm"	 	) )	_have_wm 			= false;
	    else if ( opt == QString( "-fullscreen"	) )	_fullscreen 			= true;
	    else if ( opt == QString( "-noborder" 	) )	_decorate_toplevel_window	= false;
	    else if ( opt == QString( "-auto-font"	) )	_auto_fonts			= true;
	    else if ( opt == QString( "-auto-fonts"	) )	_auto_fonts			= true;
	    // --macro is handled by YUI_component
	    else if ( opt == QString( "-help"  ) )
	    {
		fprintf( stderr,
			 "Command line options for the YaST2 Qt UI:\n"
			 "\n"
			 "--nothreads   run without additional UI threads\n"
			 "--no-wm       assume no window manager is running\n"
			 "--fullscreen  use full screen for `opt(`defaultsize) dialogs\n"
			 "--noborder    no window manager border for `opt(`defaultsize) dialogs\n"
			 "--auto-fonts	automatically pick fonts, disregard Qt standard settings\n"
			 "--help        this help text\n"
			 "\n"
			 "--macro <macro-file>        play a macro right on startup\n"
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

    if ( _lang_fonts )
	delete _lang_fonts;

    // Intentionally NOT calling dlclose() to libqt-mt
    // (see constructor for explanation)
}



YWidgetFactory *
YQUI::createWidgetFactory()
{
    YQWidgetFactory * factory = new YQWidgetFactory();
    YUI_CHECK_NEW( factory );

    return factory;
}



YOptionalWidgetFactory *
YQUI::createOptionalWidgetFactory()
{
    YQOptionalWidgetFactory * factory = new YQOptionalWidgetFactory();
    YUI_CHECK_NEW( factory );

    return factory;
}


YApplication *
YQUI::createApplication()
{
    YQApplication * app = new YQApplication();
    YUI_CHECK_NEW( app );

    return app;
}



void YQUI::calcDefaultSize()
{
    QSize primaryScreenSize 	= qApp->desktop()->screenGeometry( qApp->desktop()->primaryScreen() ).size();
    QSize availableSize		= qApp->desktop()->availableGeometry().size();

    if ( _fullscreen )
    {
	_default_size = availableSize;

        y2milestone( "-fullscreen: using %dx%d for `opt(`defaultsize)",
		     _default_size.width(), _default_size.height() );
    }
    else if ( _have_wm )
    {
	// Get _default_size via -geometry command line option (if set)

	QWidget * dummy = new QWidget();
	dummy->hide();
	qApp->setMainWidget( dummy );
	_default_size = dummy->size();


        // Set min defaultsize or figure one out if -geometry was not used

	if ( _default_size.width()  < 800 ||
	     _default_size.height() < 600   )
	{
	    if ( primaryScreenSize.width() >= 1024 && primaryScreenSize.height() >= 768  )
	    {
		// Scale down to 70% of screen size

		_default_size.setWidth ( max( (int) (availableSize.width()  * 0.7), 800 ) );
		_default_size.setHeight( max( (int) (availableSize.height() * 0.7), 600 ) );
	    }
	    else
	    {
		_default_size = availableSize;
	    }
	}
	else
	{
	    y2milestone( "Forced size (via -geometry): %dx%d",
			 _default_size.width(), _default_size.height() );
	}
    }
    else	// ! _have_wm
    {
	_default_size = primaryScreenSize;
    }


    y2milestone( "Default size: %dx%d", _default_size.width(), _default_size.height() );
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
    QObject::connect( notifier, SIGNAL( activated    ( int ) ),
		      this,	SLOT  ( leaveIdleLoop( int ) ) );

    notifier->setEnabled( true );

    while ( !_leave_idle_loop )
	qApp->processOneEvent();

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
	    qApp->exit_loop();
    }
}


YEvent * YQUI::userInput( unsigned long timeout_millisec )
{
    YEvent * 	event  = 0;
    YQDialog *	dialog = dynamic_cast<YQDialog *> ( YDialog::currentDialog( false ) );

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
	    qApp->enter_loop();
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
	YQDialog * dialog = dynamic_cast<YQDialog *> ( YDialog::currentDialog( false ) );

	if ( dialog )
	{
	    dialog->activate( true );
	    qApp->processEvents();
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
	qw->show();
    }

    ( (YQDialog *) dialog)->ensureOnlyOneDefaultButton();
    qApp->processEvents();
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
	    // y2milestone( "Hiding main window" );
	    _main_win->hide();
	    _main_dialog_id = 0;	// this should not be necessary - but better be safe than sorry
	}
	else
	{
	    // FIXME: This is anti-social behaviour - do this only in the inst-sys
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


    YQEasterBunny::layEgg();
    y2milestone( "Done." );

#if 0
    // desktop()->repaint() has no effect - we need to do it the hard way.
    system( "/usr/X11R6/bin/xrefresh" );
#endif
}


QString YQUI::productName() const
{
    return fromUTF8( YUI::productName() );
}


void
YQUI::setTextdomain( const char * domain )
{
    bindtextdomain( domain, LOCALEDIR );
    bind_textdomain_codeset( domain, "utf8" );
    textdomain( domain );

    // Make change known.
    {
	extern int _nl_msg_cat_cntr;
	++_nl_msg_cat_cntr;
    }
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
