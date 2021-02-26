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

   File:       NCTextPad.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCTextPad.h"

#include <limits.h>

using std::endl;

// FLAW: if notification is enabled the dialog gets disabled and reenabled
// when procesing the event. This causes noticable flicker if the enabled/disabled
// attriutes differ. That's why 'nonactive' style is used.
#define MY_TEXT_STYLE ( parw.widgetStyle( /*nonactive*/true ).data )

NCTextPad::NCTextPad( int l, int c, const NCWidget & p )
	: NCPad( l, c, p )
	, lines( 1U, 0 )
	, cline( lines.begin() )
	, curson( false )
	, InputMaxLength( -1 )
{
    bkgd( MY_TEXT_STYLE );
}



NCTextPad::~NCTextPad()
{
}



void NCTextPad::resize( wsze nsze )
{
    SetPadSize( nsze ); // might be enlarged by NCPadWidget if redirected

    if ( nsze.H != height()
	 || nsze.W != width() )
    {
	NCursesWindow * odest = Destwin();

	if ( odest )
	    Destwin( 0 );

	NCursesPad::resize( nsze.H, nsze.W );

	if ( odest )
	    Destwin( odest );
    }
}



void NCTextPad::assertSze( wsze minsze )
{
    if ( minsze.W > width()
	 || minsze.H > height() )
	resize( minsze );
}



void NCTextPad::assertWidth( unsigned minw )
{
    if ( minw >= ( unsigned )width() ) // == for the '\n'
	resize( wsze( height(), minw + 10 ) );
}



void NCTextPad::assertHeight( unsigned minh )
{
    if ( minh > ( unsigned )height() )
	resize( wsze( minh + 10, width() ) );
}



wpos NCTextPad::CurPos() const
{
    return curs;
}



void NCTextPad::cursor( bool on )
{
    if ( on != curson )
    {
	if (( curson = on ) )
	{
	    bkgdset( parw.wStyle().cursor );
	    add_attr_char( curs.L, curs.C );
	    bkgdset( MY_TEXT_STYLE );
	}
	else
	{
	    add_attr_char( curs.L, curs.C );
	}
    }
}



int NCTextPad::setpos()
{
    // BUG?: bkgd does not change the color attibute of nonwhite characters
    // on the pad so we repaint them in the new color in case it changed.
    chtype oldbkgd = NCattribute::getColor( getbkgd() );
    bkgd( MY_TEXT_STYLE );

    if ( NCattribute::getColor( getbkgd() ) != oldbkgd )
    {
      // repaint text
      for ( int l = 0; l < height(); ++l )
	for ( int c = 0; c < width(); ++ c )
	{
	  add_attr_char( l, c );
	}
    }
    cursor( parw.GetState() == NC::WSactive );
    return setpos( CurPos() );
}



int NCTextPad::setpos( const wpos & newpos )
{
    wpos npos( newpos.between( 0, wpos( maxy(), maxx() ) ) );

    if (( unsigned )npos.L >= lines.size() )
    {
	npos.L = lines.size() - 1;
	cline = lines.end();
	--cline;
    }
    else if ( npos.L != curs.L )
    {
	advance( cline, npos.L - curs.L );
    }

    if (( unsigned )npos.C > *cline )
    {
	npos.C = *cline;
    }

    bool ocurs = curson;

    if ( ocurs ) cursorOff();

    curs = npos;

    if ( ocurs ) cursorOn();

    wpos padpos( curs );

    if ( drect.Sze > wsze( 0 ) )
    {
	padpos = ( padpos / drect.Sze ) * drect.Sze;
    }

    return NCPad::setpos( padpos );
}



