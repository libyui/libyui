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

   File:       NCRichText.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define	 YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCRichText.h"
#include "YNCursesUI.h"
#include "stringutil.h"
#include "stdutil.h"
#include <sstream>

#include "YMenuItem.h"
#include "YApplication.h"

using stdutil::form;


const unsigned NCRichText::listindent = 4;
const wstring	NCRichText::listleveltags( L"@*+o#-%$&" );//

const bool NCRichText::showLinkTarget = false;

std::map<std::wstring, std::wstring> NCRichText::_charentity;



const wstring NCRichText::entityLookup( const std::wstring & val_r )
{
    //strip leading '#', if any
    wstring::size_type hash = val_r.find( L"#", 0 );
    wstring ascii = L"";

    if ( hash != wstring::npos )
    {
	wstring s = val_r.substr( hash + 1 );
	wchar_t *endptr;
	//and try to convert to int
	long int c = std::wcstol( s.c_str(), &endptr, 0 );

	//conversion succeeded

	if ( s.c_str() != endptr )
	{
	    std::wostringstream ws;
	    ws << char( c );
	    ascii = ws.str();
	}
    }

#define REP(l,r) _charentity[l] = r
    if ( _charentity.empty() )
    {
	// initialize replacement for character entities. A value of NULL
	// means do not replace.
	wstring product;
	NCstring::RecodeToWchar( YUI::app()->productName(), "UTF-8", &product );

	REP( L"amp", L"&" );
	REP( L"gt", L">" );
	REP( L"lt", L"<" );
	REP( L"nbsp", L" " );
	REP( L"quot", L"\"" );
	REP( L"product", product );
    }

    std::map<std::wstring, std::wstring>::const_iterator it = _charentity.find( val_r );

    if ( it != _charentity.end() )
    {
	//known entity - already in the map
	return it->second;
    }
    else
    {
	if ( !ascii.empty() )
	{
	    //replace ascii code by character - e.g. #42 -> '*'
	    //and insert into map to remember it
	    REP( val_r, ascii );
	}
    }

    return ascii;

#undef REP
}



/**
 * Filter out the known &...; entities and return the text with entities
 * replaced  
 **/
const wstring NCRichText::filterEntities( const std::wstring & text )
{
    wstring txt = text;
    // filter known '&..;'

    for ( wstring::size_type special = txt.find( L"&" );
	  special != wstring::npos;
	  special = txt.find( L"&", special + 1 ) )
    {
	wstring::size_type colon = txt.find( L";", special + 1 );

	if ( colon == wstring::npos )
	    break;  // no ';'  -> no need to continue

	const wstring repl = entityLookup( txt.substr( special + 1, colon - special - 1 ) );

	if ( !repl.empty()
	     || txt.substr( special + 1, colon - special - 1 ) == L"product" )	// always replace &product;
	{
	    txt.replace( special, colon - special + 1, repl );
	}
	else
	    yuiMilestone() << "porn.bat" << endl;
    }

    return txt;
}


void NCRichText::Anchor::draw( NCPad & pad, const chtype attr, int color )
{
    unsigned l = sline;
    unsigned c = scol;

    while ( l < eline )
    {
	pad.move( l, c );
	pad.chgat( -1, attr, color );
	++l;
	c = 0;
    }

    pad.move( l, c );

    pad.chgat( ecol - c, attr, color );
}


NCRichText::NCRichText( YWidget * parent, const string & ntext,
			bool plainTextMode )
	: YRichText( parent, ntext, plainTextMode )
	, NCPadWidget( parent )
	, text( ntext )
	, plainText( plainTextMode )
	, textwidth( 0 )
	, cl( 0 )
	, cc( 0 )
	, cindent( 0 )
	, atbol( true )
	, preTag( false )
	, Tattr( 0 )
{
    yuiDebug() << endl;
    activeLabelOnly = true;
    setValue( ntext );
}


NCRichText::~NCRichText()
{
    yuiDebug() << endl;
}


int NCRichText::preferredWidth()
{
    return wGetDefsze().W;
}


int NCRichText::preferredHeight()
{
    return wGetDefsze().H;
}


void NCRichText::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YRichText::setEnabled( do_bv );
}


void NCRichText::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


void NCRichText::setLabel( const string & nlabel )
{
    // not implemented: YRichText::setLabel( nlabel );
    NCPadWidget::setLabel( NCstring( nlabel ) );
}


