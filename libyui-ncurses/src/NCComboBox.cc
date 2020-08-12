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

   File:       NCComboBox.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#include <climits>

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCComboBox.h"
#include "NCPopupList.h"


NCComboBox::NCComboBox( YWidget * parent, const std::string & nlabel,
			bool editable )
	: YComboBox( parent, nlabel, editable )
	, NCWidget( parent )
	, mayedit( editable )
	, privText( "" )
	, lwin( 0 )
	, twin( 0 )
	, fldstart( 0 )
	, fldlength( 0 )
	, curpos( 0 )
	, longest_line( 10 )
	, index( -1 )
	, InputMaxLength( -1 )
{
    // yuiDebug() << std::endl;
    setLabel( nlabel );
    hotlabel = &label;
    setText( "" );
}


NCComboBox::~NCComboBox()
{
    delete lwin;
    delete twin;
    // yuiDebug() << std::endl;
}


int NCComboBox::preferredWidth()
{
    return wGetDefsze().W;
}


int NCComboBox::preferredHeight()
{
    return wGetDefsze().H;
}


void NCComboBox::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YComboBox::setEnabled( do_bv );
}


void NCComboBox::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


void NCComboBox::setDefsze()
{
    // Height: label h. + 1 (text area)
    // Width: longest line + 2 chars ( arrow(s) )
    // (here, we should not rely on label width only as text area may become
    // unreasonably small then - #367083 )
    defsze = wsze( label.height() + 1,
		   ( label.width() > longest_line ) ? label.width() : longest_line + 2 );
}


void NCComboBox::wCreate( const wrect & newrect )
{
    NCWidget::wCreate( newrect );

    if ( !win )
	return;

    wrect lrect( 0, wsze::min( newrect.Sze,
			       wsze( label.height(), newrect.Sze.W ) ) );

    wrect trect( 0, wsze( 1, newrect.Sze.W ) );

    if ( lrect.Sze.H == newrect.Sze.H )
	lrect.Sze.H -= 1;

    trect.Pos.L = lrect.Sze.H > 0 ? lrect.Sze.H : 0;

    lwin = new NCursesWindow( *win,
			      lrect.Sze.H, lrect.Sze.W,
			      lrect.Pos.L, lrect.Pos.C,
			      'r' );

    twin = new NCursesWindow( *win,
			      trect.Sze.H, trect.Sze.W,
			      trect.Pos.L, trect.Pos.C,
			      'r' );

    fldlength = trect.Sze.W ? trect.Sze.W - 1 : 0;
}


void NCComboBox::wDelete()
{
    delete lwin;
    delete twin;
    lwin = 0;
    twin = 0;
    NCWidget::wDelete();
}


void NCComboBox::addItem( YItem * item )
{
    if ( item )
    {
	YComboBox::addItem( item );

	deflist.push_back( item->label() );
	std::string::size_type this_line = item->label().size();

	//Is this line longer than the longest one so far?
	//(but no greater than 40 chars, we may have only 80x25 screen)

	if (( this_line > longest_line ) && ( this_line <= 40 ) )
	{
	    //yes, so let's resize the text area)
	    longest_line = this_line;
	    setDefsze();
	}

	if ( item->selected() )
	{
	    index = item->index();
	    setText( item->label() );
	}
    }
}


void NCComboBox::addItem( const std::string & label, bool selected )
{
    YItem * newItem = new YItem( label, selected );
    YUI_CHECK_NEW( newItem );

    addItem( newItem );

}


void NCComboBox::setLabel( const std::string & nlabel )
{
    label = NCstring( nlabel );
    label.stripHotkey();
    setDefsze();
    YComboBox::setLabel( nlabel );
    Redraw();
}


void NCComboBox::setCurrentItem( int nindex )
{
    int idx = 0;
    std::list<std::string>::iterator entry;

    for ( entry = deflist.begin(); entry != deflist.end(); ++entry, ++idx )
    {
	if ( idx == nindex )
	{
	    std::string strip = *entry;
	    std::string::size_type h = strip.find( '&' );

	    if ( h != std::string::npos )
		strip.erase( h, 1 );

	    setText( strip );

	    index = idx;
	    break;
	}
    }

    Redraw();
}


