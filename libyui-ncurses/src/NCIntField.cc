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

   File:       NCIntField.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCIntField.h"
#include "NCPopupTextEntry.h"
#include "stringutil.h"

///////////////////////////////////////////////////////////////////

const unsigned NCIntField::taglen = 2; // "^v"

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::NCIntField
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCIntField::NCIntField( NCWidget * parent, YWidgetOpt & opt,
			const YCPString & nlabel,
			int minV, int maxV,
			int initialV )
    : YIntField( opt, nlabel,
		 minV <= maxV ? minV : maxV,
		 maxV >= minV ? maxV : minV,
		 initialV )
    , NCWidget( parent )
    , label( nlabel )
    , lwin( 0 )
    , twin( 0 )
    , cvalue( initialV )
    , vlen( 0 )
    , vstart( 0 )
{
  WIDDBG << endl;
  vlen = numstring( minValue() ).length();
  unsigned tmpval = numstring( maxValue() ).length();
  if ( tmpval > vlen )
    vlen = tmpval;
  hotlabel = &label;
  setLabel( nlabel );
  setValue( initialV );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::~NCIntField
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCIntField::~NCIntField()
{
  delete lwin;
  delete twin;
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCIntField::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCIntField::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YIntField::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::setDefsze
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCIntField::setDefsze()
{
  unsigned cols = vlen + taglen;
  defsze = wsze( label.height() + 1,
		 label.width() < cols ? cols : label.width() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::wCreate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCIntField::wCreate( const wrect & newrect )
{
  NCWidget::wCreate( newrect );

  wrect lrect( 0, wsze::min( newrect.Sze,
			     wsze( label.height(), newrect.Sze.W ) ) );
  wrect trect( 0, wsze( 1, newrect.Sze.W ) );

  if ( lrect.Sze.H == newrect.Sze.H )
    lrect.Sze.H -= 1;

  trect.Pos.L = lrect.Sze.H > 0 ? lrect.Sze.H : 0;

  lwin = new NCursesWindow( *win,
			    lrect.Sze.H, lrect.Sze.W,
			    lrect.Pos.L, lrect.Pos.C,
			    'r' );
  twin = new NCursesWindow( *win,
			    trect.Sze.H, trect.Sze.W,
			    trect.Pos.L, trect.Pos.C,
			    'r' );
  vstart = ( vlen + 2 < (unsigned)trect.Sze.W ) ? trect.Sze.W - vlen - 2 : 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::wDelete
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCIntField::wDelete()
{
  delete lwin;
  delete twin;
  lwin = 0;
  twin = 0;
  NCWidget::wDelete();
  vstart = 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCIntField::setLabel( const YCPString & nlabel )
{
  label = NCstring( nlabel );
  setDefsze();
  YIntField::setLabel( nlabel );
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::setValue
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCIntField::setValue( int newValue )
{
  if ( newValue < minValue() )
    cvalue = minValue();
  else if ( newValue > maxValue() )
    cvalue = maxValue();
  else
    cvalue = newValue;

  YIntField::setValue( cvalue );
  tUpdate();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::Increment
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCIntField::Increment( const bool bigstep )
{
  unsigned dist = maxValue() - cvalue;
  if ( !dist )
    return false;

  unsigned step = bigstep ? 10 : 1;
  if ( step < dist )
    setValue( cvalue + step );
  else
    setValue( maxValue() );

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::Decrement
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCIntField::Decrement( const bool bigstep )
{
  unsigned dist = cvalue - minValue();
  if ( !dist )
    return false;

  unsigned step = bigstep ? 10 : 1;

  if ( step < dist )
    setValue( cvalue - step );
  else
    setValue( minValue() );

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCIntField::wRedraw()
{
  if ( !win )
    return;

  // label
  const NCstyle::StWidget & style( widgetStyle( true ) );
  lwin->bkgd( style.plain );
  lwin->clear();
  label.drawAt( *lwin, style );
  tUpdate();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::tUpdate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCIntField::tUpdate()
{
  if ( !win )
    return;

  const NCstyle::StWidget & style( widgetStyle() );
  twin->bkgd( widgetStyle( true ).plain );
  twin->bkgdset( style.data );
  twin->printw( 0, vstart, " %*d ", vlen, cvalue );
  twin->bkgdset( style.scrl );
  twin->addch( 0, vstart,
	       (cvalue != minValue() ? ACS_DARROW : ' ' ) );
  twin->addch( 0, vstart+vlen+1,
	       (cvalue != maxValue() ? ACS_UARROW : ' ' ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCIntField::wHandleInput( int key )
{
  NCursesEvent ret;
  bool   beep   = false;
  int    ovlue  = cvalue;

  switch ( key ) {

  case KEY_UP:
    beep = !Increment();
    break;

  case KEY_DOWN:
    beep = !Decrement();
    break;

  case KEY_PPAGE:
    beep = !Increment( true );
    break;

  case KEY_NPAGE:
    beep = !Decrement( true );
    break;

  case KEY_HOME:
    if ( cvalue != maxValue() )
      setValue( maxValue() );
    else
      beep = true;
    break;

  case KEY_END:
    if ( cvalue != minValue() )
      setValue( minValue() );
    else
      beep = true;
    break;

  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '-':
    enterPopup( key );
    break;
  case '+':
    enterPopup();
    break;

  case KEY_HOTKEY:
    break;

  default:
    beep = true;
    break;
  }

  if ( beep )
    ::beep();

  if ( getNotify() && ovlue != cvalue )
    ret = NCursesEvent::ValueChanged;

  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCIntField::enterPopup
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCIntField::enterPopup( char first )
{
  wpos at( ScreenPos() + wpos( win->maxy()-1, vstart+1 ) );
  string l( string( "[" ) + numstring( minValue() )
	    + "," + numstring( maxValue() ) + "]" );
  string c( 1, first );
  NCPopupTextEntry dialog( at, l, c, vlen, 0,
			   NCTextEntry::NUMBER );

  while ( dialog.post() != -1 ) {
    int nval = atoi( dialog.getText()->value().c_str() );
    if ( nval < minValue() ) {
      dialog.setText( numstring( minValue() ) );
    } else if ( maxValue() < nval ) {
      dialog.setText( numstring( maxValue() ) );
    } else {
      setValue( nval );
      break;
    }
    ::beep();
  }
  return 0;
}
