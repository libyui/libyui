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

  File:		YQUI_core.cc

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#include <rpc/types.h>		// MAXHOSTNAMELEN
#include <dlfcn.h>
#include <libintl.h>
#include <algorithm>

#include <QCursor>
#include <QMessageBox>
#include <QSocketNotifier>
#include <QStackedWidget>
#include <QDesktopWidget>
#include <QThread>
#include <QVBoxLayout>

#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "YQUI.h"
#include "QY2Styler.h"
#include "YQApplication.h"
#include "YQWidgetFactory.h"
#include "YQOptionalWidgetFactory.h"
#include "YEvent.h"
#include "YCommandLine.h"
#include "YUISymbols.h"
#include "utf8.h"

#include "YQDialog.h"


using std::max;

#define BUSY_CURSOR_TIMEOUT	200	// milliseconds


static void qMessageHandler( QtMsgType type, const char * msg );
YQUI * YQUI::_ui = 0;


YUI * createUI( bool withThreads )
{
    if ( ! YQUI::ui() )
    {
	YQUI * ui = new YQUI( withThreads );

	if ( ui && ! withThreads )
	    ui->initUI();
    }

    return YQUI::ui();
}


YQUI::YQUI( bool withThreads )
    : YUI( withThreads )
#if 0
    , _main_win( NULL )
#endif
    , _do_exit_loop( false )
    , _eventLoop( 0 )
{
    yuiDebug() << "YQUI constructor start" << endl;

    _ui				= this;
    _uiInitialized		= false;
    _fatalError			= false;
    _fullscreen			= false;
    _usingVisionImpairedPalette = false;
    _noborder			= false;
    screenShotNameTemplate	= "";
    blockedLevel		= 0;

    qInstallMsgHandler( qMessageHandler );
    
    yuiDebug() << "YQUI constructor finished" << endl;

    topmostConstructorHasFinished();
}