int NCComboBox::getCurrentItem() const
{
    return index;
}


void NCComboBox::setText( const std::string & ntext )
{
    privText = NCstring( ntext );
    buffer   = privText.str();
    modified = false;
    fldstart = 0;
    curpos   = mayedit ? buffer.length() : 0;

    // (Maybe) no need to set default size here, it has been
    // alread calculated as the items were added (see addItem() above)
    // setDefsze();

    tUpdate();
    Redraw();
}

void NCComboBox::selectItem( YItem * item, bool selected )
{
    if ( item )
    {
	YComboBox::selectItem( item, selected );

	if ( selected )
	    index = item->index();
    }
}

std::string NCComboBox::text()
{
    if ( modified )
	return NCstring( buffer ).Str();

    return privText.Str();
}


void NCComboBox::setValidChars( const std::string & validchars )
{
    validChars = NCstring( validchars );
    YComboBox::setValidChars( validchars );
}


bool NCComboBox::validKey( wint_t key ) const
{
    const std::wstring vwch( validChars.str() );

    if ( vwch.empty() )		// usually empty -> return true
	return true;

    if ( key < 0 || WCHAR_MAX < key )
	return false;

    return( vwch.find(( wchar_t )key ) != std::wstring::npos );
}


void NCComboBox::wRecoded()
{
    if ( modified )
    {
	privText = NCstring( buffer );
	modified = false;
    }

    buffer = privText.str();

    wRedraw();
}


void NCComboBox::wRedraw()
{
    if ( !win )
	return;

    // label
    const NCstyle::StWidget & style( widgetStyle( true ) );

    lwin->bkgd( style.plain );

    lwin->clear();

    label.drawAt( *lwin, style );

    tUpdate();
}


void NCComboBox::tUpdate()
{
    if ( !win )
	return;

    const std::wstring & str( buffer );

    if ( curpos > str.length() )
    {
	curpos = str.length();
    }

    // adjust fldstart that cursor is visible
    if ( str.length() >= fldlength )
    {
	if ( curpos <= fldstart )
	{
	    fldstart = curpos ? curpos - 1 : 0;
	}

	if ( curpos >= fldstart + fldlength - 1 )
	{
	    fldstart = curpos + ( curpos == str.length() ? 1 : 2 ) - fldlength;
	}
    }
    else if ( fldstart )
    {
	fldstart = 0;
    }

    const NCstyle::StWidget & style( widgetStyle() );

    twin->bkgd( widgetStyle( true ).plain );

    twin->move( 0, 0 );

    bool utf8 = haveUtf8();

    if ( fldlength )
    {
	unsigned i	= 0;
	unsigned end	= fldlength;
	const wchar_t * cp = str.data() + fldstart;

	// draw left scrollhint if

	if ( *cp && fldstart )
	{
	    twin->bkgdset( style.scrl );
	    utf8 ?
	    twin->add_wch( WACS_LARROW )
	    : twin->addch( ACS_LARROW );
	    ++i;
	    ++cp;
	}

	// check for right scrollhint
	if ( fldstart + fldlength <= str.length() )
	{
	    --end;
	}

	// draw field
	twin->bkgdset( style.data );

	for ( /*adjusted i*/; *cp && i < end; ++i )
	{
	    twin->addwstr( cp, 1 );
	    cp++;
	}

	twin->bkgdset( style.plain );

	for ( /*adjusted i*/; i < end; ++i )
	{
	    twin->addch( ACS_CKBOARD );
	}

	// draw right scrollhints
	twin->bkgdset( style.scrl );

	if ( end < fldlength )
	{
	    utf8 ?
	    twin->add_wch( WACS_RARROW )
	    : twin->addch( ACS_RARROW );
	}
    }

    utf8 ?

    twin->add_wch( 0, twin->maxx(), WACS_DARROW )
    : twin->addch( 0, twin->maxx(), ACS_DARROW );

    if ( mayedit && GetState() == NC::WSactive )
    {
	twin->move( 0, curpos - fldstart );
	twin->bkgdset( wStyle().cursor );

	if ( curpos < buffer.length() )
	    twin->add_attr_char();
	else
	    twin->addch( ACS_CKBOARD );
    }
}


