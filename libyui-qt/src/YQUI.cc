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

  File:		YQUI.cc

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/

#include <sys/param.h>		// MAXHOSTNAMELEN
#include <dlfcn.h>
#include <libintl.h>
#include <algorithm>
#include <stdio.h>

#include <QWidget>
#include <QThread>
#include <QSocketNotifier>
#include <QDesktopWidget>
#include <QEvent>
#include <QCursor>
#include <QLocale>
#include <QMessageLogContext>
#include <QMessageBox>
#include <QInputDialog>


#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>
#include <yui/Libyui_config.h>

#include "YQUI.h"

#include <yui/YEvent.h>
#include <yui/YCommandLine.h>
#include <yui/YButtonBox.h>
#include <yui/YUISymbols.h>

#include "QY2Styler.h"
#include "YQApplication.h"
#include "YQDialog.h"
#include "YQWidgetFactory.h"
#include "YQOptionalWidgetFactory.h"

#include "YQWizardButton.h"

#include "YQi18n.h"
#include "utf8.h"

// Include low-level X headers AFTER Qt headers:
// X.h pollutes the global namespace (!!!) with pretty useless #defines
// like "Above", "Below" etc. that clash with some Qt headers.
#include <X11/Xlib.h>


using std::max;

#define BUSY_CURSOR_TIMEOUT	200	// milliseconds
#define VERBOSE_EVENT_LOOP	0



static void qMessageHandler( QtMsgType type, const QMessageLogContext &, const QString & msg );
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
{
    yuiDebug() << "YQUI constructor start" << std::endl;
    yuiMilestone() << "This is libyui-qt " << VERSION << std::endl;

    _ui				= this;
    _uiInitialized		= false;
    _fatalError			= false;
    _fullscreen			= false;
    _noborder			= false;
    screenShotNameTemplate	= "";
    _blockedLevel		= 0;

    qInstallMessageHandler( qMessageHandler );

    yuiDebug() << "YQUI constructor finished" << std::endl;

    topmostConstructorHasFinished();
}


void YQUI::initUI()
{
    if ( _uiInitialized )
	return;

    _uiInitialized = true;
    yuiDebug() << "Initializing Qt part" << std::endl;

    YCommandLine cmdLine; // Retrieve command line args from /proc/<pid>/cmdline
    std::string progName;

    if ( cmdLine.argc() > 0 )
    {
	progName = cmdLine[0];
	std::size_t lastSlashPos = progName.find_last_of( '/' );

	if ( lastSlashPos != std::string::npos )
	    progName = progName.substr( lastSlashPos+1 );

	// Qt will display argv[0] as the window manager title.
	// For YaST2, display "YaST2" instead of "y2base".
	// For other applications, leave argv[0] alone.

	if ( progName == "y2base" )
	    cmdLine.replace( 0, "YaST2" );
    }

    _ui_argc     = cmdLine.argc();
    char ** argv = cmdLine.argv();

    yuiDebug() << "Creating QApplication" << std::endl;
    new QApplication( _ui_argc, argv );
    Q_CHECK_PTR( qApp );
    // Qt keeps track to a global QApplication in qApp.

    _signalReceiver = new YQUISignalReceiver();
    _busyCursorTimer = new QTimer( _signalReceiver );
    _busyCursorTimer->setSingleShot( true );

    (void) QY2Styler::styler();	// Make sure QY2Styler singleton is created

    setButtonOrderFromEnvironment();
    processCommandLineArgs( _ui_argc, argv );
    calcDefaultSize();

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


    //
    // Set application title (used by YQDialog and YQWizard)
    //

    // for YaST2, display "YaST2" instead of "y2base"
    if ( progName == "y2base" )
	_applicationTitle = QString( "YaST2" );
    else
	_applicationTitle = fromUTF8( progName );

    // read x11 display from commandline or environment variable
    int displayArgPos = cmdLine.find( "-display" );
    QString displayName;

    if ( displayArgPos > 0 && displayArgPos+1 < cmdLine.argc() )
	displayName = cmdLine[ displayArgPos+1 ].c_str();
    else
	displayName = getenv( "DISPLAY" );

    // identify hostname
    char hostname[ MAXHOSTNAMELEN+1 ];
    if ( gethostname( hostname, sizeof( hostname )-1 ) == 0 )
	hostname[ sizeof( hostname ) -1 ] = '\0'; // make sure it's terminated
    else
	hostname[0] = '\0';

    // add hostname to the window title if it's not a local display
    if ( !displayName.startsWith( ":" ) && strlen( hostname ) > 0 )
    {
	_applicationTitle += QString( "@" );
	_applicationTitle += fromUTF8( hostname );
    }


#if 0
    // Hide the main window for now. The first call to UI::OpenDialog() on an
    // `opt(`defaultSize) dialog will trigger a dialog->open() call that shows
    // the main window - there is nothing to display yet.

    _main_win->hide();
#endif

    YButtonBoxMargins buttonBoxMargins;
    buttonBoxMargins.left   = 8;
    buttonBoxMargins.right  = 8;
    buttonBoxMargins.top    = 6;
    buttonBoxMargins.bottom = 6;

    buttonBoxMargins.spacing = 4;
    buttonBoxMargins.helpButtonExtraSpacing = 16;
    YButtonBox::setDefaultMargins( buttonBoxMargins );

    //	Init other stuff

    qApp->setFont( yqApp()->currentFont() );
    busyCursor();


    QObject::connect(  _busyCursorTimer,	&pclass(_busyCursorTimer)::timeout,
		       _signalReceiver,		&pclass(_signalReceiver)::slotBusyCursor );

    yuiMilestone() << "YQUI initialized. Thread ID: 0x"
		   << hex << QThread::currentThreadId () << dec
		   << std::endl;

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

	    yuiMilestone() << "Qt argument: " << argv[i] << std::endl;

	    // Normalize command line option - accept "--xy" as well as "-xy"

	    if ( opt.startsWith( "--" ) )
		opt.remove(0, 1);

	    if	    ( opt == QString( "-fullscreen"	) )	_fullscreen	= true;
	    else if ( opt == QString( "-noborder"	) )	_noborder	= true;
	    else if ( opt == QString( "-auto-font"	) )	yqApp()->setAutoFonts( true );
	    else if ( opt == QString( "-auto-fonts"	) )	yqApp()->setAutoFonts( true );
	    else if ( opt == QString( "-gnome-button-order" ) )	YButtonBox::setLayoutPolicy( YButtonBox::gnomeLayoutPolicy() );
	    else if ( opt == QString( "-kde-button-order"   ) )	YButtonBox::setLayoutPolicy( YButtonBox::kdeLayoutPolicy() );
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
    yuiDebug() <<"Closing down Qt UI." << std::endl;

    // Intentionally NOT calling dlclose() to libqt-mt
    // (see constructor for explanation)

    if ( qApp ) // might already be reset to 0 internally from Qt
    {
	qApp->exit();
	qApp->deleteLater();
    }

    delete _signalReceiver;
}

