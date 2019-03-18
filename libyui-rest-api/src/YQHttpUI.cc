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

#include "YHttpServer.h"
#include "YQHttpUI.h"
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


using std::max;

YQHttpUI::YQHttpUI( bool withThreads )
    : YQUI( withThreads, false )
{
    yuiMilestone() << "YQHttpUI constructor start" << std::endl;
    yuiMilestone() << "This is libyui-qt with http " << VERSION << std::endl;
    _ui				= this;
    yuiMilestone() << "YQHttpUI constructor finished" << std::endl;
    topmostConstructorHasFinished();
}


void YQHttpUI::initUI()
{
    if ( _uiInitialized ){
        yuiMilestone() << "Http Ui already initialized" << std::endl;
        return;
    }
    _uiInitialized = true;
    yuiMilestone() << "Initializing http Qt part" << std::endl;
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
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  new QApplication( _ui_argc, argv );
  Q_CHECK_PTR( qApp );
  // Qt keeps track to a global QApplication in qApp.
  _signalReceiver = new YQHttpUISignalReceiver();
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
  yuiMilestone() << "YQHttpUI initialized. Thread ID: 0x"
         << hex << QThread::currentThreadId () << dec
         << std::endl;

  // handle the HTTP REST API events
  ((YQHttpUISignalReceiver *)_signalReceiver)->createHttpNotifiers();

  qApp->processEvents();

}

YQHttpUI::~YQHttpUI()
{
    yuiMilestone() <<"Closing down Qt UI." << std::endl;

    // Intentionally NOT calling dlclose() to libqt-mt
    // (see constructor for explanation)

    if ( qApp ) // might already be reset to 0 internally from Qt
    {
	qApp->exit();
	qApp->deleteLater();
    }

    delete _signalReceiver;
}

extern YUI * createYQHttpUI( bool withThreads )
{
    yuiMilestone() <<"HTTP create constructor." << std::endl;
    if ( ! YQHttpUI::ui() )
    {
    yuiMilestone() <<"No UI exists." << std::endl;
	YQHttpUI * ui = new YQHttpUI( withThreads );
    yuiMilestone() <<"Called constructor." << std::endl;
	if ( ui && ! withThreads )
        yuiMilestone() <<"Call initUI" << std::endl;
	    ((YQHttpUI *) ui)->initUI();
    } else {
        yuiMilestone() <<"UI exists" << std::endl;
    }

    return YQHttpUI::ui();
}

void YQHttpUI::idleLoop( int fd_ycp )
{
    initUI();
    YQUI::idleLoop( fd_ycp );
}

void YQHttpUI::blockEvents( bool block )
{
    initUI();
    YQUI::blockEvents( block );
}

void YQHttpUI::forceUnblockEvents()
{
    initUI();
    YQUI::forceUnblockEvents();
}


YQHttpUISignalReceiver::YQHttpUISignalReceiver()
    : YQUISignalReceiver()
{
}


void
YQHttpUISignalReceiver::httpData()
{
	YHttpServer::yserver()->process_data();
    yuiMilestone() << "httpData called" << std::endl;

	// refresh the notifiers, there might be changes if a new client connected/disconnected
	// TODO: maybe it could be better optimized to not recreate everyting from scratch...
	createHttpNotifiers();
}

void YQHttpUISignalReceiver::clearHttpNotifiers() {
	yuiMilestone() << "Clearing notifiers..." << std::endl;

	for(QSocketNotifier *_notifier: _http_notifiers)
	{
		yuiMilestone() << "Removing notifier for fd " << _notifier->socket() << std::endl;
		_notifier->setEnabled(false);
		delete _notifier;
	}
	_http_notifiers.clear();
}

void YQHttpUISignalReceiver::createHttpNotifiers()
{
    if (!YHttpServer::yserver()) {
        yuiMilestone() << "No y http server" << std::endl;
        YHttpServer * yserver = new YHttpServer();
        yserver->start();
    }

	clearHttpNotifiers();

	yuiMilestone() << "Adding notifiers..." << std::endl;
	YHttpServerSockets sockets = YHttpServer::yserver()->sockets();

	for(int fd: sockets.read())
	{
		QSocketNotifier *_notifier = new QSocketNotifier( fd, QSocketNotifier::Read );
		QObject::connect( _notifier,	&pclass(_notifier)::activated,
			this, &pclass(this)::httpData);
		_http_notifiers.push_back(_notifier);
	}

	for(int fd: sockets.write())
	{
		QSocketNotifier *_notifier = new QSocketNotifier( fd, QSocketNotifier::Write );
		QObject::connect( _notifier,	&pclass(_notifier)::activated,
			this, &pclass(this)::httpData);
		_http_notifiers.push_back(_notifier);
	}

	for(int fd: sockets.exception())
	{
		QSocketNotifier *_notifier = new QSocketNotifier( fd, QSocketNotifier::Exception );
		QObject::connect( _notifier,	&pclass(_notifier)::activated,
			this, &pclass(this)::httpData);
		_http_notifiers.push_back(_notifier);
	}
}
