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

   File:       NCMultiSelectionBox.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCMultiSelectionBox.h"


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::NCMultiSelectionBox
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCMultiSelectionBox::NCMultiSelectionBox( NCWidget * parent, const YWidgetOpt & opt,
					  const YCPString & nlabel )
    : YMultiSelectionBox( opt, nlabel )
    , NCPadWidget( parent )
    , pad( (NCTablePad *&)NCPadWidget::pad )
{
  WIDDBG << endl;
  InitPad();
  setLabel( nlabel );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::~NCMultiSelectionBox
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCMultiSelectionBox::~NCMultiSelectionBox()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCMultiSelectionBox::nicesize( YUIDimension dim )
{
  wsze sze = wsze::max( defsze, wsze( 0, labelWidht()+2 ) );
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiSelectionBox::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YMultiSelectionBox::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::getCurrentItem
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCMultiSelectionBox::getCurrentItem()
{
  if ( !pad->Lines() )
    return -1;
  return pad->CurPos().L;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::setCurrentItem
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiSelectionBox::setCurrentItem( int index )
{
  pad->ScrlLine( index );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::itemAdded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiSelectionBox::itemAdded( const YCPString& str, bool selected )
{
  vector<NCTableCol*> Items( 2U, 0 );
  Items[0] = new NCTableTag( selected );
  Items[1] = new NCTableCol( str, NCTableCol::PLAIN );
  pad->Append( Items );
  DrawPad();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::tagCell
//	METHOD TYPE : NCTableTag *
//
//	DESCRIPTION :
//
NCTableTag * NCMultiSelectionBox::tagCell( int index )
{
  NCTableLine * cl = pad->ModifyLine( index );
  if ( !cl )
    return 0;
  return static_cast<NCTableTag *>(cl->GetCol( 0 ));
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::tagCell
//	METHOD TYPE : const NCTableTag *
//
//	DESCRIPTION :
//
const NCTableTag * NCMultiSelectionBox::tagCell( int index ) const
{
  const NCTableLine * cl = pad->GetLine( index );
  if ( !cl )
    return 0;
  return static_cast<const NCTableTag *>(cl->GetCol( 0 ));
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::deleteAllItems
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiSelectionBox::deleteAllItems()
{
  YMultiSelectionBox::deleteAllItems();
  pad->ClearTable();
  DrawPad();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::isItemSelected
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCMultiSelectionBox::isItemSelected( int index ) const
{
  const NCTableTag * citem = tagCell( index );
  return citem ? citem->Selected() : false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::setAllItemsSelected
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiSelectionBox::setAllItemsSelected( bool val )
{
  for ( int i = 0; setItemSelected( i, val, false ); ++i )
    ;
  DrawPad();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::setItemSelected
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCMultiSelectionBox::setItemSelected( int index, bool val, bool update )
{
  NCTableTag * citem = tagCell( index );
  if ( !citem )
    return false;

  if ( citem->Selected() != val ) {
    citem->SetSelected( val );
    if ( update )
      DrawPad();
  }

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::toggleCurrentItem
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiSelectionBox::toggleCurrentItem()
{
  int index = getCurrentItem();
  if ( index == -1 )
    return;

  NCTableTag * citem = tagCell( index );
  if ( !citem )
    return;

  citem->SetSelected( !citem->Selected() );
  DrawPad();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiSelectionBox::setLabel( const YCPString & nlabel )
{
  YMultiSelectionBox::setLabel( nlabel );
  NCPadWidget::setLabel( NCstring( nlabel ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::CreatePad
//	METHOD TYPE : NCPad *
//
//	DESCRIPTION :
//
NCPad * NCMultiSelectionBox::CreatePad()
{
  wsze psze( defPadSze() );
  NCTablePad * npad = new NCTablePad( psze.H, psze.W, *this );
  npad->bkgd( listStyle().item.plain );
  npad->SetSepChar( ' ' );
  return npad;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::wRecoded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiSelectionBox::wRecoded()
{
  NCPadWidget::wRecoded();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiSelectionBox::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCMultiSelectionBox::wHandleInput( wint_t key )
{
  NCursesEvent ret;
  bool valueChanged = false;
  int oldCurrentItem = getCurrentItem();

  if ( ! handleInput( key ) )
  {
    switch ( key ) {
    case KEY_SPACE:
    case KEY_RETURN:
      toggleCurrentItem();
      valueChanged = true;
      break;
    case '+':
      if ( !itemIsSelected( getCurrentItem() ) ) {
        setItemSelected( getCurrentItem(), true, false );
        valueChanged = true;
      }
      pad->ScrlDown();
      break;
    case '-':
      if ( itemIsSelected( getCurrentItem() ) ) {
        setItemSelected( getCurrentItem(), false, false );
        valueChanged = true;
      }
      pad->ScrlDown();
      break;
    }
  }

  if ( getNotify() )
  {
    if ( valueChanged )
      ret = NCursesEvent::ValueChanged;
    else if ( oldCurrentItem != getCurrentItem() )
      ret = NCursesEvent::SelectionChanged;
  }

  return ret;
}