void
YQUI::uiThreadDestructor()
{
    if ( qApp ) // might already be reset to 0 internally from Qt
    {
	qApp->exit();
	qApp->deleteLater();
    }
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
		       << std::endl;
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
			   << std::endl;
	}
    }

    yuiMilestone() << "Default size: "
		   << _defaultSize.width() << " x " << _defaultSize.height()
		   << std::endl;
}


void YQUI::idleLoop( int fd_ycp )
{
    initUI();

    _received_ycp_command = false;
    QSocketNotifier * notifier = new QSocketNotifier( fd_ycp, QSocketNotifier::Read );
    QObject::connect( notifier,		&pclass(notifier)::activated,
		      _signalReceiver,	&pclass(_signalReceiver)::slotReceivedYCPCommand );

    notifier->setEnabled( true );


    //
    // Process Qt events until fd_ycp is readable
    //

#if VERBOSE_EVENT_LOOP
    yuiDebug() << "Entering idle loop" << std::endl;
#endif

    QEventLoop eventLoop( qApp );

    while ( !_received_ycp_command )
	eventLoop.processEvents( QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents );

#if VERBOSE_EVENT_LOOP
    yuiDebug() << "Leaving idle loop" << std::endl;
#endif

    delete notifier;
}


void YQUI::receivedYCPCommand()
{
    _received_ycp_command = true;
}


void YQUI::sendEvent( YEvent * event )
{
    if ( event )
    {
	_eventHandler.sendEvent( event );
	YQDialog * dialog = (YQDialog *) YDialog::currentDialog( false ); // don't throw

	if ( dialog )
	{
	    if ( dialog->eventLoop()->isRunning() )
		dialog->eventLoop()->exit( 0 );
	}
	else
	{
	    yuiError() << "No dialog" << std::endl;
	}
    }
}


