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

   File:       NCCheckBoxFrame.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCurses.h"
#include "NCCheckBoxFrame.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBoxFrame::NCCheckBoxFrame
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCCheckBoxFrame::NCCheckBoxFrame( NCWidget * parent, const YWidgetOpt & opt,
				  const YCPString & nlabel, bool checked )
    : YCheckBoxFrame( opt, nlabel )
    , NCWidget( parent )
{
  WIDDBG << endl;
  wstate = NC::WSnormal;
  framedim.Pos = wpos( 1 );
  framedim.Sze = wsze( 2 );

  checkBox = new NCCheckBox( parent, opt, nlabel, checked );

  setLabel( getLabel() );
  hotlabel = &label;

  if ( invertAutoEnable() )
      setValue( false );
  else
      setValue( checked );

// don't call setEnabling( getValue() ) here (widgets are not yet created!)		
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBoxFrame::~NCCheckBoxFrame
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCCheckBoxFrame::~NCCheckBoxFrame()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBoxFrame::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCCheckBoxFrame::nicesize( YUIDimension dim )
{
  defsze = wsze( YContainerWidget::child(0)->nicesize( YD_VERT ),
		 YContainerWidget::child(0)->nicesize( YD_HORIZ ) );
  if ( label.width() > (unsigned)defsze.W )
    defsze.W = label.width();
  defsze += framedim.Sze + 4;	// add space for check box

  return dim == YD_HORIZ ? defsze.W : defsze.H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBoxFrame::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCCheckBoxFrame::setSize( long newwidth, long newheight )
{
  wsze csze( newheight, newwidth );
  wRelocate( wpos( 0 ), csze );
  csze = wsze::max( 0, csze - framedim.Sze );
  YContainerWidget::child(0)->setSize( csze.W, csze.H );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBoxFrame::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCCheckBoxFrame::setLabel( const YCPString & nlabel )
{
  YCheckBoxFrame::setLabel( nlabel );
  
  label = NCstring( getLabel() );
  label.stripHotkey();

  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBoxFrame::setEnabling
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCCheckBoxFrame::setEnabling( bool do_bv )
{
  enabled = do_bv; // in YWidget
  for ( tnode<NCWidget*> * c = this->Next();
	c && c->IsDescendantOf( this );
	c = c->Next() ) {
    if ( c->Value()->GetState() != NC::WSdumb )
      c->Value()->setEnabling( enabled );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBoxFrame::gotBuddy
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCCheckBoxFrame::gotBuddy()
{
  if ( !label.hasHotkey() )
    return false;

  for ( tnode<NCWidget*> * c = this->Next();
	c && c->IsDescendantOf( this );
	c = c->Next() ) {
    if ( c->Value()->GetState() != NC::WSdumb )
      return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBoxFrame::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCCheckBoxFrame::wRedraw()
{
  if ( !win )
    return;

  chtype bg = wStyle().dumb.text;
  win->bkgd( bg );
  win->box();
  if ( gotBuddy() )
    label.drawAt( *win, widgetStyle(), wpos(0,5),
		  wsze(1,win->width()-2), NC::TOPLEFT, false );
  else
    label.drawAt( *win, bg, bg, wpos(0,5),
		  wsze(1,win->width()-2), NC::TOPLEFT, false );

  const NCstyle::StWidget & style( widgetStyle() );
  win->bkgdset( style.plain );
  win->printw( 0, 1, "[ ] " );

  if ( getValue() == true )
      win->printw( 0, 2, "%c", 'x' );
  else
      win->printw( 0, 2, "%c", ' ' ); 
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBoxFrame::getValue()
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
bool NCCheckBoxFrame::getValue()
{
    YCPValue retVal = YCPBoolean ( false );
    
    if ( checkBox )
	retVal = checkBox->getValue();
    
    return retVal->asBoolean()->value();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBoxFrame::setValue()
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCCheckBoxFrame::setValue( bool newValue )
{
    
    if ( checkBox )
	checkBox->setValue( YCPBoolean( newValue ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBoxFrame::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCCheckBoxFrame::wHandleInput( wint_t key )
{
  NCursesEvent ret = NCursesEvent::handled;
  
  if ( key == KEY_SPACE ||
       key == KEY_RETURN ||
       key == KEY_HOTKEY )
  {
      if ( getValue() == true )		// checked
      {
	  setValue( false );
      }
      else
      {
	  setValue( true );
      }
      Redraw();
      
      if ( getNotify() )
	  ret = NCursesEvent::ValueChanged;
      else
	  setEnabling( getValue() );
  }
  
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCCheckBoxFrame::setKeyboardFocus
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCCheckBoxFrame::setKeyboardFocus()
{
    if ( !grabFocus() )
	return YWidget::setKeyboardFocus();
    return true;
}