void YQUI::initUI()
{
    if ( _uiInitialized )
	return;

    _uiInitialized = true;
    yuiDebug() << "Initializing Qt part" << endl;

    YCommandLine cmdLine; // Retrieve command line args from /proc/<pid>/cmdline
    string progName;

    if ( cmdLine.argc() > 0 )
    {
	progName = cmdLine[0];
	std::size_t lastSlashPos = progName.find_last_of( '/' );

	if ( lastSlashPos != string::npos )
	    progName = progName.substr( lastSlashPos+1 );

	// Qt will display argv[0] as the window manager title.
	// For YaST2, display "YaST2" instead of "y2base".
	// For other applications, leave argv[0] alone.

	if ( progName == "y2base" )
	    cmdLine.replace( 0, "YaST2" );
    }

	_ui_argc = cmdLine.argc();
    char ** argv = cmdLine.argv();

    // YaST2 has no use for the glib event loop
    setenv( "QT_NO_GLIB", "1", 1 );

    new QApplication( _ui_argc, argv );

    _signalReceiver = new YQUISignalReceiver();
    _busyCursorTimer = new QTimer( _signalReceiver );
    _busyCursorTimer->setSingleShot( true );

    _userInputTimer = new QTimer( _signalReceiver );
    _userInputTimer->setSingleShot( true );

    _normalPalette = qApp->palette();

    // Qt keeps track to a global QApplication in qApp.
    Q_CHECK_PTR( qApp );

    processCommandLineArgs( _ui_argc, argv );
    calcDefaultSize();

    _styler = new QY2Styler( qApp );
    QString style = getenv("Y2STYLE");
    
    if ( !style.isEmpty() )
	_styler->setStyleSheet( style );
    else
	_styler->setStyleSheet( "style.qss" );

    // Event loop object. Required since a YaST2 UI needs to react to commands
    // from the YCP command stream as well as to X11 / Qt events.
    _eventLoop = new QEventLoop( qApp );
    _do_exit_loop = false;

#if 0
    // Create main window for `opt(`defaultsize) dialogs.
    //
    // We have to use something else than QWidgetStack since QWidgetStack
    // doesn't accept a WFlags arg which we badly need here.

    _main_win = new QWidget( 0, Qt::Window ); // parent, wflags
    _main_win->setFocusPolicy( Qt::StrongFocus );
    _main_win->setObjectName( "main_window" );

    _main_win->resize( _defaultSize );

    if ( _fullscreen )
	_main_win->move( 0, 0 );
#endif


    // Set window title

    QString title( fromUTF8( progName ) );
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

#if 0
    // Hide the main window for now. The first call to UI::OpenDialog() on an
    // `opt(`defaultSize) dialog will trigger a dialog->open() call that shows
    // the main window - there is nothing to display yet.

    _main_win->hide();
#endif


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

    QString qt_lib_name = QString( QTLIBDIR "/libQtGui.so.%1" ).arg( QT_VERSION >> 16 );;
    void * qt_lib = dlopen( qt_lib_name.toUtf8().constData(), RTLD_GLOBAL );
    yuiMilestone() << "Forcing " << qt_lib_name.toUtf8().constData() << " open "
		   << ( qt_lib ? "successful" : "failed" )
		   << endl;

    //	Init other stuff

    qApp->setFont( yqApp()->currentFont() );
    busyCursor();

    QObject::connect(  _userInputTimer, SIGNAL( timeout()	   ),
		       _signalReceiver,		SLOT  ( slotUserInputTimeout() ) );

    QObject::connect(  _busyCursorTimer,	SIGNAL( timeout()	),
		       _signalReceiver,		SLOT  ( slotBusyCursor() ) );

    yuiMilestone() << "YQUI initialized. Thread ID: 0x"
		   << hex << QThread::currentThreadId () << dec
		   << endl;

    qApp->processEvents();
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

	    yuiMilestone() << "Qt argument: " << argv[i] << endl;

	    // Normalize command line option - accept "--xy" as well as "-xy"

	    if ( opt.startsWith( "--" ) )
		opt.remove(0, 1);

	    if	    ( opt == QString( "-fullscreen"	) )	_fullscreen	= true;
	    else if ( opt == QString( "-noborder"	) )	_noborder	= true;
	    else if ( opt == QString( "-auto-font"	) )	yqApp()->setAutoFonts( true );
	    else if ( opt == QString( "-auto-fonts"	) )	yqApp()->setAutoFonts( true );
	    // --macro is handled by YUI_component
	    else if ( opt == QString( "-help"  ) )
	    {
		fprintf( stderr,
			 "Command line options for the YaST2 Qt UI:\n"
			 "\n"
			 "--nothreads	run without additional UI threads\n"
			 "--fullscreen	use full screen for `opt(`defaultsize) dialogs\n"
			 "--noborder	no window manager border for `opt(`defaultsize) dialogs\n"
			 "--auto-fonts	automatically pick fonts, disregard Qt standard settings\n"
			 "--help	this help text\n"
			 "\n"
			 "--macro <macro-file>	      play a macro right on startup\n"
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
    yuiDebug() <<"Closing down Qt UI." << endl;

    normalCursor();

    // Intentionally NOT calling dlclose() to libqt-mt
    // (see constructor for explanation)

    qApp->exit();
    qApp->deleteLater();

    delete _signalReceiver;
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
    QSize primaryScreenSize	= qApp->desktop()->screenGeometry( qApp->desktop()->primaryScreen() ).size();
    QSize availableSize		= qApp->desktop()->availableGeometry( qApp->desktop()->primaryScreen() ).size();

    if ( _fullscreen )
    {
	_defaultSize = availableSize;

	yuiMilestone() << "-fullscreen: using "
		       << _defaultSize.width() << " x " << _defaultSize.height()
		       << "for `opt(`defaultsize)"
		       << endl;
    }
    else
    {
	// Get _defaultSize via -geometry command line option (if set)

	// Set min defaultsize or figure one out if -geometry was not used

	if ( _defaultSize.width()  < 800 ||
	     _defaultSize.height() < 600   )
	{
	    if ( primaryScreenSize.width() >= 1024 && primaryScreenSize.height() >= 768	 )
	    {
		// Scale down to 70% of screen size

		_defaultSize.setWidth ( max( (int) (availableSize.width()  * 0.7), 800 ) );
		_defaultSize.setHeight( max( (int) (availableSize.height() * 0.7), 600 ) );
	    }
	    else
	    {
		_defaultSize = availableSize;
	    }
	}
	else
	{
	    yuiMilestone() << "Forced size (via -geometry): "
			   << _defaultSize.width() << " x " << _defaultSize.height()
			   << endl;
	}
    }

    yuiMilestone() << "Default size: "
		   << _defaultSize.width() << " x " << _defaultSize.height()
		   << endl;
}


void YQUI::idleLoop( int fd_ycp )
{
    initUI();

    _leave_idle_loop = false;

    // process Qt events until fd_ycp is readable.
    QSocketNotifier * notifier = new QSocketNotifier( fd_ycp, QSocketNotifier::Read );
    QObject::connect( notifier,		SIGNAL( activated( int )    ),
		      _signalReceiver,	SLOT  ( slotLeaveIdleLoop() ) );

    notifier->setEnabled( true );

    while ( !_leave_idle_loop )
	_eventLoop->processEvents( QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents );

    delete notifier;
}


void YQUI::leaveIdleLoop()
{
    _leave_idle_loop = true;
}


void YQUI::sendEvent( YEvent * event )
{
    if ( event )
    {
	_eventHandler.sendEvent( event );

	if ( _do_exit_loop )
	    _eventLoop->exit( 1 );
    }
}


YEvent * YQUI::userInput( int timeout_millisec )
{
    initUI();

    _eventHandler.blockEvents( false );
    _eventLoop->wakeUp();
    blockedLevel = 0;

#if 0
    yuiMilestone() << "userInput( " << timeout_millisec
		   << " ) Thread ID: " << hex <<  QThread::currentThreadId() << dec
		   << endl;
#endif

    YEvent *	event  = 0;
    YDialog *	dialog = YDialog::currentDialog( false );

    _userInputTimer->stop();

    if ( dialog )
    {
	if ( timeout_millisec > 0 )
	    _userInputTimer->start( timeout_millisec ); // single shot

	if ( qApp->focusWidget() )
	    qApp->focusWidget()->setFocus();

	normalCursor();
	_do_exit_loop = true; // should exit_loop() be called in sendEvent()?
	_eventLoop->exec();
	_do_exit_loop = false;

	event = _eventHandler.consumePendingEvent();

	// Display a busy cursor, but only if there is no other activity within
	// BUSY_CURSOR_TIMEOUT milliseconds: Avoid cursor flicker.

	_busyCursorTimer->start( BUSY_CURSOR_TIMEOUT ); // single shot
    }

    _userInputTimer->stop();

    return event;
}


YEvent * YQUI::pollInput()
{
    YEvent * event = 0;

    _userInputTimer->stop();

    if ( ! pendingEvent() )
    {
	YDialog * dialog = YDialog::currentDialog( false );

	if ( dialog )
	{
	    _eventLoop->processEvents( QEventLoop::AllEvents, 10 );
	    event = _eventHandler.consumePendingEvent();
	}
    }

    if ( pendingEvent() )
	event = _eventHandler.consumePendingEvent();

    return event;
}


void YQUI::userInputTimeout()
{
    if ( ! pendingEvent() )
	sendEvent( new YTimeoutEvent() );
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


void YQUI::blockEvents( bool block )
{
    initUI();

    if ( block )
    {
	if ( ++blockedLevel == 1 )
	{
	    _eventHandler.blockEvents( true );
	    _eventLoop->exit();
	}
    }
    else
    {
	if ( --blockedLevel == 0 )
	{
	    _eventHandler.blockEvents( false );
	    _eventLoop->wakeUp();
	}
    }
}


bool YQUI::eventsBlocked() const
{
    return _eventHandler.eventsBlocked();
}





YQUISignalReceiver::YQUISignalReceiver()
    : QObject()
{
}


void YQUISignalReceiver::slotBusyCursor()
{
    YQUI::ui()->busyCursor();
}


void YQUISignalReceiver::slotUserInputTimeout()
{
    YQUI::ui()->userInputTimeout();
}


void YQUISignalReceiver::slotLeaveIdleLoop()
{
    YQUI::ui()->leaveIdleLoop();
}




static void
qMessageHandler( QtMsgType type, const char * msg )
{
    switch (type)
    {
	case QtDebugMsg:
	    yuiMilestone() <<  "<qt-debug> " << msg << endl;
	    break;
	case QtWarningMsg:
	    yuiWarning() <<  "<qt-warning> " << msg << endl;
#ifndef NDEBUG
	    //abort();
#endif
	    break;
	case QtCriticalMsg:
	    yuiError() <<  "<qt-critical>" << msg << endl;
#ifndef NDEBUG
	    //abort();
#endif
	    break;
	case QtFatalMsg:
	    yuiError() << "<qt-fatal> " << msg << endl;
	    abort();
	    exit(1);		// qt does the same
    }
}



#include "YQUI.moc"
