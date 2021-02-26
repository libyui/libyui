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


//#include <qprogressbar.h>
#include <QLabel>
#include <QPalette>
#include <QTimer>
#include <QVBoxLayout>
#include <QFrame>
#include <QPainter>
#include <math.h>

#define YUILogComponent "qt-ui"
#include "YUILog.h"

using std::max;

#include "utf8.h"
#include "YQUI.h"
#include "YQBusyIndicator.h"
#include "YQWidgetCaption.h"


#define REPAINT_INTERVAL	100
#define STEP_SIZE 		.05
#define MINIMUM_WITDH		100
#define MINIMUM_HEIGHT		24


BusyBar::BusyBar(QWidget *parent)
	: QFrame(parent)
	, _position(.5)
	, _rightwards(true)
	, _alive(true)
{
    setMinimumSize(MINIMUM_WITDH, MINIMUM_HEIGHT);

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(update()));  
    _timer->start(REPAINT_INTERVAL);

    setFrameStyle (QFrame::Panel |  QFrame::Sunken );
    setLineWidth(2);
    setMidLineWidth(2);
}

void BusyBar::update()
{
    if (!_alive)
	return;

    if (_position > 1.0 - STEP_SIZE || _position < STEP_SIZE )
	_rightwards = !_rightwards;

    if (_rightwards)
	_position += STEP_SIZE;
    else
	_position -= STEP_SIZE;

    repaint();
}

void BusyBar::run()
{
    _alive=true;
}

void BusyBar::stop()
{
    _alive=false;
}

void BusyBar::paintEvent( QPaintEvent * e )
{

    QPalette palette = QApplication::palette();	
    QColor foreground = palette.color( QPalette::Active, QPalette::Highlight );
    QColor background = palette.color( QPalette::Active, QPalette::Base );
	
    QPainter painter(this);
    QLinearGradient gradient(0, 0, width()-1, 0 );

    gradient.setColorAt( 0.0, background );
    gradient.setColorAt( _position, foreground );
    gradient.setColorAt( 1.0, background );

    painter.setBrush( gradient );
    painter.setPen( Qt::NoPen );
    painter.drawRect( rect() );
    painter.end();

    QFrame::paintEvent( e );
}


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

    _bar = new BusyBar( this );
    YUI_CHECK_NEW ( _bar );
    layout->addWidget( _bar );
    _caption->setBuddy( _bar );
	
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
	_bar->run();
	_timer->stop();
	_timer->start(_timeout);
    }
    else
    {
	_bar->stop();
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
    _bar->setEnabled( enabled );
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
    _bar->setFocus();

    return true;
}


#include "YQBusyIndicator.moc"
