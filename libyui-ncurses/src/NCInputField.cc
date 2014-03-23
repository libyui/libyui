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

   File:       NCInputField.cc

   Author:     Michael Andres <ma@suse.de>

/-*/
#include <climits>


#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCInputField.h"
#include "NCInputText.h"

#include <wctype.h>		// iswalnum()


NCInputField::NCInputField( YWidget * parent,
			    const std::string & nlabel,
			    bool passwordMode,
			    unsigned maxInput,
			    unsigned maxFld )
	: YInputField( parent, nlabel, passwordMode )
	, NCInputText(parent, nlabel, passwordMode, maxInput, maxFld)        
	, fldtype( PLAIN )
	, InputMaxLength( -1 )
{
    yuiDebug() << std::endl;

    if ( maxInputLength &&
	 ( !maxFldLength || maxFldLength > maxInputLength ) )
    {
	maxFldLength = maxInputLength;
    }

    setLabel( nlabel );
}



NCInputField::~NCInputField()
{
    yuiDebug() << std::endl;
}



int NCInputField::preferredWidth()
{
    return NCInputText::preferredWidth();
}



int NCInputField::preferredHeight()
{
    return NCInputText::preferredHeight();
}



void NCInputField::setEnabled( bool do_bv )
{
    NCInputText::setEnabled( do_bv );
    YInputField::setEnabled( do_bv );
}


void NCInputField::setSize( int newwidth, int newheight )
{
    NCInputText::setSize(newwidth, newheight);
}



void NCInputField::setLabel( const std::string & nlabel )
{
    _label  = NCstring( nlabel );
    _label.stripHotkey();
    YInputField::setLabel( nlabel );
    setDefsze();
    Redraw();
}



void NCInputField::setValue( const std::string & ntext )
{
    buffer = NCstring( ntext ).str();

    if ( maxInputLength && buffer.length() > maxInputLength )
    {
	buffer = buffer.erase( maxInputLength );
    }

    fldstart = 0;

    curpos   = buffer.length();
    tUpdate();
}



std::string NCInputField::value( )
{
    NCstring text( buffer );

    return text.Str();
}



void NCInputField::setValidChars( const std::string & validchars )
{
    validChars = NCstring( validchars );
    YInputField::setValidChars( validchars );
}



bool NCInputField::validKey( wint_t key ) const
{
    // private: NCstring validChars;
    const std::wstring vwch( validChars.str() );

    if ( vwch.empty() )
	return true;

    if ( key < 0 || WCHAR_MAX < key )
	return false;

    return( vwch.find(( wchar_t )key ) != std::wstring::npos );
}



NCursesEvent NCInputField::wHandleInput( wint_t key )
{
    NCursesEvent ret = NCursesEvent::none;
    bool   beep   = false;
    bool   update = true;

    switch ( key )
    {
	case '\b': //ctrl-h 
	case 0x7f: //del
	case KEY_BACKSPACE:

	    if ( bufferFull() && curpos == maxCursor() )
	    {
		// if we're on the last char in a full buffer delete this char
		// and not the previous one.
		buffer.erase( curpos, 1 );
	    }
	    else if ( curpos )
	    {
		buffer.erase( --curpos, 1 );
	    }
	    else
	    {
		update = false;
		beep   = true;
	    }

	    break;

	case KEY_DC:

	    if ( curpos < buffer.length() )
	    {
		buffer.erase( curpos, 1 );
	    }
	    else
	    {
		update = false;
		beep   = true;
	    }

	    break;

	case KEY_HOME:

	    if ( curpos )
	    {
		curpos = 0;
	    }
	    else
	    {
		update = false;
		beep   = true;
	    }

	    break;

	case KEY_END:

	    if ( curpos < maxCursor() )
	    {
		curpos = maxCursor();
	    }
	    else
	    {
		update = false;
		beep   = true;
	    }

	    break;

	case KEY_LEFT:

	    if ( curpos )
	    {
		--curpos;
	    }
	    else
	    {
		update = false;
		beep   = true;
	    }

	    break;

	case KEY_RIGHT:

	    if ( curpos < maxCursor() )
	    {
		++curpos;
	    }
	    else
	    {
		update = false;
		beep   = true;
	    }

	    break;

	case KEY_RETURN:
	    update = false;

	    if ( notify() || returnOnReturn_b )
		ret = NCursesEvent::Activated;

	    break;

	case KEY_HOTKEY:
	    update = false;

	    break;

	default:
	    bool is_special = false;

	    if ( key > 0xFFFF )
	    {
		is_special = true;
		key -= 0xFFFF;
	    }

	    if (( !is_special && KEY_MIN < key && KEY_MAX > key )
		||
		!iswprint( key )
		||
		// if we are at limit of input
		( InputMaxLength >= 0 && InputMaxLength <= ( int )buffer.length() ) )
	    {
		update = false;
		beep   = true;
	    }
	    else if ( fldtype == NUMBER )
	    {
		if ( bufferFull() && key != L'+' )
		{
		    update = false;
		    beep   = true;
		}
		else
		{
		    switch ( key )
		    {
			case L'0':
			case L'1':
			case L'2':
			case L'3':
			case L'4':
			case L'5':
			case L'6':
			case L'7':
			case L'8':
			case L'9':

			    if ( curpos || buffer.empty() || buffer[0] != L'-' )
			    {
				buffer.insert( std::wstring::size_type( curpos ), 1, key );

				if ( curpos < maxCursor() )
				    ++curpos;
			    }
			    else
			    {
				update = false;
				beep   = true;
			    }

			    break;

			case L'+':

			    if ( !buffer.empty() && buffer[0] == L'-' )
			    {
				buffer.erase( std::wstring::size_type( 0 ), 1 );

				if ( curpos )
				    --curpos;
			    }
			    else
			    {
				update = false;
			    }

			    break;

			case L'-':

			    if ( buffer.empty() || buffer[0] != L'-' )
			    {
				buffer.insert( std::wstring::size_type( 0 ), 1, L'-' );

				if ( curpos < maxCursor() )
				    ++curpos;
			    }
			    else
			    {
				update = false;
			    }

			    break;

			default:
			    update = false;
			    beep   = true;
			    break;
		    }
		}

	    }
	    else    // PLAIN
	    {

		if ( bufferFull() || !validKey( key ) )
		{
		    update = false;
		    beep   = true;
		}
		else
		{
		    buffer.insert( std::wstring::size_type( curpos ), 1, key );

		    if ( curpos < maxCursor() )
			++curpos;
		}

	    }

	    break;
    }

    if ( update )
    {
	tUpdate();

	if ( notify() )
	    ret = NCursesEvent::ValueChanged;
    }

    if ( beep )
	::beep();

    return ret;

}


void NCInputField::setInputMaxLength( int numberOfChars )
{
    int nr = numberOfChars;

    // if there is more text then the maximum number of chars,
    // truncate the text and update the buffer

    if ( nr >= 0 && ( int )buffer.length() > nr )
    {
	buffer.erase( nr, maxCursor() - nr );
	tUpdate();
	curpos = buffer.length();
    }

    InputMaxLength = nr;

    YInputField::setInputMaxLength( numberOfChars );
}
