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

   File:       NCCheckBox.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCurses.h"
#include "NCCheckBox.h"



unsigned char NCCheckBox::statetag[3] = { '?', ' ', 'x' };



NCCheckBox::NCCheckBox( YWidget * parent,
			const string & nlabel,
			bool checked )
    : YCheckBox( parent, nlabel )
    , NCWidget( parent )
    , tristate( false )
    , checkstate( checked ? S_ON : S_OFF )
{
  yuiDebug() << endl;
  setLabel( nlabel );
  hotlabel = &label;
}



NCCheckBox::~NCCheckBox()
{
  yuiDebug() << endl;
}

int NCCheckBox::preferredWidth()
{
    return wGetDefsze().W;
}

int NCCheckBox::preferredHeight()
{
    return wGetDefsze().H;
}

void NCCheckBox::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YCheckBox::setEnabled( do_bv );
}



void NCCheckBox::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}



// set label
void NCCheckBox::setLabel( const string & nlabel )
{
  label  = NCstring( nlabel );
  label.stripHotkey();
  defsze = wsze( label.height(), label.width() + 4 );
  YCheckBox::setLabel( nlabel );
  Redraw();
}



void NCCheckBox::setValue( YCheckBoxState state )
{
    switch ( state )
    {
	case YCheckBox_on:
	    checkstate = S_ON;
	    tristate = false;
	    break;

	case YCheckBox_off:
	    checkstate = S_OFF;
	    tristate = false;
	    break;

	case YCheckBox_dont_care:
	    tristate = true;
	    checkstate = S_DC;
	    break;
    }
    Redraw();
}



YCheckBoxState NCCheckBox::value()
{
    if ( checkstate == S_DC )
	return YCheckBox_dont_care;

    if ( checkstate == S_ON )
	return YCheckBox_on;
    else
	return YCheckBox_off;
}



void NCCheckBox::wRedraw()
{
  if ( !win )
    return;

  const NCstyle::StWidget & style( widgetStyle() );
  win->bkgdset( style.plain );
  win->printw( 0, 0, "[ ] " );
  label.drawAt( *win, style, wpos(0,4) );
  win->bkgdset( style.data );
  win->printw( 0, 1, "%c", statetag[checkstate] );
}



NCursesEvent NCCheckBox::wHandleInput( wint_t key )
{
  NCursesEvent ret;
  switch ( key ) {
  case KEY_HOTKEY:
  case KEY_SPACE:
  case KEY_RETURN:
    switch ( checkstate ) {
    case S_DC:
      checkstate = S_ON;
      break;
    case S_ON:
      checkstate = S_OFF;
      break;
    case S_OFF:
      checkstate = tristate ? S_DC : S_ON;
      break;
    }
    Redraw();
    if ( notify() )
      ret = NCursesEvent::ValueChanged;
    break;
  }
  return ret;
}