NCursesEvent NCComboBox::wHandleInput( wint_t key )
{
    NCursesEvent ret;
    bool   beep   = false;
    bool   update = true;
    std::wstring oval = buffer;

    switch ( key )
    {
	case KEY_BACKSPACE:

	    if ( mayedit && curpos )
	    {
		buffer.erase( --curpos, 1 );
		modified = true;
	    }
	    else
	    {
		update = false;
		beep   = true;
	    }

	    break;

	case KEY_DC:

	    if ( mayedit && curpos < buffer.length() )
	    {
		buffer.erase( curpos, 1 );
		modified = true;
	    }
	    else
	    {
		update = false;
		beep   = true;
	    }

	    break;

	case KEY_SLEFT:
	case KEY_HOME:

	    if ( curpos && ( mayedit || fldstart ) )
	    {
		curpos = 0;
	    }
	    else
	    {
		update = false;
		beep   = true;
	    }

	    break;

	case KEY_SRIGHT:
	case KEY_END:

	    if ( curpos < buffer.length() && ( mayedit || fldstart + fldlength <= buffer.length() ) )
	    {
		curpos = buffer.length();
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
		if ( mayedit )
		    --curpos;
		else if ( fldstart )
		    curpos = fldstart - 1;
		else
		{
		    update = false;
		    beep   = true;
		}
	    }
	    else
	    {
		update = false;
		beep   = true;
	    }

	    break;

	case KEY_RIGHT:

	    if ( mayedit && curpos < buffer.length() )
	    {
		++curpos;
	    }
	    else if ( fldstart + fldlength <= buffer.length() )
	    {
		curpos = fldstart + fldlength;
	    }
	    else
	    {
		update = false;
		beep   = true;
	    }

	    break;

	case KEY_HOTKEY:

	    if ( mayedit )
		break;

	    // else fallthrough

	case KEY_DOWN:
	    listPopup();

	    break;

	default:
	    bool is_special = false;

	    if ( key > 0xFFFF )
	    {
		is_special = true;
		key -= 0xFFFF;
	    }

	    if ( !mayedit || !validKey( key )
		 ||
		 ( !is_special && KEY_MIN < key && KEY_MAX > key )
		 ||
		 !iswprint( key )
		 ||
		 // if we are at limit of input
		 ( InputMaxLength >= 0 && InputMaxLength <= ( int )buffer.length() ) )
	    {
		update = false;
		beep   = true;
	    }
	    else
	    {
		buffer.insert( curpos, 1, key );
		modified = true;
		++curpos;
	    }

	    break;
    }

    if ( update )
	tUpdate();

    if ( beep )
	::beep();

    //if ( notify() && oval != buffer )
    // to be conform to qt UI send event even if value hasn't changed
    if ( notify() )
	ret = NCursesEvent::ValueChanged;

    return ret;

}


int NCComboBox::listPopup()
{
    int idx = -1;

    if ( !deflist.empty() )
    {
	// add fix heigth of 2 (dont't use win->height() because win might be invalid, bnc#931154)
	wpos	    at( ScreenPos() + wpos( 2, -1 ) );
	NCPopupList * dialog = new NCPopupList( at, "", deflist, index );
	YUI_CHECK_NEW( dialog );
	idx = dialog->post();

	if ( idx != -1 )
	    setCurrentItem( idx );

	YDialog::deleteTopmostDialog();
    }

    return idx;
}


void NCComboBox::deleteAllItems()
{
    YComboBox::deleteAllItems();
    deflist.clear();
    setText( "" );
}


void NCComboBox::setInputMaxLength( int nr )
{
    // if there is more text then the maximum number of chars,
    // truncate the text and update the buffer
    if ( nr >= 0 && ( int )buffer.length() > nr )
    {
	buffer.erase( nr, buffer.length() - nr );
	tUpdate();
	curpos = buffer.length();
    }

    YComboBox::setInputMaxLength( nr );
}