void YQUI::setTextdomain( const char * domain )
{
    bindtextdomain( domain, YSettings::localeDir().c_str() );
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
	if ( ++_blockedLevel == 1 )
	{
	    _eventHandler.blockEvents( true );

	    YQDialog * dialog = (YQDialog *) YDialog::currentDialog( false ); // don't throw

	    if ( dialog && dialog->eventLoop()->isRunning() )
	    {
		yuiWarning() << "blocking events in active event loop of " << dialog << std::endl;
		dialog->eventLoop()->exit();
	    }
	}
    }
    else
    {
	if ( --_blockedLevel == 0 )
	{
	    _eventHandler.blockEvents( false );

	    YQDialog * dialog = (YQDialog *) YDialog::currentDialog( false ); // don't throw

	    if ( dialog )
		dialog->eventLoop()->wakeUp();
	}
    }
}


void YQUI::forceUnblockEvents()
{
    initUI();
    _blockedLevel = 0;
    _eventHandler.blockEvents( false );
}


bool YQUI::eventsBlocked() const
{
    return _eventHandler.eventsBlocked();
}


void YQUI::busyCursor()
{
    qApp->setOverrideCursor( Qt::BusyCursor );
}


void YQUI::normalCursor()
{
    if ( _busyCursorTimer->isActive() )
	_busyCursorTimer->stop();

    while ( qApp->overrideCursor() )
	qApp->restoreOverrideCursor();
}


void YQUI::timeoutBusyCursor()
{
    // Display a busy cursor, but only if there is no other activity within
    // BUSY_CURSOR_TIMEOUT milliseconds: Avoid cursor flicker.

    _busyCursorTimer->start( BUSY_CURSOR_TIMEOUT ); // single shot
}


int YQUI::defaultSize(YUIDimension dim) const
{
    return dim == YD_HORIZ ? _defaultSize.width() : _defaultSize.height();
}


void YQUI::probeX11Display( const YCommandLine & cmdLine )
{
    // obsolete, see https://bugzilla.suse.com/show_bug.cgi?id=1072411
}


void YQUI::deleteNotify( YWidget * widget )
{
    _eventHandler.deletePendingEventsFor( widget );
}

// FIXME: Does this still do anything now that YQUI is no longer a QObject?
bool YQUI::close()
{
    yuiMilestone() << "Closing application" << std::endl;
    sendEvent( new YCancelEvent() );
    return true;
}


void YQUI::askSendWidgetID()
{
    QWidget * parent = 0;
    YDialog * dialog = YDialog::currentDialog( false ); // doThrow

    if ( dialog )
        parent = (QWidget *) dialog->widgetRep();

    QString id = QInputDialog::getText( parent,
                                        _( "Widget ID" ), // dialog title
                                        _( "Enter Widget ID:" ) // label
                                        );
    if ( ! id.isEmpty() )
    {
        try
        {
            YWidget * widget = sendWidgetID( toUTF8( id ) );
            YQGenericButton * yqButton = dynamic_cast<YQGenericButton *>( widget );

            if ( yqButton )
            {
                yuiMilestone() << "Activating " << widget << endl;
                yqButton->activate();
            }
        }
        catch ( YUIWidgetNotFoundException & ex )
        {
            YUI_CAUGHT( ex );
            QMessageBox::warning( parent,
                                  _( "Error" ), // title
                                  _( "No widget with ID \"%1\"" ).arg( id ) );
        }
    }
}





YQUISignalReceiver::YQUISignalReceiver()
    : QObject()
{
}


void YQUISignalReceiver::slotBusyCursor()
{
    YQUI::ui()->busyCursor();
}


void YQUISignalReceiver::slotReceivedYCPCommand()
{
    YQUI::ui()->receivedYCPCommand();
}



static void
qMessageHandler( QtMsgType type, const QMessageLogContext &, const QString & msg )
{
    switch (type)
    {
	case QtDebugMsg:
	    yuiMilestone() <<  "<libqt-debug> " << msg << std::endl;
	    break;

#if QT_VERSION >= 0x050500
	case QtInfoMsg:
	    yuiMilestone() <<  "<libqt-info> " << msg << std::endl;
	    break;
#endif

	case QtWarningMsg:
	    yuiWarning() <<  "<libqt-warning> " << msg << std::endl;
	    break;

	case QtCriticalMsg:
	    yuiError() <<  "<libqt-critical>" << msg << std::endl;
	    break;

	case QtFatalMsg:
	    yuiError() << "<libqt-fatal> " << msg << std::endl;
	    abort();
	    exit(1);		// Qt does the same
    }

    if ( QString( msg ).contains( "Fatal IO error",  Qt::CaseInsensitive ) &&
         QString( msg ).contains( "client killed", Qt::CaseInsensitive ) )
        yuiError() << "Client killed. Possibly caused by X server shutdown or crash." << std::endl;
}



#include "YQUI.moc"
