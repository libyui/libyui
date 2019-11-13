/*
  Copyright (C) 2000-2012 Novell, Inc
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

   File:       NCRadioButtonGroup.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCRadioButtonGroup_h
#define NCRadioButtonGroup_h

#include <iosfwd>

#include <yui/YRadioButtonGroup.h>
#include "NCRadioButton.h"
#include "NCWidget.h"

class NCRadioButtonGroup;


class NCRadioButtonGroup : public YRadioButtonGroup, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & str, const NCRadioButtonGroup & obj );

    NCRadioButtonGroup & operator=( const NCRadioButtonGroup & );
    NCRadioButtonGroup( const NCRadioButtonGroup & );

    int focusId;

protected:

    virtual const char * location() const { return "NCRadioButtonGroup"; }

public:

    NCRadioButtonGroup( YWidget * parent );
    virtual ~NCRadioButtonGroup();

    virtual int preferredWidth() { return YRadioButtonGroup::preferredWidth(); }
    virtual int preferredHeight() { return YRadioButtonGroup::preferredHeight(); }

    virtual void setSize( int newWidth, int newHeight );

    virtual void addRadioButton( YRadioButton *button );
    virtual void removeRadioButton( YRadioButton *button );

    virtual void setEnabled( bool do_bv );

    void focusNextButton( );
    void focusPrevButton( );

};


#endif // NCRadioButtonGroup_h