bool NCTextPad::handleInput( wint_t key )
{
    bool handled = true;
    bool beep	 = false;
    bool update  = true;

    cursorOff();

    switch ( key )
    {
	case KEY_LEFT:

	    if ( curs.C )
	    {
		--curs.C;
	    }
	    else if ( curs.L )
	    {
		--cline;
		--curs.L;
		curs.C = ( *cline );
	    }
	    else
	    {
		beep = true;
		update = false;
	    }
	    break;


	case KEY_UP:

	    if ( curs.L )
	    {
		--cline;
		--curs.L;
	    }
	    else
	    {
		beep = true;
		update = false;
	    }
	    break;


	case KEY_RIGHT:

	    if (( unsigned )curs.C < ( *cline ) )
	    {
		++curs.C;
	    }
	    else if (( unsigned )curs.L + 1 < lines.size() )
	    {
		++cline;
		++curs.L;
		curs.C = 0;
	    }
	    else
	    {
		beep = true;
		update = false;
	    }
	    break;


	case KEY_DOWN:

	    if (( unsigned )curs.L + 1 < lines.size() )
	    {
		++cline;
		++curs.L;
	    }
	    else
	    {
		beep = true;
		update = false;
	    }
	    break;


	case KEY_PPAGE:

	    if ( curs.L )
	    {
		setpos( wpos( curs.L - 3, curs.C ) );
	    }
	    else
	    {
		beep = true;
		update = false;
	    }
	    break;


	case KEY_NPAGE:

	    if (( unsigned )curs.L + 1 < lines.size() )
	    {
		setpos( wpos( curs.L + 3, curs.C ) );
	    }
	    else
	    {
		beep = true;
		update = false;
	    }
	    break;


	case KEY_SLEFT:
	case KEY_HOME:

	    if ( curs.C )
	    {
		curs.C = 0;
	    }
	    break;


	case KEY_SRIGHT:
	case KEY_END:

	    if (( unsigned )curs.C < ( *cline ) )
	    {
		curs.C = ( *cline );
	    }
	    break;


	case KEY_BACKSPACE:
	    beep = !delch( true );
	    break;

	case KEY_DC:
	    beep = !delch();
	    break;

	case KEY_HOTKEY:
	    update = false;
	    break;

	default:
	    // if we are at limit of input

	    if ( InputMaxLength >= 0 && InputMaxLength < ( int )getText().length() )
	    {
		beep = true;
		update = false;
	    }
	    else
	    {
		beep = !insert( key );
	    }
	    break;
    }

    cursorOn();

    if ( beep )
	::beep();

    if ( update )
	setpos( curs );

    return handled;
}



bool NCTextPad::insert( wint_t key )
{
    if ( key == 10 )
    {
	return openLine();
    }

    if ( key < 32 || ( key >= 127 && key < 160 ) || UCHAR_MAX < key )
    {
	return false;
    }

    assertWidth( ++( *cline ) );

    cchar_t cchar;
    attr_t attr;
    short int color;
    wattr_get( w, &attr, &color, NULL ); // NOTE: (w)attr_get is not probided by NCursesWindow

    wchar_t wch[2];
    wch[0] = key;
    wch[1] = L'\0';

    setcchar( &cchar, wch, attr, color, NULL );
// libncurses6 enables ext_color from struct cchar_t (see curses.h).
// Set ext_color to 0 to respect the settings got from attr_get (bnc#652240).
#ifdef NCURSES_EXT_COLORS
    cchar.ext_color = 0;
#endif
    ins_wch( curs.L, curs.C++, &cchar );

    return true;
}



bool NCTextPad::openLine()
{
    assertHeight( lines.size() + 1 );
    std::list<unsigned>::iterator newl( cline );
    newl = lines.insert( ++newl, 0 );

    if ( curs.C == 0 )
    {
	// eazy at line begin: new empty line above
	insertln();

	( *newl ) = ( *cline );
	( *cline ) = 0;
    }
    else
    {
	// new empty line below
	move( curs.L + 1, 0 );
	insertln();

	if (( unsigned )curs.C < ( *cline ) )
	{
	    // copy down rest of line
	    ( *newl ) = ( *cline ) - curs.C;
	    ( *cline ) = curs.C;

	    move( curs.L, curs.C );
	    copywin( *this, curs.L, curs.C, curs.L + 1, 0, curs.L + 1, ( *newl ), false );
	    clrtoeol();
	}
    }

    cline = newl;

    ++curs.L;
    curs.C = 0;

    return true;
}