void NCRichText::setValue( const string & ntext )
{
    DelPad();
    text = NCstring( ntext );
    YRichText::setValue( ntext );
    Redraw();
}


void NCRichText::wRedraw()
{
    if ( !win )
	return;

    bool initial = ( !myPad() || !myPad()->Destwin() );

    if ( !( plainText || anchors.empty() ) )
	arm( armed );

    NCPadWidget::wRedraw();

    if ( initial && autoScrollDown() )
    {
	myPad()->ScrlTo( wpos( myPad()->maxy(), 0 ) );
    }

    return;
}


void NCRichText::wRecoded()
{
    DelPad();
    wRedraw();
}


NCursesEvent NCRichText::wHandleInput( wint_t key )
{
    NCursesEvent ret;
    handleInput( key );

    if ( !( plainText || anchors.empty() ) )
    {
	switch ( key )
	{
	    case KEY_SPACE:
	    case KEY_RETURN:

		if ( armed != Anchor::unset )
		{
		    ret = NCursesEvent::menu;
		    string str;
		    NCstring::RecodeFromWchar( anchors[armed].target, "UTF-8", &str );
		    yuiMilestone() << "LINK: " << str << endl;
		    ret.selection = new YMenuItem( str );
		}

		break;
	}
    }

    return ret;
}


NCPad * NCRichText::CreatePad()
{
    wsze psze( defPadSze() );
    textwidth = psze.W;
    NCPad * npad = new NCPad( psze.H, textwidth, *this );
    return npad;
}


void NCRichText::DrawPad()
{
    yuiDebug()
    << "Start: plain mode " << plainText << endl
    << "       padsize " << myPad()->size() << endl
    << "       text length " << text.str().size() << endl;

    myPad()->bkgdset( wStyle().richtext.plain );
    myPad()->clear();

    if ( plainText )
	DrawPlainPad();
    else
	DrawHTMLPad();

    yuiDebug() << "Done" << endl;
}


void NCRichText::DrawPlainPad()
{
    NCtext ftext( text );
    yuiDebug() << "ftext is " << wsze( ftext.Lines(), ftext.Columns() ) << endl;

    AdjustPad( wsze( ftext.Lines(), ftext.Columns() ) );

    cl = 0;

    for ( NCtext::const_iterator line = ftext.begin();
	  line != ftext.end(); ++line, ++cl )
    {
	myPad()->addwstr( cl, 0, ( *line ).str().c_str() );
    }
}


void NCRichText::PadPlainTXT( const wchar_t * osch, const unsigned olen )
{
    wstring wtxt( osch, olen );
    // resolve the entities even in PRE (#71718)
    wtxt = filterEntities( wtxt );
    
    NCstring nctxt( wtxt );
    NCtext ftext( nctxt );
    
    if ( ftext.Columns() > textwidth )
	textwidth = ftext.Columns();

    AdjustPad( wsze( cl + ftext.Lines(), textwidth ) );

    // insert the text
    const wchar_t * sch = wtxt.data();

    while ( *sch )
    {
	if ( *sch != L'\r' ) // skip carriage return 
	{
	    myPad()->addwstr( sch, 1 );	// add one wide chararacter
	    cc += wcwidth( *sch );

	    if ( *sch == L'\n' )
	    {
		PadNL();	// add a new line
	    }
	}
	++sch;
    }
}

//
// DrawHTMLPad tools
//

inline void SkipToken( const wchar_t *& wch )
{
    do
    {
	++wch;
    }
    while ( *wch && *wch != L'>' );

    if ( *wch )
	++wch;
}


static wstring WStoken( L" \n\t\v\r" );


inline void SkipWS( const wchar_t *& wch )
{
    do
    {
	++wch;
    }
    while ( *wch && WStoken.find( *wch ) != wstring::npos );
}


static wstring WDtoken( L" <\n\t\v\r" ); // WS + TokenStart '<'


inline void SkipWord( const wchar_t *& wch )
{
    do
    {
	++wch;
    }
    while ( *wch && WDtoken.find( *wch ) == wstring::npos );
}


inline void SkipPreTXT( const wchar_t *& wch )
{
    wstring wstr( wch, 6 );

    do
    {
	++wch;
	wstr.assign( wch, 6 );
    }
    while ( *wch && wstr != L"</pre>" );
}




