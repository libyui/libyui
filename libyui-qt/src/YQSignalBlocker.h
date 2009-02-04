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

  File:	      YQSignalBlocker.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YQSignalBlocker_h
#define YQSignalBlocker_h

class QObject;

/**
 * Helper class to block Qt signals for QWidgets or QObjects as long as this
 * object exists. 
 *
 * This object will restore the old blocked state when it goes out of scope. 
 **/
class YQSignalBlocker
{
public:

    /**
     * Constructor.
     *
     * Qt signals for 'qobject' will be blocked as long as this object exists.
     * Remember that a QWidget is also a QObject.
     **/
    YQSignalBlocker( QObject * qobject );

    /**
     * Destructor.
     *
     * This will restore the old signal state.
     **/
    ~YQSignalBlocker();


private:

    QObject *	_qobject;
    bool	_oldBlockedState;
};


#endif // ifndef YQSignalBlocker_h
