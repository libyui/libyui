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

   File:       NCBusyIndicator.cc

   Author:     Thomas Goettlicher <tgoettlicher@suse.de>
   Maintainer: Thomas Goettlicher <tgoettlicher@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCBusyIndicator.h"
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define REPAINT_INTERVAL	100	// in ms
#define STEP_SIZE		.05

/*
 Some words about the timer stuff:
 With each SIG_ALRM signal _timer_progress gets incremented by _timer_divisor.
 When a tick is received [=setAlive(true) is called] _timer_progress is set to 0.
 If _timer_progress is larger than 1 the widget goes to stalled state.

 How the timer works:
 NCBusyIndicatorHandlerWrapper is registered as signal handler for SIG_ALRM
 signal and calls NCBusyIndicatorObject->handler(). This wrapper is needed
 because a member function cannot be registered as signal handler.

 LIMITATIONS:
 i)  Only one BusyIndicator widget works at the same time, because the
     wrapper function only calls the handler() member function of the last
     created instance of BusyIndicator.

 ii) The UserInput widget cannot be used, because UserInput is a blocking
     function. When UserInput waits for UserInput no SIG_ALRM signal is sent
     and therefore the BusyIndicator widget doesn't show progress.
     Please use the TimeoutUserInput widget in a loop instead.
*/

struct itimerval interval;
NCBusyIndicator* NCBusyIndicatorObject;
#if 0
void NCBusyIndicatorHandlerWrapper( int sig_num );
#endif



NCBusyIndicator::NCBusyIndicator( YWidget * parent,
				  const string & nlabel,
				  int timeout )
    : YBusyIndicator( parent, nlabel, timeout )
    , NCWidget( parent )
    , _label( nlabel )
    , _timeout( timeout )
    , _lwin( 0 )
    , _twin( 0 )
    , _position( .5 )
    , _rightwards( true )
    , _alive( true )
{
    yuiDebug() << endl;

    if ( timeout <= 0 )
	timeout = 1;

    setLabel( nlabel );
    hotlabel = &_label;
    wstate = NC::WSdumb;
    NCBusyIndicatorObject = this;
    _timer_divisor = ( double ) REPAINT_INTERVAL / ( double ) timeout;
    _timer_progress = 0;

#if 0
    signal( SIGALRM, NCBusyIndicatorHandlerWrapper );
    interval.it_value.tv_sec = 0;
    interval.it_value.tv_usec = REPAINT_INTERVAL * 1000;
    setitimer( ITIMER_REAL, &interval, NULL );
#endif
}


NCBusyIndicator::~NCBusyIndicator()
{
    NCBusyIndicatorObject = NULL;
    delete _lwin;
    delete _twin;
    yuiDebug() << endl;
}


int NCBusyIndicator::preferredWidth()
{
    return wGetDefsze().W;
}


int NCBusyIndicator::preferredHeight()
{
    return wGetDefsze().H;
}


void NCBusyIndicator::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YBusyIndicator::setEnabled( do_bv );
}


void NCBusyIndicator::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


void NCBusyIndicator::setDefsze()
{
    defsze = wsze( _label.height() + 1,
		   _label.width() < 5 ? 5 : _label.width() );
}


void NCBusyIndicator::wCreate( const wrect & newrect )
{
    NCWidget::wCreate( newrect );

    if ( !win )
	return;

    wrect lrect( 0, wsze::min( newrect.Sze,
			       wsze( _label.height(), newrect.Sze.W ) ) );

    wrect trect( 0, wsze( 1, newrect.Sze.W ) );

    if ( lrect.Sze.H == newrect.Sze.H )
	lrect.Sze.H -= 1;

    trect.Pos.L = lrect.Sze.H > 0 ? lrect.Sze.H : 0;

    _lwin = new NCursesWindow( *win,
			       lrect.Sze.H, lrect.Sze.W,
			       lrect.Pos.L, lrect.Pos.C,
			       'r' );

    _twin = new NCursesWindow( *win,
			       trect.Sze.H, trect.Sze.W,
			       trect.Pos.L, trect.Pos.C,
			       'r' );
}


void NCBusyIndicator::wDelete()
{
    delete _lwin;
    delete _twin;
    _lwin = 0;
    _twin = 0;
    NCWidget::wDelete();
}


void NCBusyIndicator::setLabel( const string & nlabel )
{
    _label = NCstring( nlabel );
    setDefsze();
    YBusyIndicator::setLabel( nlabel );
    Redraw();
}


/**
 * handler, called by NCBusyIndicatorHandlerWrapper
 **/
void NCBusyIndicator::handler( int sig_num )
{
    _timer_progress += _timer_divisor;

    if ( _timer_progress >= 1 )
    {
	_timer_progress = 0;
	_alive = false;
    }

    update();

#if 0
    interval.it_value.tv_sec = 0;
    interval.it_value.tv_usec = REPAINT_INTERVAL * 1000;
    setitimer( ITIMER_REAL, &interval, NULL );
#endif

}


/**
 * static wrapper for member function handler
 **/
#if 0
void NCBusyIndicatorHandlerWrapper( int sig_num )
{
    signal( SIGALRM, SIG_IGN );
    NCBusyIndicatorObject->handler( sig_num );
    signal( SIGALRM, NCBusyIndicatorHandlerWrapper );
}

#endif


/**
 * Calculate position of moving bar
 **/
void NCBusyIndicator::update()
{
    if ( !_alive )
	return;

    if ( _position > 1.0 || _position < 0 )
	_rightwards = !_rightwards;

    if ( _rightwards )
	_position += STEP_SIZE;
    else
	_position -= STEP_SIZE;

    Redraw();
    refresh();
}


/**
 * set alive or stalled
 **/
void NCBusyIndicator::setAlive( bool newAlive )
{
    _alive = newAlive;

    if ( newAlive )
	_timer_progress = 0;
}


void NCBusyIndicator::setTimeout( int newTimeout )
{
    if ( newTimeout < 1 )
	newTimeout = 1;

    _timeout = newTimeout;
    YBusyIndicator::setTimeout( newTimeout );
    _timer_divisor = (double) REPAINT_INTERVAL / (double) _timeout;
}


/**
 * draw busy indicator widget
 **/
void NCBusyIndicator::wRedraw()
{
    if ( !win )
	return;

    // label
    chtype bg = wStyle().dumb.text;
    _lwin->bkgdset( bg );
    _lwin->clear();
    _label.drawAt( *_lwin, bg, bg );
    tUpdate();
}


/**
 * Draw busy bar
 **/
void NCBusyIndicator::tUpdate()
{
    if ( !win )
	return;

    int cp = ( int )(( _twin->maxx() ) * _position );

    const NCstyle::StProgbar & style( wStyle().progbar );

    _twin->bkgdset( style.nonbar.chattr );
    _twin->clear();

    if ( cp <= _twin->maxx() )
    {
	_twin->bkgdset( NCattribute::getNonChar( style.bar.chattr ) );
	_twin->move( 0, cp );
	_twin->addch( NCattribute::getChar( style.bar.chattr ) );
    }
}
