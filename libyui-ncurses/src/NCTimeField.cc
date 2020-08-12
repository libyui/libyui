/*
  Copyright (C) 2014 Angelo Naselli 

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
#include <iostream>
#include <sstream>

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCTimeField.h"
#include "NCInputTextBase.h"

#include <wctype.h>		// iswalnum()

  
const unsigned NCTimeField::fieldLength = 8;

NCTimeField::NCTimeField ( YWidget * parent,
                           const std::string & nlabel )
  : YTimeField ( parent, nlabel )
  , NCInputTextBase ( parent, false, fieldLength, fieldLength )
{
    // yuiDebug() << std::endl;

  setLabel ( nlabel );

  setValue ( "00:00:00" );
}



NCTimeField::~NCTimeField()
{
    // yuiDebug() << std::endl;
}



int NCTimeField::preferredWidth()
{
  return NCInputTextBase::preferredWidth();
}



int NCTimeField::preferredHeight()
{
  return NCInputTextBase::preferredHeight();
}



void NCTimeField::setSize ( int newwidth, int newheight )
{
  NCInputTextBase::setSize ( newwidth, newheight );
}


void NCTimeField::setEnabled ( bool do_bv )
{
  NCWidget::setEnabled ( do_bv );
  YWidget::setEnabled ( do_bv );
}

void NCTimeField::setLabel ( const std::string & nlabel )
{
  _label  = NCstring ( nlabel );
  _label.stripHotkey();
  YTimeField::setLabel ( nlabel );
  setDefsze();
  Redraw();
}


bool NCTimeField::validTime(const std::string& input_time)
{
  tm tm1;
  std::stringstream ss;
  ss << input_time;
  char c; 
  
  if (!(ss >> tm1.tm_hour))
    return false;
  ss >> c;
  
  if (!(ss >> tm1.tm_min))
    return false;
  ss >> c;
  
  if (!(ss >> tm1.tm_sec))
    return false;

  return (tm1.tm_hour<=23 && tm1.tm_min <= 59 && tm1.tm_sec <= 59);
}


void NCTimeField::setValue ( const std::string & ntext )
{
  if (validTime(ntext))
  {
    buffer = NCstring ( ntext ).str();

    if ( buffer.length() > maxFldLength )
    {
      buffer = buffer.erase ( maxFldLength );
    }

    fldstart = 0;

    tUpdate();
  }
}



std::string NCTimeField::value()
{
  NCstring text ( buffer );

  return text.Str();
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
          {
            std::string buf = NCstring(buffer).Str();
            buffer.erase ( curpos, 1 );
            buffer.insert ( std::wstring::size_type ( curpos ), 1, key );
            
            if (validTime(NCstring(buffer).Str()))
            {
                if ( curpos == 1 || curpos == 4 )
                    curpos += 2;
                else if ( curpos < maxCursor() )
                    ++curpos;
            }
            else
            {
                update = false;
                setValue(buf);
                beep   = true;
            }
          }
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


