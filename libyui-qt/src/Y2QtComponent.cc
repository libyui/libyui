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


Y2QtComponent::Y2QtComponent()
    : _argc(0)
    , _argv(0)
    , _interpreter(0)
    , _with_threads( true )
{
}


Y2QtComponent::~Y2QtComponent()
{
    if ( _interpreter )
	delete _interpreter;
}


YCPValue Y2QtComponent::evaluate( const YCPValue & command )
{
    if ( ! _interpreter )
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

	// The handler must be installed before calling the
	// constructor of QApplication
	qInstallMsgHandler( qMessageHandler );

	_interpreter = new YUIQt( _argc, _argv, _with_threads, getCallback() );
	
	if ( ! _interpreter || _interpreter->fatalError() )
	    return YCPNull();
    }

    YCPValue val = _interpreter->evaluate(command);

    if ( _interpreter->fatalError() )
    {
	y2error( "Fatal UI error" );
	return YCPNull();
    }
    
    return val;
}


void Y2QtComponent::result( const YCPValue & )
{
    if ( _interpreter )
	delete _interpreter;
    
    _interpreter = 0;
}


void Y2QtComponent::setServerOptions( int argc, char **argv )
{
    _argc = argc;
    _argv = argv;
}


Y2Component *
Y2QtComponent::getCallback( void ) const
{
    if ( _interpreter)
	return _interpreter->getCallback();
    
    return _callback;
}

void
Y2QtComponent::setCallback( Y2Component * callback )
{
    if ( _interpreter)
    {
	// interpreter allready running, pass callback directly
	// to where it belongs
	return _interpreter->setCallback( callback );
    }
    else
    {
	// interpreter not yet running, save the callback information
	// until first evaluate() call which starts the interpreter
	// and passes this information to it.
	_callback = callback;
    }
    
    return;
}

