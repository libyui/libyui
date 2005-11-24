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

   File:       NCRichText.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include "Y2Log.h"
#include "NCRichText.h"
#include "YNCursesUI.h"
#include "stringutil.h"

#if 0
#undef  DBG_CLASS
#define DBG_CLASS "_NCRichText_"
#endif

///////////////////////////////////////////////////////////////////

const unsigned NCRichText::listindent = 4;
const wstring   NCRichText::listleveltags( L"@*+o#-%$&" );//

const bool NCRichText::showLinkTarget = false;

///////////////////////////////////////////////////////////////////

std::map<std::wstring, std::wstring> NCRichText::_charentity;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::entityLookup
//	METHOD TYPE : const char *
//
//	DESCRIPTION :
//
const wstring NCRichText::entityLookup( const std::wstring & val_r )
{
  if ( _charentity.empty() ) {
    // initialize replacement for character entities. A value of NULL
    // means do not replace.
    wstring product;
    NCstring::RecodeToWchar( YNCursesUI::ui()->productName(), "UTF-8", &product);

#define REP(l,r) _charentity[l] = r
    REP(L"amp",	L"&");
    REP(L"gt",	L">");
    REP(L"lt",	L"<");
    REP(L"nbsp", L" ");
    REP(L"quot", L"\"");
    REP(L"product", product);
#undef REP
  }

  std::map<std::wstring, std::wstring>::const_iterator it = _charentity.find( val_r );
  if ( it != _charentity.end() ) {
    return it->second;
  }

  return L"";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::filterEntities
//	METHOD TYPE : const wstring
//
//	DESCRIPTION : Filter out the known &...; entities, return
//                    the text with entities replaced
//
const wstring NCRichText::filterEntities( const std::wstring & text )
{
    wstring txt = text;
    // filter known '&..;'
    for( wstring::size_type special = txt.find( L"&" );
	special != wstring::npos;
	special = txt.find( L"&", special+1 ) )
	{
	    wstring::size_type colon = txt.find( L";", special+1 );

	    if ( colon == wstring::npos )
    	        break;  // no ';'  -> no need to continue

	    const wstring repl = entityLookup( txt.substr( special+1, colon-special-1 ) );
	    if ( !repl.empty() )
	    {
		txt.replace( special, colon-special+1, repl );
	    }
	}
    return txt;
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::Anchor::draw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::Anchor::draw( NCPad & pad, const chtype attr, int color )
{
  unsigned l = sline;
  unsigned c = scol;
  while ( l < eline ) {
    pad.move( l, c );
    pad.chgat( -1, attr, color );
    ++l;
    c = 0;
  }
  pad.move( l, c );
  pad.chgat( ecol - c, attr, color );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::NCRichText
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCRichText::NCRichText( NCWidget * parent, const YWidgetOpt & opt,
			const YCPString & ntext )
    : YRichText( opt, ntext )
    , NCPadWidget( parent )
    , text( ntext )
    , plainText( opt.plainTextMode.value() )
    , preTag( false )
    , Tattr( 0 )
{
  WIDDBG << endl;
  activeLabelOnly = true;
  setText( ntext );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::~NCRichText
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCRichText::~NCRichText()
{
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCRichText::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YRichText::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::setLabel( const YCPString & nlabel )
{
  // not implemented: YRichText::setLabel( nlabel );
  NCPadWidget::setLabel( NCstring( nlabel ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::setText
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::setText( const YCPString & ntext )
{
  DelPad();
  text = ntext;
  YRichText::setText( ntext );
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::wRedraw()
{
  if ( !win )
    return;

  bool initial = ( !myPad() || !myPad()->Destwin() );

  if ( !( plainText || anchors.empty() ) )
    arm( armed );
  NCPadWidget::wRedraw();

  if ( initial && autoScrollDown ) {
    myPad()->ScrlTo( wpos( myPad()->maxy(), 0 ) );
  }
  return;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::wRecoded
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::wRecoded()
{
  DelPad();
  wRedraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCRichText::wHandleInput( wint_t key )
{
  NCursesEvent ret;
  handleInput( key );

  if ( !( plainText || anchors.empty() ) ) {
    switch ( key ) {
    case KEY_SPACE:
    case KEY_RETURN:
      if ( armed != Anchor::unset ) {
	ret = NCursesEvent::menu;
	string ycpstr;
	NCstring::RecodeFromWchar( anchors[armed].target, "UTF-8", &ycpstr );
	NCMIL << "LINK: " << ycpstr << endl;
	ret.selection = YCPString( ycpstr );
      }
      break;
    }
  }

  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::CreatePad
//	METHOD TYPE : NCPad *
//
//	DESCRIPTION :
//
NCPad * NCRichText::CreatePad()
{
  wsze psze( defPadSze() );
  textwidth = psze.W;
  NCPad * npad = new NCPad( psze.H, textwidth, *this );
  return npad;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::DrawPad
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::DrawPad()
{
  MDBG
    << "Start: plain mode " << plainText << endl
    << "       padsize " << myPad()->size() << endl
    << "       text length " << text.str().size() << endl;

  myPad()->bkgdset( wStyle().richtext.plain );
  myPad()->clear();

  if ( plainText )
    DrawPlainPad();
  else
    DrawHTMLPad();

  MDBG << "Done" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::DrawPlainPad
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::DrawPlainPad()
{
  NCtext ftext( text );
  MDBG << "ftext is " << wsze( ftext.Lines(), ftext.Columns() ) << endl;

  AdjustPad( wsze( ftext.Lines(), ftext.Columns() ) );

  cl = 0;
  for ( NCtext::const_iterator line = ftext.begin();
	line != ftext.end(); ++line, ++cl ) {
    myPad()->addwstr( cl, 0, (*line).str().c_str() );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::PadPlainTXT
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::PadPlainTXT( const wchar_t * osch, const unsigned olen )
{
    wstring wtxt( osch, olen );
    NCstring nctxt( wtxt );
    NCtext ftext( nctxt );

    if ( ftext.Columns() > textwidth )
	textwidth = ftext.Columns();

    AdjustPad( wsze( cl+ftext.Lines(), textwidth ) );

    // insert the text
    const wchar_t * sch = wtxt.data();
    while ( *sch )
    {
	myPad()->addwstr( sch, 1 );	// add one wide chararacter
	cc += wcwidth(*sch);
	
	if ( *sch == L'\n' )
	{
	    PadNL();	// add a new line
	}
	++sch;
    }
}


/******************************************************************
**
**
**	FUNCTION NAME : Skip...
**	FUNCTION TYPE : void
**
**	DESCRIPTION : DrawHTMLPad tools
*/
inline void SkipToken( const wchar_t *& wch )
{
  do {
    ++wch;
  } while ( *wch && *wch != L'>' );
  if ( *wch )
    ++wch;
}

static wstring WStoken( L" \n\t\v\r" );
inline void SkipWS( const wchar_t *& wch ) {
  do {
    ++wch;
  } while ( *wch && WStoken.find( *wch ) != wstring::npos );
}

static wstring WDtoken( L" <\n\t\v\r" ); // WS + TokenStart '<'
inline void SkipWord( const wchar_t *& wch ) {
  do {
    ++wch;
  } while ( *wch && WDtoken.find( *wch ) == wstring::npos );
}

inline void SkipPreTXT( const wchar_t *& wch ) {
    wstring wstr( wch, 6 );
    do {
	++wch;
	wstr.assign( wch, 6 );
    } while ( *wch && wstr != L"</pre>" ); 
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::DrawHTMLPad
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::DrawHTMLPad()
{
  MDBG << "Start:" << endl;

  liststack = stack<int>();
  canchor = Anchor();
  anchors.clear();
  armed = Anchor::unset;

  cl = 0;
  cc = 0;
  cindent = 0;
  myPad()->move( cl, cc );
  atbol = true;

  const wchar_t * wch = (wchar_t *)text.str().data(); 
  const wchar_t * swch = 0;
  
  while ( *wch )
  {
      switch ( *wch ) {
	  case L' ':
	  case L'\t':
	  case L'\n':
	  case L'\v':
	  case L'\r':
	      if ( !preTag )
	      {
		  SkipWS( wch );
		  PadWS();
	      }
	      else
	      {
		  myPad()->addwstr( wch, 1 );	// add one wide chararacter
		  ++cc;
		  ++wch;
	      }
	      break;

	  case L'<':
	      swch = wch;
	      SkipToken( wch );
	      if ( PadTOKEN( swch, wch ) )
		  break;    	// strip token
	      else
		  wch = swch; 	// reset and fall through

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
		  
		  // resolve the entities even in PRE (#71718)
		  wstring txt = filterEntities( wstring(swch, wch-swch) );

		  PadPlainTXT( txt.c_str (), textWidth( txt ) );
	      }
	      break;
      }
  }

  PadBOL();
  AdjustPad( wsze( cl, textwidth ) );
  
  MDBG << "Anchors: " << anchors.size() << endl;
  for ( unsigned i = 0; i < anchors.size(); ++i ) {
    MDBG << form( "  %2d: [%2d,%2d] -> [%2d,%2d]",
		  i,
		  anchors[i].sline, anchors[i].scol,
		  anchors[i].eline, anchors[i].ecol ) << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::PadNL
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
inline void NCRichText::PadNL()
{
  cc = cindent;
  if ( ++cl == (unsigned)myPad()->height() ) {
    AdjustPad( wsze( myPad()->height() + defPadSze().H, textwidth ) );
  }
  myPad()->move( cl, cc );
  atbol = true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::PadBOL
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
inline void NCRichText::PadBOL()
{
  if ( !atbol )
    PadNL();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::PadWS
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::PadTXT
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
inline void NCRichText::PadTXT( const wchar_t * osch, const unsigned olen )
{
  wstring txt( osch, olen );

  txt = filterEntities (txt);

  size_t	len = textWidth( txt );

  if ( !atbol && cc + len > textwidth )
    PadNL();

  // insert the text
  const wchar_t * sch = txt.data();

  while ( *sch )
  {
      myPad()->addwstr( sch, 1 );	// add one wide chararacter
      cc += wcwidth(*sch);
      atbol = false;	// at begin of line = false
      if ( cc >= textwidth )
      {
	  PadNL();	// add a new line
      }
      sch++;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::textWidth
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
size_t NCRichText::textWidth( wstring wstr )
{
    size_t len = 0;
    std::wstring::const_iterator wstr_it;	// iterator for wstring
      
    for ( wstr_it = wstr.begin(); wstr_it != wstr.end() ; ++wstr_it )
    {
	len += wcwidth( *wstr_it );
    }
    return len;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::PadSetAttr
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
inline void NCRichText::PadSetAttr()
{
  const NCstyle::StRichtext & style( wStyle().richtext );
  chtype nbg = style.plain;
  if ( Tattr & T_ANC ) {
    nbg = style.link;
  } else if ( Tattr & T_HEAD ) {
    nbg = style.title;
  } else {
    switch ( Tattr & Tfontmask ) {
    case T_BOLD: nbg = style.B; break;
    case T_IT:   nbg = style.I; break;
    case T_TT:   nbg = style.T; break;

    case T_BOLD|T_IT: nbg = style.BI; break;
    case T_BOLD|T_TT: nbg = style.BT; break;
    case T_IT|T_TT:   nbg = style.IT; break;

    case T_BOLD|T_IT|T_TT: nbg = style.BIT; break;
    }
  }
  myPad()->bkgdset( nbg );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::PadSetLevel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::PadSetLevel()
{
  cindent = listindent * liststack.size();
  if ( cindent > textwidth/2 )
    cindent = textwidth/2;

  if ( atbol ) {
    cc = cindent;
    myPad()->move( cl, cc );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::PadChangeLevel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::PadChangeLevel( bool down, int tag )
{
  if ( down ) {
    if ( liststack.size() )
      liststack.pop();
  } else {
    liststack.push( tag );
  }
  PadSetLevel();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::openAnchor
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::openAnchor( wstring args )
{
  canchor.open( cl, cc );

  const wchar_t * ch = (wchar_t *)args.data();
  const wchar_t * lookupstr = L"href = ";
  const wchar_t * lookup = lookupstr;

  for ( ; *ch && *lookup; ++ch ) {
    wchar_t c = towlower(*ch);
    switch ( c ) {
    case L'\t':
    case L' ':
      if ( *lookup != L' ' )
	lookup = lookupstr;
      break;

    default:
      if ( *lookup == L' ' ) {
	++lookup;
	if ( !*lookup ) {
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

  if ( !*lookup ) {
    const wchar_t * delim = ( *ch == L'"' ) ? L"\"" : L" \t";
    args = ( *ch == L'"' ) ? ++ch : ch;

    wstring::size_type end = args.find_first_of( delim );
    if ( end != wstring::npos )
      args.erase( end );

    canchor.target = args;
  } else {
    WIDERR << "No value for 'HREF=' in anchor '" << args << "'" << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::closeAnchor
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::closeAnchor()
{
  canchor.close( cl, cc );
  if ( canchor.valid() )
    anchors.push_back( canchor );
  canchor = Anchor();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::PadTOKEN
//	METHOD TYPE : bool
//
//	DESCRIPTION : expect "<[/]value>"
//
bool NCRichText::PadTOKEN( const wchar_t * sch, const wchar_t *& ech )
{
  // "<[/]value>"
  if ( *sch++ != L'<' || *(ech-1) != L'>' )
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
  if ( argstart != wstring::npos ) {
    args = value.substr( argstart );
    value.erase( argstart );
  }
  for ( unsigned i = 0; i < value.length(); ++i ) {
    if ( isupper( value[i] ) ) {
      value[i] = static_cast<char>(tolower( value[i] ));
    }
  }

  int leveltag = 0;

  TOKEN token = T_UNKNOWN;
  switch ( value.length() ) {
  case 1:
    if (      value[0] == 'b' )		token = T_BOLD;
    else if ( value[0] == 'i' )		token = T_IT;
    else if ( value[0] == 'p' )		token = T_PAR;
    else if ( value[0] == 'a' )		token = T_ANC;
    else if ( value[0] == 'u' )		token = T_BOLD;
    break;
  case 2:
    if (      value == L"br" )		token = T_BR;
    else if ( value == L"em" )		token = T_IT;
    else if ( value == L"h1" )		token = T_HEAD;
    else if ( value == L"h2" )		token = T_HEAD;
    else if ( value == L"h3" )		token = T_HEAD;
    else if ( value == L"hr" )		token = T_IGNORE;
    else if ( value == L"li" )		token = T_LI;
    else if ( value == L"ol" )		{ token = T_LEVEL; leveltag = 1; }
    else if ( value == L"qt" )		token = T_IGNORE;
    else if ( value == L"tt" )		token = T_TT;
    else if ( value == L"ul" )		{ token = T_LEVEL; leveltag = 0; }
    break;
  case 3:
    if (      value == L"big" )		token = T_IGNORE;
    else if ( value == L"pre" )		token = T_PLAIN;
    break;
  case 4:
    if (      value == L"bold" )	token = T_BOLD;
    else if ( value == L"code" )	token = T_TT;
    else if ( value == L"font" )	token = T_IGNORE;
    break;
  case 5:
    if (      value == L"large" )	token = T_IGNORE;
    else if ( value == L"small" )	token = T_IGNORE;
    break;
  case 6:
    if (      value == L"center" )	token = T_PAR;
    else if ( value == L"strong" )	token = T_BOLD;
    break;
  case 10:
    if (      value == L"blockquote" )	token = T_PAR;
    break;
  default:
    token = T_UNKNOWN;
    break;
  }

  if ( token == T_UNKNOWN ) {
    IDBG << "T_UNKNOWN :" << value << ":" << args << ":" << endl;
// see bug #67319
//  return false;
    return true;
  }

  if ( token == T_IGNORE )
    return true;

  switch ( token ) {

  case T_LEVEL:
    PadChangeLevel( endtag, leveltag );
    PadBOL();
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
    break;

  case T_PAR:
    PadBOL();
    if ( !endtag && !cindent ) {
      PadTXT( L"  ", 2 );
    }
    break;

  case T_LI:
    PadSetLevel();
    PadBOL();
    if ( !endtag ) {
      wstring tag;
      if ( liststack.empty() ) {
	tag = wstring( listindent, L' ' );
      } else {
	wchar_t buf[16];
	if ( liststack.top() )
	{
	    swprintf( buf, 15, L"%2ld. ", liststack.top()++ );
	}
	else
	{
	    swprintf( buf, 15, L" %lc  ",listleveltags[liststack.size()%listleveltags.size()] );
	}
	tag = buf;
      }
      // outsent list tag:
      cc = (tag.size() < cc ? cc - tag.size() : 0 );
      myPad()->move( cl, cc );
      PadTXT( tag.c_str(), tag.size() );
      atbol = true;
    }
    break;

  case T_PLAIN:
      if ( !endtag )
      {
	  preTag = true; 	// display plain text
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::arm
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::arm( unsigned i )
{
  if ( !myPad() ) {
    armed = i;
    return;
  }
  SDBG << i << " (" << armed << ")" << endl;
  if ( i == armed ) {
    if ( armed != Anchor::unset ) {
      // just redraw
      anchors[armed].draw( *myPad(), wStyle().richtext.getArmed( GetState() ), 0 );
      myPad()->update();
    }
    return;
  }
  if ( armed != Anchor::unset ) {
    anchors[armed].draw( *myPad(), wStyle().richtext.link, 0 );
    armed = Anchor::unset;
  }
  if ( i != Anchor::unset ) {
    armed = i;
    anchors[armed].draw( *myPad(), wStyle().richtext.getArmed( GetState() ), 0 );
  }

  if ( showLinkTarget ) {
    if ( armed != Anchor::unset )
      NCPadWidget::setLabel( NCstring( anchors[armed].target ) );
    else
      NCPadWidget::setLabel( NCstring() );
  } else {
    myPad()->update();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::HScroll
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::HScroll( unsigned total, unsigned visible, unsigned start )
{
  NCPadWidget::HScroll( total, visible, start );
  // no hyperlink handling needed, because Ritchtext does not HScroll
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::VScroll
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCRichText::VScroll( unsigned total, unsigned visible, unsigned start )
{
  NCPadWidget::VScroll( total, visible, start );

  if ( plainText || anchors.empty() )
    return; // <-- no links to check

  // Take care of hyperlinks: Check whether an armed link is visible.
  // If not arm the first visible link on pake or none.
  vScrollFirstvisible  = start;
  vScrollNextinvisible = start+visible;

  if ( armed != Anchor::unset ) {
    if ( anchors[armed].within( vScrollFirstvisible, vScrollNextinvisible ) )
      return; // <-- armed link is vissble
    else
      disarm();
  }

  for ( unsigned i = 0; i < anchors.size(); ++i ) {
    if ( anchors[i].within( vScrollFirstvisible, vScrollNextinvisible ) ) {
      arm( i );
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCRichText::handleInput
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCRichText::handleInput( wint_t key )
{
  if ( plainText || anchors.empty() ) {
    return NCPadWidget::handleInput( key );
  }

  // take care of hyperlinks
  bool handled = true;

  switch ( key ) {

  case KEY_LEFT:
    // jump to previous link; scroll up if none
    {
      unsigned newarmed = Anchor::unset;
      if ( armed == Anchor::unset ) {
	// look for an anchor above current page
	for ( unsigned i = anchors.size(); i; ) {
	  --i;
	  if ( anchors[i].eline < vScrollFirstvisible ) {
	    newarmed = i;
	    break;
	  }
	}
      } else if ( armed > 0 ) {
	newarmed = armed-1;
      }
      if ( newarmed == Anchor::unset ) {
	handled = NCPadWidget::handleInput( KEY_UP );
      } else {
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
      if ( armed == Anchor::unset ) {
	// look for an anchor below current page
	for ( unsigned i = 0; i < anchors.size(); ++i ) {
	  if ( anchors[i].sline >= vScrollNextinvisible ) {
	    newarmed = i;
	    break;
	  }
	}
      } else if ( armed+1 < anchors.size() ) {
	newarmed = armed+1;
      }
      if ( newarmed == Anchor::unset ) {
	handled = NCPadWidget::handleInput( KEY_DOWN );
      } else {
	if ( !anchors[newarmed].within( vScrollFirstvisible, vScrollNextinvisible ) )
	  myPad()->ScrlLine( anchors[newarmed].sline );
	arm( newarmed );
      }
    }
    break;

  case KEY_UP:
    // arm previous visible link; scroll up if none
    if (    armed != Anchor::unset
	 && armed > 0
	 && anchors[armed-1].within( vScrollFirstvisible, vScrollNextinvisible ) ) {
      arm( armed-1 );
    } else {
      handled = NCPadWidget::handleInput( key );
    }
    break;

  case KEY_DOWN:
    // arm next visible link; scroll down if none
    if (    armed != Anchor::unset
	 && armed+1 < anchors.size()
	 && anchors[armed+1].within( vScrollFirstvisible, vScrollNextinvisible ) ) {
      arm( armed+1 );
    } else {
      handled = NCPadWidget::handleInput( key );
    }
    break;

  default:
    handled = NCPadWidget::handleInput( key );
  };

  return handled;
}