void NCRichText::DrawHTMLPad()
{
    yuiDebug() << "Start:" << endl;

    liststack = stack<int>();
    canchor = Anchor();
    anchors.clear();
    armed = Anchor::unset;

    cl = 0;
    cc = 0;
    cindent = 0;
    myPad()->move( cl, cc );
    atbol = true;

    const wchar_t * wch = ( wchar_t * )text.str().data();
    const wchar_t * swch = 0;

    while ( *wch )
    {
	switch ( *wch )
	{
	    case L' ':
	    case L'\t':
	    case L'\n':
	    case L'\v':
	    case L'\r':
		if ( ! preTag )
		{
		    SkipWS( wch );
		    PadWS();
		}
		else
		{
		    if ( *wch != L'\r' )	// skip carriage return
		    {
			myPad()->addwstr( wch, 1 ); // add the wide chararacter
			cc += wcwidth( *wch );
			if ( *wch == '\n' )
			    PadNL();
		    }
		    ++wch;
		}

		break;

	    case L'<':
		if ( !preTag )
		{
		    swch = wch;
		    SkipToken( wch );

		    if ( PadTOKEN( swch, wch ) )
			break;	// strip token
		    else
			wch = swch;		// reset and fall through
		}
	    default:
		swch = wch;

		if ( !preTag )
		{
		    SkipWord( wch );
		    PadTXT( swch, wch - swch );
		}
		else
		{
		    SkipPreTXT( wch );
		    PadPlainTXT( swch, wch - swch );
		    preTag = false;
		    PadNL();	// add new line after pre is closed
		}

		break;
	}
    }

    PadBOL();
    AdjustPad( wsze( cl, textwidth ) );
    yuiDebug() << "Anchors: " << anchors.size() << endl;

    for ( unsigned i = 0; i < anchors.size(); ++i )
    {
	yuiDebug() << form( "  %2d: [%2d,%2d] -> [%2d,%2d]",
			    i,
			    anchors[i].sline, anchors[i].scol,
			    anchors[i].eline, anchors[i].ecol ) << endl;
    }
}


inline void NCRichText::PadNL()
{
    cc = cindent;

    if ( ++cl == ( unsigned )myPad()->height() )
    {
	AdjustPad( wsze( myPad()->height() + defPadSze().H, textwidth ) );
    }

    myPad()->move( cl, cc );

    atbol = true;
}


inline void NCRichText::PadBOL()
{
    if ( !atbol )
	PadNL();
}


inline void NCRichText::PadWS( const bool tab )
{
    if ( atbol )
	return; // no WS at beginning of line

    if ( cc == textwidth )
    {
	PadNL();
    }
    else
    {
	myPad()->addwstr( L" " );
	++cc;
    }
}


inline void NCRichText::PadTXT( const wchar_t * osch, const unsigned olen )
{
    wstring txt( osch, olen );

    txt = filterEntities( txt );

    size_t	len = textWidth( txt );

    if ( !atbol && cc + len > textwidth )
	PadNL();

    // insert the text
    const wchar_t * sch = txt.data();

    while ( *sch )
    {
	myPad()->addwstr( sch, 1 );	// add one wide chararacter
	cc += wcwidth( *sch );
	atbol = false;	// at begin of line = false

	if ( cc >= textwidth )
	{
	    PadNL();	// add a new line
	}

	sch++;
    }
}

/**
 * Get the number of columns needed to print a 'wstring'. Only printable characters
 * are taken into account because others would return -1 (e.g. '\n').
 * Attention: only use textWidth() to calculate space, not for iterating through a text
 * or to get the length of a text (real text length includes new lines).
 */
size_t NCRichText::textWidth( wstring wstr )
{
    size_t len = 0;
    std::wstring::const_iterator wstr_it;	// iterator for wstring

    for ( wstr_it = wstr.begin(); wstr_it != wstr.end() ; ++wstr_it )
    {
	// check whether char is printable
	if ( iswprint( *wstr_it ) )
	     len += wcwidth( *wstr_it );
    }

    return len;
}


/**
 * Set character attributes (e.g. color, font face...)
 **/
