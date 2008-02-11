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
NCMultiLineEdit::NCMultiLineEdit( YWidget * parent, const string & nlabel )
    : YMultiLineEdit( parent, nlabel )
    , NCPadWidget( parent )
{
  WIDDBG << endl;
  defsze = wsze( 5, 5 ) + wsze(0,2);
  setLabel( nlabel );
  // initial text isn't an argument any longer
  //setText( initialText );
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
//	METHOD NAME : NCMultiLineEdit::preferredWidth
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCMultiLineEdit::preferredWidth()
{
    defsze.W = ( 5 > labelWidht() ? 5 : labelWidht() ) + 2;
    return wGetDefsze().W;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::preferredHeight
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCMultiLineEdit::preferredHeight()
{
    return wGetDefsze().H;
    //return YMultiLineEdit::defaultVisibleLines();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::setEnabled
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiLineEdit::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YMultiLineEdit::setEnabled( do_bv );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiLineEdit::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiLineEdit::setLabel( const string & nlabel )
{
  YMultiLineEdit::setLabel( nlabel );
  NCPadWidget::setLabel( NCstring( nlabel ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::setValue
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCMultiLineEdit::setValue( const string & ntext )
{
  DelPad();
  ctext = NCstring( ntext );
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCMultiLineEdit::value
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string NCMultiLineEdit::value()
{
  if ( myPad() ) {
    ctext = NCstring( myPad()->getText() );
  }
  return ctext.Str();
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
  if ( notify() )
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
  myPad()->setText( ctext );
}

void NCMultiLineEdit::setInputMaxLength( int numberOfChars )
{
    myPad()->setInputMaxLength( numberOfChars );
}
