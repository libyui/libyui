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

  File:	      YQBusyIndicator.cc

  Author:     Thomas Goettlicher <tgoettlicher@suse.de>

/-*/


#include <qprogressbar.h>
#include <QVBoxLayout>
#include <QTimer>

#include <qlabel.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::max;

#include "utf8.h"
#include "YQUI.h"
#include "YQBusyIndicator.h"
#include "YQWidgetCaption.h"


YQBusyIndicator::YQBusyIndicator( YWidget * 	parent,
			      const string &	label,
			      int		timeout )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YBusyIndicator( parent, label, timeout )
    , _timeout (timeout)
{

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(setStalled()));  
    _timer->start(_timeout);

    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );

    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, label );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_progressbar = new QProgressBar( this );
    _qt_progressbar->setRange(0, 0);
    YUI_CHECK_NEW( _qt_progressbar );
    layout->addWidget( _qt_progressbar );

    _caption->setBuddy( _qt_progressbar );
}


YQBusyIndicator::~YQBusyIndicator()
{
    // NOP
}


void YQBusyIndicator::setLabel( const string & label )
{
    _caption->setText( label );
    YBusyIndicator::setLabel( label );
}


void YQBusyIndicator::setAlive( bool newAlive )	
{
    YBusyIndicator::setAlive( newAlive );
    if (newAlive)
    {
    	_qt_progressbar->setRange(0, 0);
	_timer->stop();
	_timer->start(_timeout);
    }
    else
    {
    	_qt_progressbar->setRange(0, 1);
	_qt_progressbar->reset();
	_timer->stop();
    }
}


void YQBusyIndicator::setStalled()
{
    setAlive( false );
}


void YQBusyIndicator::setTimeout( int newTimeout )
{
    _timeout = newTimeout;
    YBusyIndicator::setTimeout( newTimeout );
}


void YQBusyIndicator::setEnabled( bool enabled )
{
    _caption->setEnabled( enabled );
    _qt_progressbar->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQBusyIndicator::preferredWidth()
{
    int hintWidth = !_caption->isHidden() ?
      _caption->sizeHint().width() + layout()->margin() : 0;

    return max( 200, hintWidth );
}


int YQBusyIndicator::preferredHeight()
{
    return sizeHint().height();
}


void YQBusyIndicator::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQBusyIndicator::setKeyboardFocus()
{
    _qt_progressbar->setFocus();

    return true;
}


#include "YQBusyIndicator.moc"