bool NCTextPad::delch( bool previous )
{
    if ( previous )
    {
	if ( curs.C )
	    --curs.C;
	else if ( curs.L )
	{
	    --cline;
	    --curs.L;
	    curs.C = ( *cline );
	}
	else
	    return false;
    }

    if (( unsigned )curs.C < *cline )
    {
	// eazy not at line end
	--( *cline );

	NCPad::delch( curs.L, curs.C );
    }
    else if (( unsigned )curs.L + 1 < lines.size() )
    {
	// at line end: join with next line
	std::list<unsigned>::iterator nextl( cline );
	++nextl;
	( *cline ) += ( *nextl );
	lines.erase( nextl );

	assertWidth(( *cline ) );
	copywin( *this, curs.L + 1, 0, curs.L, curs.C, curs.L, ( *cline ), false );

	move( curs.L + 1, 0 );
	deleteln();
    }
    else
	return false;

    return true;
}


void NCTextPad::setText( const NCtext & ntext )
{
    bkgd( MY_TEXT_STYLE );

    bool ocurs = curson;
    if ( ocurs ) cursorOff();

    clear();
    assertSze( wsze( ntext.Lines(), ntext.Columns() + 1 ) );
    curs = 0;

    cchar_t cchar;
    attr_t attr;
    short int color;
    wattr_get( w, &attr, &color, NULL ); // NOTE: (w)attr_get is not probided by NCursesWindow

    wchar_t wch[2];
    wch[1] = L'\0';
    lines.clear();

    unsigned cl = 0;

    for ( NCtext::const_iterator line = ntext.begin(); line != ntext.end(); ++line )
    {
	lines.push_back(( *line ).str().length() );

	unsigned cc = 0;

	for ( std::wstring::const_iterator c = line->str().begin(); c != line->str().end(); c++ )
	{
	    //replace tabs for right arrows (#66104)
	    if ( *c == 9 ) // horizontal tabulation
	    {
		wch[0] = 8677; // U+21E5 RIGHTWARDS ARROW TO BAR (rightward tab)
	    }
	    else
	    {
		wch[0] = *c;
	    }

	    setcchar( &cchar, wch, attr, color, NULL );
// libncurses6 enables ext_color from struct cchar_t (see curses.h).
// Set ext_color to 0 to respect the settings got from attr_get (bcn#652240).
#ifdef NCURSES_EXT_COLORS
	    cchar.ext_color = 0;
#endif
	    ins_wch( cl, cc++, &cchar );
	}

	cl++;
    }

    if ( lines.empty() )
	lines.push_back( 0U );

    cline = lines.begin();

    if ( ocurs )
	cursorOn();

    setpos( curs );
}



std::wstring NCTextPad::getText() const
{
    // just for inch(x,y) call, which isn't const due to
    // hw cursor movement, but that's of no interest here.
    NCTextPad * myself = const_cast<NCTextPad *>( this );

    cchar_t cchar;
    std::wstring ret;
    unsigned l = 0;
    wchar_t wch[CCHARW_MAX+1];
    attr_t attr;
    short int colorpair;

    for ( std::list<unsigned>::const_iterator cgetl( lines.begin() ); cgetl != lines.end(); ++cgetl )
    {
	for ( unsigned c = 0; c < ( *cgetl ); ++c )
	{
	    myself->in_wchar( l, c, &cchar );
	    getcchar( &cchar, wch, &attr, &colorpair, NULL );

	    //replace right arrow back for horizontal tab - see setText method above (#142509)

	    if ( wch[0] == 8677 )
		wch[0] = 9;

	    ret += wch[0];
	}

	++l;
	// do not append \n after the very last line (bnc #573553)
	if ( l < lines.size() )
	{
	    ret += L"\n";
	}
    }

    return ret;
}


std::ostream & operator<<( std::ostream & STREAM, const NCTextPad & OBJ )
{
    STREAM << "at " << OBJ.CurPos() << " on " << wsze( OBJ.height(), OBJ.width() )
    << " lines " << OBJ.lines.size() << " (" << *OBJ.cline << ")";
    return STREAM;
}

void NCTextPad::setInputMaxLength( int nr )
{
    // if there is more text then the maximum number of chars,
    // truncate the text and update the buffer
    if ( nr >= 0 && nr < ( int )getText().length() )
    {
	NCstring newtext = getText().substr( 0, nr );
	setText( newtext );
    }

    InputMaxLength = nr;
}