inline void NCRichText::PadSetAttr()
{
    const NCstyle::StRichtext & style( wStyle().richtext );
    chtype nbg = style.plain;

    if ( Tattr & T_ANC )
    {
	nbg = style.link;
    }
    else if ( Tattr & T_HEAD )
    {
	nbg = style.title;
    }
    else
    {
	switch ( Tattr & Tfontmask )
	{
	    case T_BOLD:
		nbg = style.B;
		break;

	    case T_IT:
		nbg = style.I;
		break;

	    case T_TT:
		nbg = style.T;
		break;

	    case T_BOLD|T_IT:
		nbg = style.BI;
		break;

	    case T_BOLD|T_TT:
		nbg = style.BT;
		break;

	    case T_IT|T_TT:
		nbg = style.IT;
		break;

	    case T_BOLD|T_IT|T_TT:
		nbg = style.BIT;
		break;
	}
    }

    myPad()->bkgdset( nbg );
}


void NCRichText::PadSetLevel()
{
    cindent = listindent * liststack.size();

    if ( cindent > textwidth / 2 )
	cindent = textwidth / 2;

    if ( atbol )
    {
	cc = cindent;
	myPad()->move( cl, cc );
    }
}


void NCRichText::PadChangeLevel( bool down, int tag )
{
    if ( down )
    {
	if ( liststack.size() )
	    liststack.pop();
    }
    else
    {
	liststack.push( tag );
    }

    PadSetLevel();
}


void NCRichText::openAnchor( wstring args )
{
    canchor.open( cl, cc );

    const wchar_t * ch = ( wchar_t * )args.data();
    const wchar_t * lookupstr = L"href = ";
    const wchar_t * lookup = lookupstr;

    for ( ; *ch && *lookup; ++ch )
    {
	wchar_t c = towlower( *ch );

	switch ( c )
	{
	    case L'\t':
	    case L' ':

		if ( *lookup != L' ' )
		    lookup = lookupstr;

		break;

	    default:
		if ( *lookup == L' ' )
		{
		    ++lookup;

		    if ( !*lookup )
		    {
			// ch is the 1st char after lookupstr
			--ch; // end of loop will ++ch
			break;
		    }
		}

		if ( c == *lookup )
		    ++lookup;
		else
		    lookup = lookupstr;

		break;
	}
    }

    if ( !*lookup )
    {
	const wchar_t * delim = ( *ch == L'"' ) ? L"\"" : L" \t";
	args = ( *ch == L'"' ) ? ++ch : ch;

	wstring::size_type end = args.find_first_of( delim );

	if ( end != wstring::npos )
	    args.erase( end );

	canchor.target = args;
    }
    else
    {
	yuiError() << "No value for 'HREF=' in anchor '" << args << "'" << endl;
    }
}


void NCRichText::closeAnchor()
{
    canchor.close( cl, cc );

    if ( canchor.valid() )
	anchors.push_back( canchor );

    canchor = Anchor();
}


