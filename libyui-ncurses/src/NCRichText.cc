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

#if 0
#undef  DBG_CLASS
#define DBG_CLASS "_NCRichText_"
#endif

///////////////////////////////////////////////////////////////////

const unsigned NCRichText::listindent = 4;
const unsigned NCRichText::listindentafterlevel = 1;
const string   NCRichText::listleveltags( "@*+o#-%$&" );//

const bool NCRichText::showLinkTarget = false;

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
NCRichText::NCRichText( NCWidget * parent, YWidgetOpt & opt,
			const YCPString & ntext )
    : YRichText( opt, ntext )
    , NCPadWidget( parent )
    , text( ntext )
    , plainText( opt.plainTextMode.value() )
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

  if ( autoScrollDown && pad ) {
    pad->ScrlDown( pad->maxy() );
    pad->update();
  }
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

  if ( !( plainText || anchors.empty() ) )
    arm( armed );
  NCPadWidget::wRedraw();
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
NCursesEvent NCRichText::wHandleInput( int key )
{
  NCursesEvent ret;
  handleInput( key );

  if ( !( plainText || anchors.empty() ) ) {
    switch ( key ) {
    case KEY_SPACE:
    case KEY_RETURN:
      if ( armed != Anchor::unset ) {
	ret = NCursesEvent::menu;
	ret.selection = YCPString( anchors[armed].target );
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
    << "       padsize " << pad->size() << endl
    << "       text length " << text.str().size() << endl;

  pad->bkgdset( wStyle().richtext.plain );
  pad->clear();

  if ( plainText )
    DrawPlainPad();
  else
    DrawHTMLPad();

  if ( autoScrollDown ) {
    pad->ScrlDown( pad->maxy() );
    pad->update();
  }
  
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
    pad->addstr( cl, 0, (*line).str().c_str() );
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
inline void SkipToken( const char *& ch )
{
  do {
    ++ch;
  } while ( *ch && *ch != '>' );
  if ( *ch )
    ++ch;
}

static string WStoken( " \n\t\v\r" );
inline void SkipWS( const char *& ch ) {
  do {
    ++ch;
  } while ( *ch && WStoken.find( *ch ) != string::npos );
}

static string WDtoken( " <\n\t\v\r" ); // WS + TokenStart '<'
inline void SkipWord( const char *& ch ) {
  do {
    ++ch;
  } while ( *ch && WDtoken.find( *ch ) == string::npos );
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
  pad->move( cl, cc );
  atbol = true;

  const char * ch = text.str().c_str();
  const char * sch = 0;

  while ( *ch ) {
    switch ( *ch ) {
    case ' ':
    case '\t':
    case '\n':
    case '\v':
    case '\r':
      SkipWS( ch );
      PadWS();
      break;

    case '<':
      sch = ch;
      SkipToken( ch );
      if ( PadTOKEN( sch, ch ) )
	break;    // strip token
      else
	ch = sch; // reset and fall through

    default:
      sch = ch;
      SkipWord( ch );
      PadTXT( sch, ch - sch );
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
  if ( ++cl == (unsigned)pad->height() ) {
    AdjustPad( wsze( pad->height() + defPadSze().H, textwidth ) );
  }
  pad->move( cl, cc );
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
    PadNL();
  else {
    pad->addch( ' ' );
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
inline void NCRichText::PadTXT( const char * osch, const unsigned olen )
{
  string txt( osch, olen );
  for( string::size_type special = txt.find( "&nbsp;" );
       special != string::npos;
       special = txt.find( "&nbsp;", special ) )
    txt.replace( special, 6, 1, ' ' );

  const char * sch = txt.c_str();
  unsigned     len = txt.length();

  if ( !atbol && cc + len > textwidth )
    PadNL();
  for ( unsigned i = 0; i < len; ++i, ++sch ) {
    pad->addch( *sch );
    atbol = false;
    if ( ++cc >= textwidth ) {
      PadNL();
    }
  }
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
  pad->bkgdset( nbg );
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
  if ( liststack.size() <= listindentafterlevel ) {
    cindent = 0;
  } else {
    cindent = listindent * (liststack.size() - listindentafterlevel);
    if ( cindent > textwidth/2 )
      cindent = textwidth/2;
  }

  if ( atbol )
    pad->move( cl, cindent );
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
void NCRichText::openAnchor( string args )
{
  canchor.open( cl, cc );

  const char * ch = args.c_str();
  const char * lookupstr = "href = ";
  const char * lookup = lookupstr;

  for ( ; *ch && *lookup; ++ch ) {
    char c = tolower(*ch);
    switch ( c ) {
    case '\t':
    case ' ':
      if ( *lookup != ' ' )
	lookup = lookupstr;
      break;

    default:
      if ( *lookup == ' ' ) {
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
    const char * delim = ( *ch == '"' ) ? "\"" : " \t";
    args = ( *ch == '"' ) ? ++ch : ch;

    string::size_type end = args.find_first_of( delim );
    if ( end != string::npos )
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
bool NCRichText::PadTOKEN( const char * sch, const char *& ech )
{
  // "<[/]value>"
  if ( *sch++ != '<' || *(ech-1) != '>' )
    return false;

  // "[/]value>"
  bool endtag = ( *sch == '/' );
  if ( endtag )
    ++sch;

  // "value>"
  if ( ech - sch <= 1 )
    return false;

  string value( sch, ech - 1 - sch );
  string args;
  string::size_type argstart = value.find_first_of( " \t\n" );
  if ( argstart != string::npos ) {
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
    break;
  case 2:
    if (      value == "br" )		token = T_BR;
    else if ( value == "em" )		token = T_IT;
    else if ( value == "h1" )		token = T_HEAD;
    else if ( value == "h2" )		token = T_HEAD;
    else if ( value == "h3" )		token = T_HEAD;
    else if ( value == "hr" )		token = T_IGNORE;
    else if ( value == "li" )		token = T_LI;
    else if ( value == "ol" )		{ token = T_LEVEL; leveltag = 1; }
    else if ( value == "qt" )		token = T_IGNORE;
    else if ( value == "tt" )		token = T_TT;
    else if ( value == "ul" )		{ token = T_LEVEL; leveltag = 0; }
    break;
  case 3:
    if (      value == "big" )		token = T_IGNORE;
    else if ( value == "pre" )		token = T_IGNORE;
    break;
  case 4:
    if (      value == "bold" )		token = T_BOLD;
    else if ( value == "code" )		token = T_TT;
    else if ( value == "font" )		token = T_IGNORE;
    break;
  case 5:
    if (      value == "large" )	token = T_IGNORE;
    else if ( value == "small" )	token = T_IGNORE;
    break;
  case 6:
    if (      value == "center" )	token = T_PAR;
    else if ( value == "strong" )	token = T_BOLD;
    break;
  case 10:
    if (      value == "blockquote" )	token = T_PAR;
    break;
  default:
    token = T_UNKNOWN;
    break;
  }

  if ( token == T_UNKNOWN ) {
    IDBG << "T_UNKNOWN :" << value << ":" << args << ":" << endl;
    return false;
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
      PadTXT( "  ", 2 );
    }
    break;

  case T_LI:
    PadSetLevel();
    PadBOL();
    if ( !endtag ) {
      string tag;
      if ( liststack.empty() ) {
	tag = string( listindent, ' ' );
      } else {
	char buf[16];
	if ( liststack.top() )
	  sprintf( buf, "%2d. ", liststack.top()++ );
	else
	  sprintf( buf, " %c  ", listleveltags[liststack.size()%listleveltags.size()] );
	tag = buf;
      }
      PadTXT( tag.c_str(), tag.size() );
      cindent += listindent;
      atbol = true;
    }
    break;

  case T_ANC:
    if ( endtag )
      closeAnchor();
    else
      openAnchor( args );
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
  if ( !pad ) {
    armed = i;
    return;
  }
  SDBG << i << " (" << armed << ")" << endl;
  if ( i == armed ) {
    if ( armed != Anchor::unset ) {
      // just redraw
      anchors[armed].draw( *pad, wStyle().richtext.getArmed( GetState() ), 0 );
      pad->update();
    }
    return;
  }
  if ( armed != Anchor::unset ) {
    anchors[armed].draw( *pad, wStyle().richtext.link, 0 );
    armed = Anchor::unset;
  }
  if ( i != Anchor::unset ) {
    armed = i;
    anchors[armed].draw( *pad, wStyle().richtext.getArmed( GetState() ), 0 );
  }

  if ( showLinkTarget ) {
    if ( armed != Anchor::unset )
      NCPadWidget::setLabel( NCstring( anchors[armed].target ) );
    else
      NCPadWidget::setLabel( NCstring() );
  } else {
    pad->update();
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
bool NCRichText::handleInput( int key )
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
	  pad->ScrlLine( anchors[newarmed].sline );
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
	  pad->ScrlLine( anchors[newarmed].sline );
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

