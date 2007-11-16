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

   File:       NCInputField.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include <climits>

#include "Y2Log.h"
#include "NCurses.h"
#include "NCInputField.h"

#include <wctype.h>		// iswalnum()

#if 0
#undef  DBG_CLASS
#define DBG_CLASS "_NCInputField_"
#endif

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::NCInputField
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCInputField::NCInputField( YWidget * parent,
			    const string & nlabel,
			    bool passwordMode,
			    unsigned maxInput,
			    unsigned maxFld )
    : YInputField( parent, nlabel, passwordMode )
    , NCWidget( parent )
    , passwd( passwordMode )
    , lwin( 0 )
    , twin( 0 )
    , maxFldLength  ( maxFld )
    , maxInputLength( maxInput )
    , fldstart ( 0 )
    , fldlength( 0 )
    , curpos   ( 0 )
    , fldtype  ( PLAIN )
    , returnOnReturn_b( false )
    , InputMaxLength( -1 )
{
  WIDDBG << endl;
  if ( maxInputLength &&
       ( !maxFldLength || maxFldLength > maxInputLength ) ) {
     maxFldLength = maxInputLength;
  }
  setLabel( nlabel );
  hotlabel = &label;
  // initial text isn't an argument any longer
  //setText( ntext );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::~NCInputField
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCInputField::~NCInputField()
{
  delete lwin;
  delete twin;
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::preferredWidth
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCInputField::preferredWidth()
{
    return wGetDefsze().W;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::preferredHeight
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCInputField::preferredHeight()
{
    return wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::setEnabled
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCInputField::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YInputField::setEnabled( do_bv );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCInputField::setSize( int newwidth, int newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::setDefsze
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCInputField::setDefsze()
{
  unsigned defwidth = maxFldLength ? maxFldLength : 5;
  if ( label.width() > defwidth )
    defwidth = label.width();
  defsze = wsze( label.height() + 1, defwidth );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::wCreate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCInputField::wCreate( const wrect & newrect )
{
  NCWidget::wCreate( newrect );

  if ( !win )
      return;
  
  wrect lrect( 0, wsze::min( newrect.Sze,
			     wsze( label.height(), newrect.Sze.W ) ) );
  if ( lrect.Sze.H == newrect.Sze.H )
    lrect.Sze.H -= 1;

  wrect trect( 0, wsze( 1, newrect.Sze.W ) );
  trect.Pos.L = lrect.Sze.H > 0 ? lrect.Sze.H : 0;

  lwin = new NCursesWindow( *win,
			    lrect.Sze.H, lrect.Sze.W,
			    lrect.Pos.L, lrect.Pos.C,
			    'r' );
  twin = new NCursesWindow( *win,
			    trect.Sze.H, trect.Sze.W,
			    trect.Pos.L, trect.Pos.C,
			    'r' );
  if ( maxFldLength && maxFldLength < (unsigned)newrect.Sze.W )
    trect.Sze.W = maxFldLength;
  fldlength=trect.Sze.W;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::wDelete
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCInputField::wDelete()
{
  delete lwin;
  delete twin;
  lwin = 0;
  twin = 0;
  NCWidget::wDelete();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCInputField::setLabel( const string & nlabel )
{
  label  = NCstring( nlabel );
  label.stripHotkey();
  YInputField::setLabel( nlabel );
  setDefsze();
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::setValue
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCInputField::setValue( const std::string & ntext )
{
  buffer = NCstring( ntext ).str();
  if ( maxInputLength && buffer.length() > maxInputLength ) {
    buffer = buffer.erase( maxInputLength );
  }
  fldstart = 0;
  curpos   = buffer.length();
  tUpdate();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::value
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
string NCInputField::value( )
{
    NCstring text ( buffer );

    return text.Str();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::setValidChars
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCInputField::setValidChars( const string & validchars )
{
  validChars = NCstring( validchars );
  YInputField::setValidChars( validchars );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::validKey
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCInputField::validKey( wint_t key ) const
{
  // private: NCstring validChars;  
  const wstring vwch( validChars.str() );

  if ( vwch.empty() )
    return true;

  if ( key < 0 || WCHAR_MAX < key )
    return false;

  return( vwch.find( (wchar_t)key ) != wstring::npos );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCInputField::wRedraw()
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::bufferFull
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
inline bool NCInputField::bufferFull() const
{
  return( maxInputLength && buffer.length() == maxInputLength );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::maxCursor
//	METHOD TYPE : unsigned
//
//	DESCRIPTION :
//
inline unsigned NCInputField::maxCursor() const
{
  return( bufferFull() ? buffer.length() - 1 : buffer.length() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::tUpdate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCInputField::tUpdate()
{
  if ( !win )
    return;

  unsigned maxc = maxCursor();

  // adjust cursor
  if ( curpos > maxc ) {
    curpos = maxc;
  }

  // adjust fldstart that cursor is visible
  if ( maxc < fldlength ) {
    fldstart = 0;
  } else {
    if ( curpos <= fldstart ) {
      fldstart = curpos ? curpos - 1 : 0;
    }
    if ( curpos >= fldstart + fldlength - 1 ) {
      fldstart = curpos + (curpos == maxc ? 1 : 2) - fldlength;
    }
  }

  const NCstyle::StWidget & style( widgetStyle() );
  twin->bkgd( widgetStyle( true ).plain );
  twin->move( 0, 0 );

  unsigned i      = 0;
  unsigned end    = fldlength;
  const wchar_t * cp = buffer.data() + fldstart;

  // draw left scrollhint if
  if ( *cp && fldstart ) {
    twin->bkgdset( style.scrl );
    twin->addch( ACS_LARROW );
    ++i;
    ++cp;
  }

  // check for right scrollhint
  if ( fldstart + fldlength <= maxc ) {
    --end;
  }

  // draw field
  twin->bkgdset( style.data );
  for ( /*adjusted i*/; *cp && i < end; ++i )
  {
      if ( passwd )
      {
	  twin->addwstr( L"*" );
      }
      else
      {
	  twin->addwstr( cp, 1 );
      }
      ++cp;
  }
  twin->bkgdset( style.plain );
  for ( /*adjusted i*/; i < end; ++i ) {
      twin->addch( ACS_CKBOARD ); 
  }

  // draw right scrollhint if
  if ( end < fldlength ) {
    twin->bkgdset( style.scrl );
    twin->addch( ACS_RARROW );
  }

  // reverse curpos
  if ( GetState() == NC::WSactive ) {
    twin->move( 0, curpos - fldstart );
    twin->bkgdset( wStyle().cursor );

    if ( curpos < buffer.length() )
	twin->add_attr_char( );
    else
	twin->addch( ACS_CKBOARD );
  }
  
  Update();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCInputField::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCInputField::wHandleInput( wint_t key )
{
  NCursesEvent ret = NCursesEvent::none;
  bool   beep   = false;
  bool   update = true;

  switch ( key ) {

  case KEY_BACKSPACE:
    if ( bufferFull() && curpos == maxCursor() ) {
      // if we're on the last char in a full buffer delete this char
      // and not the previous one.
      buffer.erase( curpos, 1 );
    } else if ( curpos ) {
      buffer.erase( --curpos, 1 );
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_DC:
    if ( curpos < buffer.length() ) {
      buffer.erase( curpos, 1 );
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_HOME:
    if ( curpos ) {
      curpos = 0;
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_END:
    if ( curpos < maxCursor() ) {
      curpos = maxCursor();
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_LEFT:
    if ( curpos ) {
      --curpos;
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_RIGHT:
    if ( curpos < maxCursor() ) {
      ++curpos;
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_RETURN:
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
   
   if ( (!is_special && KEY_MIN < key && KEY_MAX > key )
	||
	!iswprint( key )
	||
	// if we are at limit of input
	(InputMaxLength >= 0 && InputMaxLength <= (int)buffer.length()) )
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
	   switch ( key ) {
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
		   if ( curpos || buffer.empty() || buffer[0] != L'-' ) {
		       buffer.insert(wstring::size_type(curpos), 1, key );
		       if ( curpos < maxCursor() )
			   ++curpos;
		   } else {
		       update = false;
		       beep   = true;
		   }
		   break;

	       case L'+':
		   if ( !buffer.empty() && buffer[0] == L'-' ) {
		       buffer.erase(wstring::size_type(0), 1);
		       if ( curpos )
			   --curpos;
		   } else {
		       update = false;
		   }
		   break;

	       case L'-':
		   if ( buffer.empty() || buffer[0] != L'-' ) {
		       buffer.insert(wstring::size_type(0), 1, L'-');
		       if ( curpos < maxCursor() )
			   ++curpos;
		   } else {
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

      if ( bufferFull() || !validKey( key ) ) {
	update = false;
	beep   = true;
      } else {
	buffer.insert( wstring::size_type(curpos), 1, key );
	if ( curpos < maxCursor() )
	  ++curpos;
      }

    }
    break;
  }

  if ( update ) {
    tUpdate();
    if ( notify() )
      ret = NCursesEvent::ValueChanged;
  }

  if ( beep )
    ::beep();

  return ret;

}


void NCInputField::setInputMaxLength( int numberOfChars)
{
    int nr = numberOfChars;

    // if there is more text then the maximum number of chars,
    // truncate the text and update the buffer
    if ( nr >= 0 && (int)buffer.length() > nr ) {
	buffer.erase( nr, maxCursor() - nr );
	tUpdate();
	curpos = buffer.length();
    }
  
    InputMaxLength = nr;

    YInputField::setInputMaxLength( numberOfChars );        
}