// expect "<[/]value>"
bool NCRichText::PadTOKEN( const wchar_t * sch, const wchar_t *& ech )
{
    // "<[/]value>"
    if ( *sch++ != L'<' || *( ech - 1 ) != L'>' )
	return false;

    // "[/]value>"
    bool endtag = ( *sch == L'/' );

    if ( endtag )
	++sch;

    // "value>"
    if ( ech - sch <= 1 )
	return false;

    wstring value( sch, ech - 1 - sch );

    wstring args;

    wstring::size_type argstart = value.find_first_of( L" \t\n" );

    if ( argstart != wstring::npos )
    {
	args = value.substr( argstart );
	value.erase( argstart );
    }

    for ( unsigned i = 0; i < value.length(); ++i )
    {
	if ( isupper( value[i] ) )
	{
	    value[i] = static_cast<char>( tolower( value[i] ) );
	}
    }

    int leveltag = 0;

    int headinglevel = 0;

    TOKEN token = T_UNKNOWN;

    switch ( value.length() )
    {
	case 1:

	    if      ( value[0] == 'b' )		token = T_BOLD;
	    else if ( value[0] == 'i' )		token = T_IT;
	    else if ( value[0] == 'p' )		token = T_PAR;
	    else if ( value[0] == 'a' )		token = T_ANC;
	    else if ( value[0] == 'u' )		token = T_BOLD;

	    break;

	case 2:
	    if      ( value == L"br" )		token = T_BR;
	    else if ( value == L"em" )		token = T_IT;
	    else if ( value == L"h1" )		{ token = T_HEAD; headinglevel = 1; }
	    else if ( value == L"h2" )		{ token = T_HEAD; headinglevel = 2; }
	    else if ( value == L"h3" )		{ token = T_HEAD; headinglevel = 3; }
	    else if ( value == L"hr" )		token = T_IGNORE;
	    else if ( value == L"li" )		token = T_LI;
	    else if ( value == L"ol" )		{ token = T_LEVEL; leveltag = 1; }
	    else if ( value == L"qt" )		token = T_IGNORE;
	    else if ( value == L"tt" )		token = T_TT;
	    else if ( value == L"ul" )		{ token = T_LEVEL; leveltag = 0; }

	    break;

	case 3:

	    if      ( value == L"big" )		token = T_IGNORE;
	    else if ( value == L"pre" )		token = T_PLAIN;

	    break;

	case 4:
	    if      ( value == L"bold" )	token = T_BOLD;
	    else if ( value == L"code" )	token = T_TT;
	    else if ( value == L"font" )	token = T_IGNORE;

	    break;

	case 5:
	    if      ( value == L"large" )	token = T_IGNORE;
	    else if ( value == L"small" )	token = T_IGNORE;

	    break;

	case 6:
	    if      ( value == L"center" )	token = T_PAR;
	    else if ( value == L"strong" )	token = T_BOLD;

	    break;

	case 10:
	    if ( value == L"blockquote" )	token = T_PAR;

	    break;

	default:
	    token = T_UNKNOWN;

	    break;
    }

    if ( token == T_UNKNOWN )
    {
	yuiDebug() << "T_UNKNOWN :" << value << ":" << args << ":" << endl;
	// see bug #67319
        //  return false;
	return true;
    }

    if ( token == T_IGNORE )
	return true;

    switch ( token )
    {
	case T_LEVEL:
	    PadChangeLevel( endtag, leveltag );
	    PadBOL();
	    // add new line after end of the list

	    if ( endtag )
		PadNL();

	    break;

	case T_BR:
	    PadNL();

	    break;

	case T_HEAD:
	    if ( endtag )
		Tattr &= ~token;
	    else
		Tattr |= token;

	    PadSetAttr();
	    PadBOL();

	    if ( headinglevel && endtag )
		PadNL();

	    break;

	case T_PAR:
	    PadBOL();

	    if ( !cindent )
	    {
		if ( endtag )
		    // add new line after closing tag (FaTE 3124)
		    PadNL();
	    }

	    break;

	case T_LI:
	    PadSetLevel();
	    PadBOL();

	    if ( !endtag )
	    {
		wstring tag;

		if ( liststack.empty() )
		{
		    tag = wstring( listindent, L' ' );
		}
		else
		{
		    wchar_t buf[16];

		    if ( liststack.top() )
		    {
			swprintf( buf, 15, L"%2ld. ", liststack.top()++ );
		    }
		    else
		    {
			swprintf( buf, 15, L" %lc  ", listleveltags[liststack.size()%listleveltags.size()] );
		    }

		    tag = buf;
		}

		// outsent list tag:
		cc = ( tag.size() < cc ? cc - tag.size() : 0 );

		myPad()->move( cl, cc );

		PadTXT( tag.c_str(), tag.size() );

		atbol = true;
	    }

	    break;

	case T_PLAIN:

	    if ( !endtag )
	    {
		preTag = true;	// display plain text
	    }
	    else
	    {
		preTag = false;
	    }

	    break;

	case T_ANC:

	    if ( endtag )
	    {
		closeAnchor();
	    }
	    else
	    {
		openAnchor( args );
	    }

	    // fall through

	case T_BOLD:
	case T_IT:
	case T_TT:
	    if ( endtag )
		Tattr &= ~token;
	    else
		Tattr |= token;

	    PadSetAttr();

	    break;

	case T_IGNORE:
	case T_UNKNOWN:
	    break;
    }

    return true;
}


void NCRichText::arm( unsigned i )
{
    if ( !myPad() )
    {
	armed = i;
	return;
    }

    yuiDebug() << i << " (" << armed << ")" << endl;

    if ( i == armed )
    {
	if ( armed != Anchor::unset )
	{
	    // just redraw
	    anchors[armed].draw( *myPad(), wStyle().richtext.getArmed( GetState() ), 0 );
	    myPad()->update();
	}

	return;
    }

    if ( armed != Anchor::unset )
    {
	anchors[armed].draw( *myPad(), wStyle().richtext.link, ( int ) wStyle().richtext.visitedlink );
	armed = Anchor::unset;
    }

    if ( i != Anchor::unset )
    {
	armed = i;
	anchors[armed].draw( *myPad(), wStyle().richtext.getArmed( GetState() ), 0 );
    }

    if ( showLinkTarget )
    {
	if ( armed != Anchor::unset )
	    NCPadWidget::setLabel( NCstring( anchors[armed].target ) );
	else
	    NCPadWidget::setLabel( NCstring() );
    }
    else
    {
	myPad()->update();
    }
}


