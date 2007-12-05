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
#include <libintl.h>

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
    , _do_exit_loop( false )
{
    y2debug( "YQUI constructor start" );

    _ui				= this;
    _fatal_error		= false;
    _fullscreen			= false;
    _usingVisionImpairedPalette	= false;
    _leftHandedMouse		= false;
    _askedForLeftHandedMouse	= false;
    screenShotNameTemplate 	= "";

    qInstallMsgHandler( qMessageHandler );

    y2debug( "Creating QApplication" );
    new QApplication( argc, argv );

    _normalPalette = qApp->palette();

    // Qt keeps track to a global QApplication in qApp.
    CHECK_PTR( qApp );

    processCommandLineArgs( argc, argv );
    calcDefaultSize();


    // Create main window for `opt(`defaultsize) dialogs.
    //
    // We have to use something else than QWidgetStack since QWidgetStack
    // doesn't accept a WFlags arg which we badly need here.

    // if we have a window already, delete it
    if (_main_win)
	delete _main_win;

    _main_win = new QVBox( 0, 0, WType_TopLevel ); // parent, name, wflags


#if 0
    // still needed?
    _widget_stack->setFocusPolicy( QWidget::StrongFocus );
#endif
    
    _main_win->resize( _default_size );

    if ( _fullscreen )
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

    qApp->setFont( yqApp()->currentFont() );
    busyCursor();

    connect( & _user_input_timer,	SIGNAL( timeout()          ),
	     this,		  	SLOT  ( userInputTimeout() ) );

    connect( & _busy_cursor_timer,	SIGNAL( timeout()	),
	     this,			SLOT  ( busyCursor()	) );

    if ( macro_file )
	playMacro( macro_file );


    topmostConstructorHasFinished();

    y2debug( "YQUI constructor end" );
}


YQApplication *
YQUI::yqApp()
{
    return static_cast<YQApplication *>( app() );
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

	    if      ( opt == QString( "-fullscreen"	) )	_fullscreen 	= true;
	    else if ( opt == QString( "-noborder" 	) )	_noborder	= true;
	    else if ( opt == QString( "-auto-font"	) )	yqApp()->setAutoFonts( true );
	    else if ( opt == QString( "-auto-fonts"	) )	yqApp()->setAutoFonts( true );
	    // --macro is handled by YUI_component
	    else if ( opt == QString( "-help"  ) )
	    {
		fprintf( stderr,
			 "Command line options for the YaST2 Qt UI:\n"
			 "\n"
			 "--nothreads   run without additional UI threads\n"
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
}



YQUI::~YQUI()
{
    y2debug("Closing down Qt UI.");

    normalCursor();

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
    else
    {
	// Get _default_size via -geometry command line option (if set)

	// FIXME there must be a better and simpler way to do this
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
	    qApp->processEvents();
	    event = _event_handler.consumePendingEvent();
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


#warning FIXME Move this to Y(Q)Dialog (and rename it to ::finalize()?)
void YQUI::showDialog( YDialog * dialog )
{
    QWidget * qw = (QWidget *) dialog->widgetRep();
    
    if ( qw )
    {
	qw->show();
	qw->raise();
	qw->update();
    }

    ( (YQDialog *) dialog)->ensureOnlyOneDefaultButton();
    qApp->processEvents();
}


void YQUI::closeDialog( YDialog * dialog )
{
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
