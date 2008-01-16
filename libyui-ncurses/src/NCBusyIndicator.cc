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

   File:       NCBusyIndicator.cc

   Author:     Thomas Goettlicher <tgoettlicher@suse.de>
   Maintainer: Thomas Goettlicher <tgoettlicher@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCBusyIndicator.h"
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define REPAINT_INTERVAL        100	// in ms
#define STEP_SIZE               .05


struct itimerval interval;


NCBusyIndicator* NCBusyIndicatorObject;
void NCBusyIndicatorHandlerWrapper(int sig_num);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCBusyIndicator::NCBusyIndicator
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCBusyIndicator::NCBusyIndicator( YWidget * parent,
			      const string & nlabel,
			      int timeout )
    : YBusyIndicator( parent, nlabel, timeout )
    , NCWidget( parent )
    , _label( nlabel )
    , _timeout( timeout )
    , _lwin( 0 )
    , _twin( 0 )
    , _position (.5)
    , _rightwards (true)
    , _alive (true)
{
  WIDDBG << endl;
  if ( timeout <= 0 )
    timeout = 1;
  hotlabel = &_label;
  setLabel( nlabel );
  wstate = NC::WSdumb;

  NCBusyIndicatorObject=this;

  _factor =  (double) REPAINT_INTERVAL / (double) timeout; 
//_factor = 0.01;
  _merker=0;

  signal(SIGALRM, NCBusyIndicatorHandlerWrapper);
  interval.it_value.tv_sec=0;
  interval.it_value.tv_usec=REPAINT_INTERVAL * 1000;
  setitimer(ITIMER_REAL, &interval, NULL);

//  alarm(1);	// set timer 1 second
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCBusyIndicator::~NCBusyIndicator
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCBusyIndicator::~NCBusyIndicator()
{
  delete _lwin;
  delete _twin;
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCBusyIndicator::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCBusyIndicator::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCBusyIndicator::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCBusyIndicator::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCBusyIndicator::setDefsze
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCBusyIndicator::setDefsze()
{
  defsze = wsze( _label.height() + 1,
		 _label.width() < 5 ? 5 : _label.width() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCBusyIndicator::wCreate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCBusyIndicator::wDelete
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCBusyIndicator::wDelete()
{
  delete _lwin;
  delete _twin;
  _lwin = 0;
  _twin = 0;
  NCWidget::wDelete();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCBusyIndicator::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCBusyIndicator::setLabel( const string & nlabel )
{
  _label = NCstring( nlabel );
  setDefsze();
  YBusyIndicator::setLabel( nlabel );
  Redraw();
}

void NCBusyIndicator::handler(int sig_num)
{
	_merker+=_factor;
	if (_merker>=1)
	{
		_merker=0;
    		_alive=false;
	}

	update();
	//alarm(1);
	interval.it_value.tv_sec=0;
	interval.it_value.tv_usec=REPAINT_INTERVAL * 1000;
	setitimer(ITIMER_REAL, &interval, NULL);
}

void NCBusyIndicatorHandlerWrapper(int sig_num)
{
	NCBusyIndicatorObject->handler(sig_num);
}

void NCBusyIndicator::update()
{
    if (!_alive)
        return;

    if (_position > 1.0 - STEP_SIZE || _position < STEP_SIZE )
        _rightwards = !_rightwards;

    if (_rightwards)
        _position += STEP_SIZE;
    else
        _position -= STEP_SIZE;

    Redraw();
    refresh();
}

void NCBusyIndicator::setAlive( bool newAlive)
{
	_alive = newAlive;
}

void NCBusyIndicator::setTimeout( int newTimeout)
{
  if (newTimeout < 1)
    newTimeout = 1;

  _timeout = newTimeout;
  YBusyIndicator::setTimeout( newTimeout );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCBusyIndicator::setValue
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
/*void NCBusyIndicator::setValue( int newValue )
{


  cval = newValue;
  if ( cval < 0 )
    cval = 0;
  else if ( cval > maxval )
    cval = maxval;
  Redraw();
  YBusyIndicator::setValue( newValue );

}
*/

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCBusyIndicator::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCBusyIndicator::tUpdate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCBusyIndicator::tUpdate()
{
  if ( !win )
    return;

  double split = double(_twin->maxx()+1) * _position;
  int cp = int(split);
  if ( cp == 0 && split > 0.0 )
    cp = 1;

  const NCstyle::StProgbar & style( wStyle().progbar );
  _twin->bkgdset( style.bar.chattr );
  _twin->clear();

  if ( cp <= _twin->maxx() ) {
    _twin->bkgdset( NCattribute::getNonChar( style.nonbar.chattr ) );
    _twin->move( 0, cp );
    for ( int i = 0; i < _twin->width()-cp; ++i ) {
      _twin->addch( NCattribute::getChar( style.nonbar.chattr ) );
    }
  }


  if ( _twin->maxx() >= 6 ) {
    Value_t pc  = 100 * _position ;
    Value_t off = _twin->maxx() / 2 - ( pc == 100 ? 2
                                                 : pc >= 10 ? 1
                                                           : 0 );
//    char buf[5];
//    sprintf( buf, "%lld%%", pc );
    char buf[5];
    sprintf( buf, "busy");
    _twin->move( 0, off );
    for ( char * ch = buf; *ch; ++ch ) {
      chtype a = _twin->inch();
      NCattribute::setChar( a, *ch );
      _twin->addch( a );
    }
  }



}
