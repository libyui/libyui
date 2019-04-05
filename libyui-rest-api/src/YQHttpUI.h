/*/*
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


/*-/

   File:      YNCursesUI.h
   Purpose:   Introducing rest-api related changes to qt libyui library
              Extends YQUI, overrides initialization to handle events coming
              from the http server
/-*/

#ifndef YQHttpUI_h
#define YQHttpUI_h

#include "YQUI.h"

//! The class of a pointer expression.
// To be used in connect(foo, &pclass(foo)::mysignal, bar, &pclass(bar)::myslot);
// That checks types at compile time,
// unlike the string based SIGNAL and SLOT macros.
#define pclass(ptr) std::remove_reference<decltype(*ptr)>::type

class QSocketNotifier;
class YQHttpUISignalReceiver;

using std::string;
using std::vector;

class YQHttpUI: public YQUI
{
    friend class YQHttpUISignalReceiver;

public:

    /**
     * Constructor.
     **/
    YQHttpUI( bool withThreads );

    /**
     * Destructor.
     **/
    virtual ~YQHttpUI();

    /**
     * Post-constructor initialization. If running with threads, this has to be
     * called in the UI thread. Any subsequent calls will do nothing.
     **/
    void initUI();

};

/**
 * Helper class that acts as a Qt signal receiver for YQUI.
 * YQUI itself cannot be a QObject to avoid problems with starting up the UI
 * with multiple threads.
 **/
class YQHttpUISignalReceiver : public YQUISignalReceiver
{

public:
    YQHttpUISignalReceiver();
     ~YQHttpUISignalReceiver() { clearHttpNotifiers(); }

public slots:
    void httpData();

public:
    void clearHttpNotifiers();
    void createHttpNotifiers();

private:
    std::vector<QSocketNotifier*>  _http_notifiers;
};

/**
 * Create a new UI if there is none yet or return the existing one if there is.
 *
 * This is the UI plugin's interface to the outside world, so don't change the
 * name or signature!
 **/
extern "C" {
    YUI * createYQHttpUI( bool withThreads );
}

#endif // YQHttpUI_h
