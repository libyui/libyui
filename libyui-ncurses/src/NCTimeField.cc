/*
  Copyright (C) 2014 Novell, Inc
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

   File:       NCTimeField.cc

   Author:     Angelo Naselli <anaselli@linux.it>

/-*/
#include <climits>


#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCTimeField.h"

#include <wctype.h>		// iswalnum()

const unsigned NCTimeField::maxFldLength = 8;

NCTimeField::NCTimeField ( YWidget * parent,
                           const std::string & nlabel )
  : YTimeField ( parent, nlabel )
  , NCWidget ( parent )
  , lwin ( 0 )
  , twin ( 0 )
  , fldstart ( 0 )
  , fldlength ( 0 )
  , curpos ( 0 )
  , returnOnReturn_b ( false )
{
  yuiDebug() << std::endl;

  setLabel ( nlabel );

  hotlabel = &label;

  setValue ( "00:00:00" );
}



NCTimeField::~NCTimeField()
{
  delete lwin;
  delete twin;
  yuiDebug() << std::endl;
}



int NCTimeField::preferredWidth()
{
  return wGetDefsze().W;
}



int NCTimeField::preferredHeight()
{
  return wGetDefsze().H;
}



void NCTimeField::setSize ( int newwidth, int newheight )
{
  wRelocate ( wpos ( 0 ), wsze ( newheight, newwidth ) );
}



void NCTimeField::setDefsze()
{
  unsigned defwidth = maxFldLength;

  if ( label.width() > defwidth )
    defwidth = label.width();

  defsze = wsze ( label.height() + 1, defwidth );
}



void NCTimeField::wCreate ( const wrect & newrect )
{
  NCWidget::wCreate ( newrect );

  if ( !win )
    return;

  wrect lrect ( 0, wsze::min ( newrect.Sze,
                               wsze ( label.height(), newrect.Sze.W ) ) );

  if ( lrect.Sze.H == newrect.Sze.H )
    lrect.Sze.H -= 1;

  wrect trect ( 0, wsze ( 1, newrect.Sze.W ) );

  trect.Pos.L = lrect.Sze.H > 0 ? lrect.Sze.H : 0;

  lwin = new NCursesWindow ( *win,
                             lrect.Sze.H, lrect.Sze.W,
                             lrect.Pos.L, lrect.Pos.C,
                             'r' );

  twin = new NCursesWindow ( *win,
                             trect.Sze.H, trect.Sze.W,
                             trect.Pos.L, trect.Pos.C,
                             'r' );

  if ( maxFldLength && maxFldLength < ( unsigned ) newrect.Sze.W )
    trect.Sze.W = maxFldLength;

  fldlength = trect.Sze.W;
}



void NCTimeField::wDelete()
{
  delete lwin;
  delete twin;
  lwin = 0;
  twin = 0;
  NCWidget::wDelete();
}

void NCTimeField::setEnabled ( bool do_bv )
{
  NCWidget::setEnabled ( do_bv );
  YWidget::setEnabled ( do_bv );
}

void NCTimeField::setLabel ( const std::string & nlabel )
{
  label  = NCstring ( nlabel );
  label.stripHotkey();
  YTimeField::setLabel ( nlabel );
  setDefsze();
  Redraw();
}



void NCTimeField::setValue ( const std::string & ntext )
{
  buffer = NCstring ( ntext ).str();

  if ( buffer.length() > maxFldLength )
  {
    buffer = buffer.erase ( maxFldLength );
  }

  fldstart = 0;

  curpos   = buffer.length();
  tUpdate();
}



std::string NCTimeField::value( )
{
  NCstring text ( buffer );

  return text.Str();
}



bool NCTimeField::validKey ( wint_t key ) const
{
  // private: NCstring validChars;
  const std::wstring vwch ( validChars.str() );

  if ( vwch.empty() )
    return true;

  if ( key < 0 || WCHAR_MAX < key )
    return false;

  return ( vwch.find ( ( wchar_t ) key ) != std::wstring::npos );
}



