/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

   File:       NCMultiLineEdit.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCMultiLineEdit.h"


NCMultiLineEdit::NCMultiLineEdit( YWidget * parent, const std::string & nlabel )
	: YMultiLineEdit( parent, nlabel )
	, NCPadWidget( parent )
{
    // yuiDebug() << std::endl;
    defsze = wsze( 5, 5 ) + wsze( 0, 2 );
    setLabel( nlabel );
}


NCMultiLineEdit::~NCMultiLineEdit()
{
    // yuiDebug() << std::endl;
}


int NCMultiLineEdit::preferredWidth()
{
    defsze.W = ( 5 > labelWidth() ? 5 : labelWidth() ) + 2;
    return wGetDefsze().W;
}


int NCMultiLineEdit::preferredHeight()
{
    return wGetDefsze().H;
    //return YMultiLineEdit::defaultVisibleLines();
}


void NCMultiLineEdit::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YMultiLineEdit::setEnabled( do_bv );
}


void NCMultiLineEdit::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


void NCMultiLineEdit::setLabel( const std::string & nlabel )
{
    YMultiLineEdit::setLabel( nlabel );
    NCPadWidget::setLabel( NCstring( nlabel ) );
}


void NCMultiLineEdit::setValue( const std::string & ntext )
{
    DelPad();
    ctext = NCstring( ntext );
    Redraw();
}


std::string NCMultiLineEdit::value()
{
    if ( myPad() )
    {
	ctext = NCstring( myPad()->getText() );
    }

    return ctext.Str();
}


void NCMultiLineEdit::wRedraw()
{
    if ( !win )
	return;

    NCPadWidget::wRedraw();
}


NCursesEvent NCMultiLineEdit::wHandleInput( wint_t key )
{
    NCursesEvent ret;
    handleInput( key );

    if ( notify() )
	ret = NCursesEvent::ValueChanged;

    return ret;
}


NCPad * NCMultiLineEdit::CreatePad()
{
    wsze psze( defPadSze() );
    NCPad * npad = new NCTextPad( psze.H, psze.W, *this );
    npad->bkgd( listStyle().item.plain );

    return npad;
}


void NCMultiLineEdit::DrawPad()
{
    myPad()->setText( ctext );
}


void NCMultiLineEdit::setInputMaxLength( int numberOfChars )
{
    myPad()->setInputMaxLength( numberOfChars );
    YMultiLineEdit::setInputMaxLength( numberOfChars );
}
