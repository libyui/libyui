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

  File:	      Y2QtComponent.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include "Y2QtComponent.h"
#include "YUIInterpreter.h"
#include "YUIQt.h"


static void
myqmsg (QtMsgType type, const char* msg)
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


Y2QtComponent::Y2QtComponent()
    : argc(0)
    , argv(0)
    , interpreter(0)
    , with_threads(true)
{
}


Y2QtComponent::~Y2QtComponent()
{
    if (interpreter) delete interpreter;
}


string Y2QtComponent::name() const
{
    return "qt";
}


YCPValue Y2QtComponent::evaluate(const YCPValue &command)
{
    if (!interpreter)
    {
	for (int i=1; i<argc; i++)
	{
	    if (!strcmp(argv[i], "--nothreads"))
	    {
		with_threads = false;
		y2milestone("Running Qt UI without threads.");
	    }
	}

	// the handler must be installed before calling the
	// constructor of QApplication
	qInstallMsgHandler (myqmsg);

	interpreter = new YUIQt(argc, argv, with_threads, getCallback());
	if (!interpreter)
	    return YCPNull();
    }

    return interpreter->evaluate(command);
}


void Y2QtComponent::result(const YCPValue &)
{
    if (interpreter) delete interpreter;
    interpreter = 0;
}


void Y2QtComponent::setServerOptions(int argc, char **argv)
{
    this->argc = argc;
    this->argv = argv;
}


Y2Component *
Y2QtComponent::getCallback (void) const
{
    if (interpreter)
	return interpreter->getCallback ();
    return m_callback;
}

void
Y2QtComponent::setCallback (Y2Component *callback)
{
    if (interpreter)
    {
	// interpreter allready running, pass callback directly
	// to where it belongs
	return interpreter->setCallback (callback);
    }
    else
    {
	// interpreter not yet running, save the callback information
	// until first evaluate() call which starts the interpreter
	// and passes this information to it.
	m_callback = callback;
    }
    return;
}
