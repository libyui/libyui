/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCtext.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCtext.h"
#include "stringutil.h"

#include <wchar.h>		// wcwidth
#include <langinfo.h>


const NCstring NCtext::emptyStr;




NCtext::NCtext( const NCstring & nstr )
{
    lset( nstr );
}



NCtext::NCtext( const NCstring & nstr, size_t columns )
{
    lbrset( nstr, columns );
}



NCtext::~NCtext()
{}



void NCtext::lset( const NCstring & ntext )
{
    // FIXME: rewrite this function so one understands it

    mtext.clear();
    mtext.push_back( "" );

    if ( ntext.str().empty() )
	return;

    const wstring & text( ntext.str() );

    wstring::size_type spos = 0;

    wstring::size_type cpos = wstring::npos;

    bool sawnl = false;		// saw new line

    while (( cpos = text.find( L'\n', spos ) ) != wstring::npos )
    {
	if ( sawnl )
	    mtext.push_back( "" );

	mtext.back() = NCstring( mtext.back().str() + text.substr( spos, cpos - spos ) );

	sawnl = true;

	spos = cpos + 1;
    }

    if ( spos < text.size() )
    {
	if ( sawnl )
	    mtext.push_back( "" );

	mtext.back() = NCstring( mtext.back().str() + text.substr( spos ) );
    }
}



void NCtext::lbrset( const NCstring & ntext, size_t columns )
{
    mtext.clear();

    if ( ntext.str().empty() )
	return;

    const wstring & text( ntext.str() );

    wstring::size_type spos = 0;

    wstring::size_type cpos = wstring::npos;

    cpos = text.find( L'\n', spos );

    while ( cpos != wstring::npos )
    {
	wstring line = text.substr( spos, cpos - spos );

	if ( line.size() <= columns )
	{
	    mtext.push_back( NCstring( line ) );
	}
	else
	{
	    size_t start = columns;
	    mtext.push_back( NCstring( line.substr( 0, columns ) ) );

	    while ( start < line.size() )
	    {
		yuiDebug() << "Add: " << line.substr( start, columns ) << endl;
		mtext.push_back( NCstring( L'~' + line.substr( start, columns - 1 ) ) );
		start += columns - 1;
	    }
	}

	spos = cpos + 1;

	cpos = text.find( L'\n', spos );
    }

    if ( spos < text.size() )
    {
	mtext.push_back( NCstring( text.substr( spos ) ) );
    }
}



unsigned NCtext::Lines() const
{
    if ( mtext.size() == 1 && mtext.front().Str() == "" )
    {
	return 0;
    }
    else
	return mtext.size();
}



void NCtext::append( const NCstring &line )
{
    mtext.push_back( line );
}



size_t NCtext::Columns() const
{
    size_t llen = 0;		// longest line
    size_t tmp_len = 0;		// width of current line

    const_iterator line;		// iterator for list <NCstring> mtext
    std::wstring::const_iterator wstr_it;	// iterator for wstring

    for ( line = mtext.begin(); line != mtext.end(); ++line )
    {
	tmp_len = 0;

	for ( wstr_it = ( *line ).str().begin(); wstr_it != ( *line ).str().end() ; ++wstr_it )
	{
	    tmp_len += wcwidth( *wstr_it );
	}

	if ( tmp_len > llen )
	    llen = tmp_len;
    }

    return llen;
}



const NCstring & NCtext::operator[]( wstring::size_type idx ) const
{
    if ( idx >= Lines() )
	return emptyStr;

    const_iterator line = mtext.begin();

    for ( ; idx; --idx, ++line )
	;

    return *line;
}


ostream & operator<<( ostream & STREAM, const NCtext & OBJ )
{
    return STREAM << "[Text:" << OBJ.Lines() << ',' << OBJ.Columns() << ']';
}





void NClabel::stripHotkey()
{
    hotline = wstring::npos;
    unsigned lineno = 0;

    for ( iterator line = mtext.begin(); line != mtext.end(); ++line, ++lineno )
    {
	line->getHotkey();

	if ( line->hotpos() != wstring::npos )
	{
	    hotline = lineno;
	    break;
	}
    }
}



void NClabel::drawAt( NCursesWindow & w, chtype style, chtype hotstyle,
		      const wrect & dim,
		      const NC::ADJUST adjust,
		      bool fillup ) const
{
    wrect area( dim.intersectRelTo( w.area() ) );

    if ( area.Sze > 0 )
    {
	unsigned maxlen = area.Sze.W;
	unsigned len	= ( width() < maxlen ) ? width() : maxlen;
	unsigned pre	= 0;
	unsigned post	= 0;

	if ( len < maxlen )
	{
	    unsigned dist = maxlen - len;

	    if ( adjust & NC::LEFT )
		pre = 0;
	    else if ( adjust & NC::RIGHT )
		pre = dist;
	    else
		pre = dist / 2;

	    post = dist - pre;
	}

	int l		= area.Pos.L;

	int maxl	= area.Pos.L + area.Sze.H;
	unsigned lineno = 0;

	chtype obg = w.getbkgd();
	w.bkgdset( style );

	for ( NCtext::const_iterator line = begin();
	      line != end() && l < maxl;
	      ++line, ++l, ++lineno )
	{
	    if ( pre && fillup )
	    {
		w.move( l, area.Pos.C );
		w.addwstr( wstring( pre, L' ' ).c_str() );
	    }
	    else
	    {
		w.move( l, area.Pos.C + pre );
	    }

	    // yuiDebug() << "TERMINAL: " << NCstring::terminalEncoding() << " CODESET: " << nl_langinfo( CODESET) << endl;
	    if ( len )
	    {
		if ( NCstring::terminalEncoding() != "UTF-8" )
		{
		    string out;
		    bool ok = NCstring::RecodeFromWchar(( *line ).str(), NCstring::terminalEncoding(), &out );

		    if ( ok )
		    {
			w.printw( "%-*.*s", len, ( int )len, out.c_str() );
		    }
		}
		else
		{
		    // TODO formatting (like above) needed ?
		    w.printw( "%ls", ( *line ).str().c_str() );
		}
	    }

	    if ( post && fillup )
	    {
		w.addwstr( wstring( post, L' ' ).c_str() );
	    }

	    if ( lineno == hotline && hotstyle && pre + hotpos() < maxlen )
	    {
		w.bkgdset( hotstyle );

		w.add_attr_char( l, area.Pos.C + pre + hotpos() );

		w.bkgdset( style );
	    }

	}

	if ( fillup )
	{
	    for ( ; l < maxl; ++l )
	    {
		w.printw( l, area.Pos.C, "%-*.*s", area.Sze.W, area.Sze.W, "" );
	    }
	}

	w.bkgdset( obg );
    }
}


ostream & operator<<( ostream & STREAM, const NClabel & OBJ )
{
    STREAM << "[label" << OBJ.size() << ':' << OBJ[0].str();

    if ( OBJ.hasHotkey() )
	STREAM << ':' << OBJ.hotkey() << " at " << OBJ.hotpos();

    return STREAM  << ']';
}