void NCTimeField::wRedraw()
{
  if ( !win )
    return;

  // label
  const NCstyle::StWidget & style ( widgetStyle ( true ) );

  lwin->bkgd ( style.plain );

  lwin->clear();

  label.drawAt ( *lwin, style );

  tUpdate();
}



inline bool NCTimeField::bufferFull() const
{
  return ( buffer.length() == maxFldLength );
}



inline unsigned NCTimeField::maxCursor() const
{
  return ( bufferFull() ? buffer.length() - 1 : buffer.length() );
}



void NCTimeField::tUpdate()
{
  if ( !win )
    return;

  unsigned maxc = maxCursor();

  // adjust cursor
  if ( curpos > maxc )
  {
    curpos = maxc;
  }

  // adjust fldstart that cursor is visible
  if ( maxc < fldlength )
  {
    fldstart = 0;
  }
  else
  {
    if ( curpos <= fldstart )
    {
      fldstart = curpos ? curpos - 1 : 0;
    }

    if ( curpos >= fldstart + fldlength - 1 )
    {
      fldstart = curpos + ( curpos == maxc ? 1 : 2 ) - fldlength;
    }
  }

  const NCstyle::StWidget & style ( widgetStyle() );

  twin->bkgd ( widgetStyle ( true ).plain );

  twin->move ( 0, 0 );

  unsigned i	    = 0;

  unsigned end    = fldlength;

  const wchar_t * cp = buffer.data() + fldstart;

  // draw left scrollhint if
  if ( *cp && fldstart )
  {
    twin->bkgdset ( style.scrl );
    twin->addch ( ACS_LARROW );
    ++i;
    ++cp;
  }

  // check for right scrollhint
  if ( fldstart + fldlength <= maxc )
  {
    --end;
  }

  // draw field
  twin->bkgdset ( style.data );

  for ( /*adjusted i*/; *cp && i < end; ++i )
  {
    twin->addwstr ( cp, 1 );

    ++cp;
  }

  twin->bkgdset ( style.plain );

  for ( /*adjusted i*/; i < end; ++i )
  {
    twin->addch ( ACS_CKBOARD );
  }

  // draw right scrollhint if
  if ( end < fldlength )
  {
    twin->bkgdset ( style.scrl );
    twin->addch ( ACS_RARROW );
  }

  // reverse curpos
  if ( GetState() == NC::WSactive )
  {
    twin->move ( 0, curpos - fldstart );
    twin->bkgdset ( wStyle().cursor );

    if ( curpos < buffer.length() )
      twin->add_attr_char( );
    else
      twin->addch ( ACS_CKBOARD );
  }

  Update();
}



NCursesEvent NCTimeField::wHandleInput ( wint_t key )
{
  NCursesEvent ret = NCursesEvent::none;
  bool   beep   = false;
  bool   update = true;

  switch ( key )
  {
  case '\b': //ctrl-h
  case 0x7f: //del
  case KEY_BACKSPACE:

    buffer.erase ( curpos, 1 );
    buffer.insert ( std::wstring::size_type ( curpos ), 1, '0' );
    if ( curpos == 3 || curpos == 6 )
      curpos -= 2;
    else
      if ( curpos )
        curpos--;

    break;

  case KEY_DC:

    if ( curpos < buffer.length() )
    {
      buffer.erase ( curpos, 1 );
      buffer.insert ( std::wstring::size_type ( curpos ), 1, '0' );
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

    if ( curpos == 3 || curpos == 6 )
      curpos -= 2;
    else
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

    if ( curpos == 1 || curpos == 4 )
      curpos += 2;
    else
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

    if ( ( !is_special && KEY_MIN < key && KEY_MAX > key )
         ||
         !iswprint ( key ) )
    {
      update = false;
      beep   = true;
    }
    else
    {
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

          buffer.erase ( curpos, 1 );
          buffer.insert ( std::wstring::size_type ( curpos ), 1, key );

          if ( curpos == 1 || curpos == 4 )
            curpos += 2;
          else
            if ( curpos < maxCursor() )
              ++curpos;

          break;

        default:
          update = false;
          beep   = true;
          break;
        }
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


