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

   File:       NCInputText.cc

   Author:     Angelo Naselli <anaselli@linux.it>

/-*/
#include <climits>


#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCurses.h"
#include "NCInputText.h"

#include <wctype.h>		// iswalnum()


NCInputTextBase::NCInputTextBase ( YWidget * parent,
                           bool passwordMode,
                           unsigned maxInput,
                           unsigned maxFld )
                        : NCWidget ( parent )
                        , passwd ( passwordMode )
                        , lwin ( 0 )
                        , twin ( 0 )
                        , maxFldLength ( maxFld )
                        , maxInputLength ( maxInput )
                        , fldstart ( 0 )
                        , fldlength ( 0 )
                        , curpos ( 0 )
                        , returnOnReturn_b ( false )
{
  yuiDebug() << std::endl;

  if ( maxInputLength &&
       ( !maxFldLength || maxFldLength > maxInputLength ) )
  {
    maxFldLength = maxInputLength;
  }

  hotlabel = &_label;
}



NCInputTextBase::~NCInputTextBase()
{
  delete lwin;
  delete twin;
  yuiDebug() << std::endl;
}



int NCInputTextBase::preferredWidth()
{
  return wGetDefsze().W;
}



int NCInputTextBase::preferredHeight()
{
  return wGetDefsze().H;
}



void NCInputTextBase::setEnabled ( bool do_bv )
{
  NCWidget::setEnabled ( do_bv );
}



void NCInputTextBase::setSize ( int newwidth, int newheight )
{
  wRelocate ( wpos ( 0 ), wsze ( newheight, newwidth ) );
}



void NCInputTextBase::setDefsze()
{
  unsigned defwidth = maxFldLength ? maxFldLength : 5;

  if ( _label.width() > defwidth )
    defwidth = _label.width();

  defsze = wsze ( _label.height() + 1, defwidth );
}



void NCInputTextBase::wCreate ( const wrect & newrect )
{
  NCWidget::wCreate ( newrect );

  if ( !win )
    return;

  wrect lrect ( 0, wsze::min ( newrect.Sze,
                               wsze ( _label.height(), newrect.Sze.W ) ) );

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



void NCInputTextBase::wDelete()
{
  delete lwin;
  delete twin;
  lwin = 0;
  twin = 0;
  NCWidget::wDelete();
}


void NCInputTextBase::wRedraw()
{
  if ( !win )
    return;

  // label
  const NCstyle::StWidget & style ( widgetStyle ( true ) );

  lwin->bkgd ( style.plain );

  lwin->clear();

  _label.drawAt ( *lwin, style );

  tUpdate();
}



bool NCInputTextBase::bufferFull() const
{
  return ( maxInputLength && buffer.length() == maxInputLength );
}



unsigned NCInputTextBase::maxCursor() const
{
  return ( bufferFull() ? buffer.length() - 1 : buffer.length() );
}



void NCInputTextBase::tUpdate()
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
    if ( passwd )
    {
      twin->addwstr ( L"*" );
    }
    else
    {
      twin->addwstr ( cp, 1 );
    }

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



