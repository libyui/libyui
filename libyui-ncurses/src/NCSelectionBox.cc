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

   File:       NCSelectionBox.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCSelectionBox.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSelectionBox::NCSelectionBox
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCSelectionBox::NCSelectionBox( NCWidget * parent, YWidgetOpt & opt,
				const YCPString & nlabel )
    : YSelectionBox( opt, nlabel )
    , NCPadWidget( parent )
    , pad( (NCTablePad *&)NCPadWidget::pad )
    , biglist( false )
{
  WIDDBG << endl;
  InitPad();
  setLabel( nlabel );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSelectionBox::~NCSelectionBox
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCSelectionBox::~NCSelectionBox()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSelectionBox::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCSelectionBox::nicesize( YUIDimension dim )
{
  wsze sze = ( biglist ) ? pad->tableSize() + 2 : wGetDefsze();
  return dim == YD_HORIZ ? sze.W : sze.H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSelectionBox::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCSelectionBox::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YSelectionBox::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSelectionBox::getCurrentItem
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCSelectionBox::getCurrentItem()
{
  if ( !pad->Lines() )
    return -1;
  return pad->CurPos().L;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSelectionBox::setCurrentItem
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCSelectionBox::setCurrentItem( int index )
{
  pad->ScrlLine( index );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSelectionBox::itemAdded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCSelectionBox::itemAdded( const YCPString& str, int index, bool selected )
{
  vector<NCTableCol*> Items( 1U, 0 );
  Items[0] = new NCTableCol( str );
  pad->Append( Items );
  DrawPad();
  if ( selected )
    pad->ScrlLine( pad->Lines() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSelectionBox::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCSelectionBox::setLabel( const YCPString & nlabel )
{
  YSelectionBox::setLabel( nlabel );
  NCPadWidget::setLabel( NCstring( nlabel ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSelectionBox::CreatePad
//	METHOD TYPE : NCPad *
//
//	DESCRIPTION :
//
NCPad * NCSelectionBox::CreatePad()
{
  wsze    psze( defPadSze() );
  NCPad * npad = new NCTablePad( psze.H, psze.W, *this );
  npad->bkgd( listStyle().item.plain );

  return npad;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSelectionBox::wRecoded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCSelectionBox::wRecoded()
{
  NCPadWidget::wRecoded();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSelectionBox::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCSelectionBox::wHandleInput( int key )
{
  NCursesEvent ret;

  if ( handleInput( key ) )
    return ret;

  switch ( key ) {
  case KEY_SPACE:
  case KEY_RETURN:
    if ( getNotify() )
      ret = NCursesEvent::button;
    break;
  }
  return ret;
}

