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

   File:       NCRadioButton.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCRadioButton.h"
#include "NCRadioButtonGroup.h"

#include <ycp/YCPBoolean.h>

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButton::NCRadioButton
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCRadioButton::NCRadioButton( NCWidget * parent, YWidgetOpt & opt,
			      YRadioButtonGroup * rbg,
			      const YCPString & nlabel,
			      bool check )
    : YRadioButton( opt, nlabel, rbg )
    , NCWidget( parent )
    , checked( false )
    , label( nlabel )
{
  WIDDBG << endl;
  hotlabel = &label;
  setLabel( nlabel );
  setValue( check );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButton::~NCRadioButton
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCRadioButton::~NCRadioButton()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButton::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCRadioButton::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButton::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRadioButton::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YRadioButton::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButton::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRadioButton::setLabel( const YCPString & nlabel )
{
  label  = NCstring( nlabel );
  defsze = wsze( label.height(), label.width() + 4 );
  YRadioButton::setLabel( nlabel );
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButton::setValue
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRadioButton::setValue( const YCPBoolean & newval )
{
  setValue( newval->value() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButton::setValue
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRadioButton::setValue( const bool & newval )
{
  if ( newval != checked ) {
    checked = newval;
    if ( checked && buttonGroup() ) {
      buttonGroup()->uncheckOtherButtons( this );
    }
    Redraw();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButton::getValue
//	METHOD TYPE : YCPBoolean
//
//	DESCRIPTION :
//
YCPBoolean NCRadioButton::getValue()
{
  return YCPBoolean( checked );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButton::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRadioButton::wRedraw()
{
  if ( !win )
    return;

  const NCstyle::StWidget & style( widgetStyle() );
  win->bkgdset( style.plain );
  win->printw( 0, 0, "( ) " );
  label.drawAt( *win, style, wpos(0,4) );
  win->bkgdset( style.data );
  win->printw( 0, 1, "%c", (checked ? 'x' : ' ') );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRadioButton::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCRadioButton::wHandleInput( wint_t key )
{
  NCursesEvent ret;
  bool oldChecked = checked;
  NCRadioButtonGroup * group;
  
  switch ( key ) {
  case KEY_HOTKEY:
  case KEY_SPACE:
  case KEY_RETURN:
    setValue( true );
    if ( getNotify() && oldChecked != checked )
      ret = NCursesEvent::ValueChanged;
    break;
  case KEY_UP:
	group = dynamic_cast<NCRadioButtonGroup *>(buttonGroup());
	group->focusPrevButton();
	break;
   case KEY_DOWN:
	group = dynamic_cast<NCRadioButtonGroup *>(buttonGroup());
	group->focusNextButton();
	break;	  
  }	
  return ret;
}
