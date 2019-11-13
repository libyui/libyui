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

   File:       NCInputText.h

   Author:     Angelo Naselli <anaselli@linux.it>

/-*/
#ifndef NCInputText_h
#define NCInputText_h

#include <iosfwd>

#include "NCWidget.h"


class NCInputTextBase : public NCWidget
{

  friend std::ostream & operator<< ( std::ostream & str, const NCInputTextBase & obj );

  NCInputTextBase & operator= ( const NCInputTextBase & );
  NCInputTextBase ( const NCInputTextBase & );

protected:

  bool     passwd;
  NClabel  _label;
  std::wstring   buffer;

  NCursesWindow * lwin;
  NCursesWindow * twin;

  unsigned maxFldLength;
  unsigned maxInputLength;

  unsigned fldstart;
  unsigned fldlength;
  unsigned curpos;

  bool     returnOnReturn_b;

  virtual void setDefsze();
  virtual void tUpdate();

  virtual bool     bufferFull() const;
  virtual unsigned maxCursor() const;

  virtual const char * location() const
  {
    return "NCInputTextBase";
  }

  virtual void wCreate ( const wrect & newrect );
  virtual void wDelete();

  virtual void wRedraw();


  NCInputTextBase ( YWidget * parent,
                bool passwordMode = false,
                unsigned maxInput = 0,
                unsigned maxFld   = 0
              );
  virtual ~NCInputTextBase();

public:

  void setReturnOnReturn ( bool on_br )
  {
    returnOnReturn_b = on_br;
  }

  virtual int preferredWidth();
  virtual int preferredHeight();

  virtual void setSize ( int newWidth, int newHeight );

  virtual void setEnabled ( bool do_bv );

  virtual void setCurPos ( unsigned pos )
  {
    curpos = pos;
  }
  
};


#endif // NCInputText_h
