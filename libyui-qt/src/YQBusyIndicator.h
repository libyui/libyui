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

  File:	      YQBusyIndicator.h

  Author:     Thomas Goettlicher <tgoettlicher@suse.de>

/-*/


#ifndef YQBusyIndicator_h
#define YQBusyIndicator_h

#include <QFrame>
#include <QTimer>
#include "YBusyIndicator.h"


class QString;
class YQWidgetCaption;
class QProgressBar;

class YQBusyIndicator : public QFrame, public YBusyIndicator
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQBusyIndicator( YWidget * 		parent,
		   const string &	label,
		   int			timeout = 1000 );
    /**
     * Destructor.
     **/
    virtual ~YQBusyIndicator();

    /**
     * Set the label (the caption above the progress bar).
     *
     * Reimplemented from YBusyIndicator.
     **/
    virtual void setLabel( const string & label );

    /**
     * Set the timeout is ms after that the widget shows 'stalled' when no
     * new tick is received.
     *
     * Reimplemented from YBusyIndicator.
     **/
    virtual void setTimeout( int newTimeout  );

    /**
     * Return the timeout is ms after that the widget shows 'stalled' when no
     * new tick is received.
     *
     * Reimplemented from YBusyIndicator.
     **/
    int timeout()   const   { return _timeout;  }

    /**
     * Send a keep allive message.
     *
     * Reimplemented from YBusyIndicator.
     **/
    virtual void setAlive( bool newAlive );

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Accept the keyboard focus.
     **/
    virtual bool setKeyboardFocus();

protected:

    YQWidgetCaption *	_caption;
    QProgressBar *	_qt_progressbar;
    QTimer *		_timer;
    int			_timeout;

private slots:
    void setStalled();

};

#endif // YQBusyIndicator_h
