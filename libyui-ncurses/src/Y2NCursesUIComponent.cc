/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       Y2NCursesUIComponent.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include "Y2NCursesUIComponent.h"
#include <yui/YUIInterpreter.h>
#include "Y2NCursesUI.h"
#include <ycp/y2log.h>

Y2NCursesUIComponent::Y2NCursesUIComponent()
    : argc(0)
    , argv(0)
    , interpreter(0)
    , m_callback(0)
    , with_threads(true)
{
    y2milestone ("Start Y2NCursesUIComponent");
}

Y2NCursesUIComponent::~Y2NCursesUIComponent()
{
    if (interpreter) {
	y2milestone ("Shutdown Y2NCursesUI...");
	delete interpreter;
	y2milestone ("Y2NCursesUI down");
    }
    y2milestone ("Stop Y2NCursesUIComponent");
}

string Y2NCursesUIComponent::name() const
{
    return "ncurses";
}

YCPValue Y2NCursesUIComponent::evaluate(const YCPValue &command)
{
    if (!interpreter)
    {
	y2milestone ("Launch Y2NCursesUI...");
	for (int i=1; i<argc; i++)
	{
	    if (!strcmp(argv[i], "--nothreads"))
	    {
		with_threads = false;
		y2milestone ("Running Y2NCursesUI without threads");
	    }
	}
	interpreter = new Y2NCursesUI (with_threads, getCallback());
	if (!interpreter)
	{
	    y2internal("Launch Y2NCursesUI failed");
	    return YCPNull();
	}
	y2milestone ("Y2NCursesUIComponent @ %p, Y2NCursesUI @ %p ready, callback @ %p", this, interpreter, getCallback());
    }

    return interpreter->evaluateUI (command);
}


void Y2NCursesUIComponent::result(const YCPValue &result)
{
    if (interpreter)
    {
	y2milestone ("Shutdown Y2NCursesUI...");
	delete interpreter;
	y2milestone ("Y2NCursesUI down");
    }
    interpreter = 0;
}

void Y2NCursesUIComponent::setServerOptions(int argc, char **argv)
{
    this->argc = argc;
    this->argv = argv;
}

Y2Component *
Y2NCursesUIComponent::getCallback (void) const
{
    Y2Component *callback;
    if (interpreter)
    {
	callback = interpreter->getCallback ();
    }
    else
    {
	callback = m_callback;
    }
    y2debug ("Y2NCursesUIComponent[%p]::getCallback[i %p]() = %p", this, interpreter, callback);
    return callback;
}


void
Y2NCursesUIComponent::setCallback (Y2Component *callback)
{
    y2debug ("Y2NCursesUIComponent[%p]::setCallback[i %p](%p)", this, interpreter, callback);
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
