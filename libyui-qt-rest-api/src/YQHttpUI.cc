/*
  Copyright (C) 2018-2019 SUSE LLC
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

#include <QThread>
#include <QSocketNotifier>

#define YUILogComponent "qt-rest-api"
#include <yui/YUILog.h>
#include <yui/Libyui_config.h>

#include "YHttpServer.h"
#include "YQHttpUI.h"
#include "YQUI.h"

YQHttpUI::YQHttpUI( bool withThreads )
    : YQUI( withThreads, false )
{
    yuiMilestone() << "YQHttpUI constructor start" << std::endl;
    _ui                = this;
    yuiMilestone() << "YQHttpUI constructor finished" << std::endl;
    topmostConstructorHasFinished();
}

void YQHttpUI::initUI()
{
    if ( _uiInitialized ) return;

    // call the parent implementation
    YQUI::initUI();

    // but replace the signal receiver with a new one
    // which can also watch the incoming HTTP requests

    delete _busyCursorTimer;
    delete _signalReceiver;

    YQHttpUISignalReceiver *receiver = new YQHttpUISignalReceiver();
    // handle the HTTP REST API events
    receiver->createHttpNotifiers();

    _signalReceiver = receiver;
    _busyCursorTimer = new QTimer( _signalReceiver );
    _busyCursorTimer->setSingleShot( true );
    QObject::connect( _busyCursorTimer, &pclass(_busyCursorTimer)::timeout,
                      _signalReceiver,  &pclass(_signalReceiver)::slotBusyCursor );

    yuiMilestone() << "YQHttpUI initialized. Thread ID: 0x"
         << std::hex << QThread::currentThreadId () << std::dec
         << std::endl;

    qApp->processEvents();
}

YQHttpUI::~YQHttpUI()
{
    yuiMilestone() << "Closing down YQHttpUI UI" << std::endl;
}

extern YUI * createYQHttpUI( bool withThreads )
{
    yuiMilestone() << "This is libyui Qt REST API " << VERSION << std::endl;

    if ( ! YQHttpUI::ui() )
    {
        yuiMilestone() << "No UI exists. Creating YQHttpUI" << std::endl;
        YQHttpUI * ui = new YQHttpUI( withThreads );
        if ( ui && ! withThreads )
            ((YQHttpUI *) ui)->initUI();
    } else {
        yuiMilestone() << "UI exists" << std::endl;
    }

    return YQHttpUI::ui();
}

YQHttpUISignalReceiver::YQHttpUISignalReceiver()
    : YQUISignalReceiver()
{
}

void
YQHttpUISignalReceiver::httpData()
{
    yuiDebug() << "Processing HTTP data" << std::endl;
    YHttpServer::yserver()->process_data();

    // refresh the notifiers, there might be changes if a new client connected/disconnected
    // TODO: maybe it could be better optimized to not recreate everyting from scratch...
    createHttpNotifiers();
}

void YQHttpUISignalReceiver::clearHttpNotifiers() {
    yuiDebug() << "Clearing HTTP notifiers..." << std::endl;

    for(QSocketNotifier *_notifier: _http_notifiers)
    {
        yuiDebug() << "Removing notifier for fd " << _notifier->socket() << std::endl;
        _notifier->setEnabled(false);
        delete _notifier;
    }
    _http_notifiers.clear();
}

void YQHttpUISignalReceiver::createHttpNotifiers()
{
    if (!YHttpServer::yserver()) {
        yuiMilestone() << "Creating the YHttpServer..." << std::endl;
        YHttpServer * yserver = new YHttpServer();
        yserver->start();
    }

    clearHttpNotifiers();

    yuiDebug() << "Adding notifiers..." << std::endl;
    YHttpServerSockets sockets = YHttpServer::yserver()->sockets();

    for(int fd: sockets.read())
    {
        QSocketNotifier *_notifier = new QSocketNotifier( fd, QSocketNotifier::Read );
        QObject::connect( _notifier,    &pclass(_notifier)::activated,
            this, &pclass(this)::httpData);
        _http_notifiers.push_back(_notifier);
    }

    for(int fd: sockets.write())
    {
        QSocketNotifier *_notifier = new QSocketNotifier( fd, QSocketNotifier::Write );
        QObject::connect( _notifier,    &pclass(_notifier)::activated,
            this, &pclass(this)::httpData);
        _http_notifiers.push_back(_notifier);
    }

    for(int fd: sockets.exception())
    {
        QSocketNotifier *_notifier = new QSocketNotifier( fd, QSocketNotifier::Exception );
        QObject::connect( _notifier,    &pclass(_notifier)::activated,
            this, &pclass(this)::httpData);
        _http_notifiers.push_back(_notifier);
    }
}
