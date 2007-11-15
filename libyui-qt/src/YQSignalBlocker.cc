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

  File:	      YQSignalBlocker.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#include "YQSignalBlocker.h"
#include <qobject.h>


YQSignalBlocker::YQSignalBlocker( QObject * qobject )
    : _qobject( qobject )
{
    if ( _qobject )
    {
	_oldBlockedState = _qobject->signalsBlocked();
	_qobject->blockSignals( true );
    }
}


YQSignalBlocker::~YQSignalBlocker()
{
    if ( _qobject )
	_qobject->blockSignals( _oldBlockedState );
}

