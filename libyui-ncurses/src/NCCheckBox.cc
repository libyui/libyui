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
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCCheckBox.h"

#include <ycp/YCPVoid.h>
#include <ycp/YCPBoolean.h>

///////////////////////////////////////////////////////////////////

unsigned char NCCheckBox::statetag[3] = { '?', ' ', 'x' };

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBox::NCCheckBox
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCCheckBox::NCCheckBox( NCWidget * parent, YWidgetOpt & opt,
			const YCPString & nlabel,
			bool check )
    : YCheckBox( opt, nlabel )
    , NCWidget( parent )
    , tristate( false )
    , checkstate( check ? S_ON : S_OFF )
    , label( nlabel )
{
  WIDDBG << endl;
  hotlabel = &label;
  setLabel( nlabel );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBox::~NCCheckBox
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCCheckBox::~NCCheckBox()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBox::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCCheckBox::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBox::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCCheckBox::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YCheckBox::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBox::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCCheckBox::setLabel( const YCPString & nlabel )
{
  label  = NCstring( nlabel );
  defsze = wsze( label.height(), label.width() + 4 );
  YCheckBox::setLabel( nlabel );
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBox::setValue
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCCheckBox::setValue( const YCPValue & newval )
{
  if ( newval->isBoolean() ) {
    tristate = false;
    checkstate = newval->asBoolean()->value() ? S_ON : S_OFF;
  }
  else {
    tristate = true;
    checkstate = S_DC;
  }
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBox::getValue
//	METHOD TYPE : YCPBoolean
//
//	DESCRIPTION :
//
YCPValue NCCheckBox::getValue()
{
  if ( checkstate == S_DC )
    return YCPVoid();

  return YCPBoolean( checkstate == S_ON );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBox::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBox::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
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
    if ( getNotify() )
      ret = NCursesEvent::ValueChanged;
    break;
  }
  return ret;
}

