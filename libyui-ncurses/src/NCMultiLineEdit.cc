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

   File:       NCMultiLineEdit.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCMultiLineEdit.h"

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::NCMultiLineEdit
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCMultiLineEdit::NCMultiLineEdit( NCWidget * parent, const YWidgetOpt & opt,
				  const YCPString & nlabel,
				  const YCPString & initialText )
    : YMultiLineEdit( opt, nlabel )
    , NCPadWidget( parent )
    , pad( (NCTextPad *&)NCPadWidget::pad )
{
  WIDDBG << endl;
  defsze = wsze( 5, 5 ) + 2;
  setLabel( nlabel );
  setText( initialText );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::~NCMultiLineEdit
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCMultiLineEdit::~NCMultiLineEdit()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCMultiLineEdit::nicesize( YUIDimension dim )
{
  defsze.W = ( 5 > labelWidht() ? 5 : labelWidht() ) + 2;
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiLineEdit::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YMultiLineEdit::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiLineEdit::setLabel( const YCPString & nlabel )
{
  YMultiLineEdit::setLabel( nlabel );
  NCPadWidget::setLabel( NCstring( nlabel ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::setText
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiLineEdit::setText( const YCPString & ntext )
{
  DelPad();
  ctext = ntext;
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::text
//	METHOD TYPE : YCPString
//
//	DESCRIPTION :
//
YCPString NCMultiLineEdit::text()
{
  if ( pad ) {
    ctext = NCstring( pad->getText() );
  }
  return ctext.YCPstr();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiLineEdit::wRedraw()
{
  if ( !win )
    return;

  NCPadWidget::wRedraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCMultiLineEdit::wHandleInput( wint_t key )
{
  NCursesEvent ret;
  handleInput( key );
  if ( getNotify() )
      ret = NCursesEvent::ValueChanged;
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::CreatePad
//	METHOD TYPE : NCPad *
//
//	DESCRIPTION :
//
NCPad * NCMultiLineEdit::CreatePad()
{
  wsze psze( defPadSze() );
  NCPad * npad = new NCTextPad( psze.H, psze.W, *this );
  npad->bkgd( listStyle().item.plain );

  return npad;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::DrawPad
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiLineEdit::DrawPad()
{
  pad->setText( ctext );
}

void NCMultiLineEdit::setInputMaxLength( const YCPInteger & numberOfChars )
{
    int nr = numberOfChars->asInteger()->value();
    pad->setInputMaxLength(nr);
}