void NCRichText::HScroll( unsigned total, unsigned visible, unsigned start )
{
    NCPadWidget::HScroll( total, visible, start );
    // no hyperlink handling needed, because Ritchtext does not HScroll
}


void NCRichText::VScroll( unsigned total, unsigned visible, unsigned start )
{
    NCPadWidget::VScroll( total, visible, start );

    if ( plainText || anchors.empty() )
	return; // <-- no links to check

    // Take care of hyperlinks: Check whether an armed link is visible.
    // If not arm the first visible link on page or none.
    vScrollFirstvisible	 = start;

    vScrollNextinvisible = start + visible;

    if ( armed != Anchor::unset )
    {
	if ( anchors[armed].within( vScrollFirstvisible, vScrollNextinvisible ) )
	    return; // <-- armed link is vissble
	else
	    disarm();
    }

    for ( unsigned i = 0; i < anchors.size(); ++i )
    {
	if ( anchors[i].within( vScrollFirstvisible, vScrollNextinvisible ) )
	{
	    arm( i );
	    break;
	}
    }
}


bool NCRichText::handleInput( wint_t key )
{
    if ( plainText || anchors.empty() )
    {
	return NCPadWidget::handleInput( key );
    }

    // take care of hyperlinks
    bool handled = true;

    switch ( key )
    {
	case KEY_LEFT:
	    // jump to previous link; scroll up if none
	    {
		unsigned newarmed = Anchor::unset;

		if ( armed == Anchor::unset )
		{
		    // look for an anchor above current page
		    for ( unsigned i = anchors.size(); i; )
		    {
			--i;

			if ( anchors[i].eline < vScrollFirstvisible )
			{
			    newarmed = i;
			    break;
			}
		    }
		}
		else if ( armed > 0 )
		{
		    newarmed = armed - 1;
		}

		if ( newarmed == Anchor::unset )
		{
		    handled = NCPadWidget::handleInput( KEY_UP );
		}
		else
		{
		    if ( !anchors[newarmed].within( vScrollFirstvisible, vScrollNextinvisible ) )
			myPad()->ScrlLine( anchors[newarmed].sline );

		    arm( newarmed );
		}
	    }

	    break;

	case KEY_RIGHT:
	    // jump to next link; scroll down if none
	    {
		unsigned newarmed = Anchor::unset;

		if ( armed == Anchor::unset )
		{
		    // look for an anchor below current page
		    for ( unsigned i = 0; i < anchors.size(); ++i )
		    {
			if ( anchors[i].sline >= vScrollNextinvisible )
			{
			    newarmed = i;
			    break;
			}
		    }
		}
		else if ( armed + 1 < anchors.size() )
		{
		    newarmed = armed + 1;
		}

		if ( newarmed == Anchor::unset )
		{
		    handled = NCPadWidget::handleInput( KEY_DOWN );
		}
		else
		{
		    if ( !anchors[newarmed].within( vScrollFirstvisible, vScrollNextinvisible ) )
			myPad()->ScrlLine( anchors[newarmed].sline );

		    arm( newarmed );
		}
	    }

	    break;

	case KEY_UP:
	    // arm previous visible link; scroll up if none

	    if ( armed != Anchor::unset
		 && armed > 0
		 && anchors[armed-1].within( vScrollFirstvisible, vScrollNextinvisible ) )
	    {
		arm( armed - 1 );
	    }
	    else
	    {
		handled = NCPadWidget::handleInput( key );
	    }

	    break;

	case KEY_DOWN:
	    // arm next visible link; scroll down if none

	    if ( armed != Anchor::unset
		 && armed + 1 < anchors.size()
		 && anchors[armed+1].within( vScrollFirstvisible, vScrollNextinvisible ) )
	    {
		arm( armed + 1 );
	    }
	    else
	    {
		handled = NCPadWidget::handleInput( key );
	    }

	    break;

	default:
	    handled = NCPadWidget::handleInput( key );
    };

    return handled;
}


