/*
  Copyright (C) Angelo Naselli

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

   File:       NCTimeField.h

   Author:     Angelo Naselli <anaselli@linux.it>

/-*/

#ifndef NCTimeField_h
#define NCTimeField_h

#include <iosfwd>

#include <yui/YTimeField.h>
#include "NCWidget.h"
#include "NCInputTextBase.h"


class NCTimeField : public YTimeField, public NCInputTextBase
{

  friend std::ostream & operator<< ( std::ostream & STREAM, const NCTimeField & OBJ );

  NCTimeField & operator= ( const NCTimeField & );
  NCTimeField ( const NCTimeField & );

private:
  static const unsigned fieldLength;
  
  bool validTime(const std::string& input_time);

protected:

  virtual const char * location() const
  {
    return "NCTimeField";
  }

public:

  NCTimeField ( YWidget * parent,
                const std::string & label
              );
  virtual ~NCTimeField();

  virtual int preferredWidth();
  virtual int preferredHeight();

  virtual void setSize ( int newWidth, int newHeight );

  virtual void setLabel ( const std::string & nlabel );

  virtual void setValue ( const std::string & ntext );
  virtual std::string value();

  virtual void setEnabled ( bool do_bv );

  virtual NCursesEvent wHandleInput ( wint_t key );

  virtual bool setKeyboardFocus()
  {
    if ( !grabFocus() )
      return YWidget::setKeyboardFocus();

    return true;
  }

};


#endif // NCTimeField_h
