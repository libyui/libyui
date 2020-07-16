/*
  Copyright (C) 2020 SUSE LLC
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

   File:       NCPulldownMenu.h

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#include <iosfwd>
#include "NCWidget.h"


#ifndef NCPulldownMenu_h
#define NCPulldownMenu_h


/**
 * Helper class for NCMenuBar: Pulldown menu button.
 *
 * Notice that this is only an NCWidget, not a YWidget.
 **/
class NCPulldownMenu: public NCWidget
{
public:

    NCPulldownMenu( NCWidget * parent, const std::string & label = "" );
    virtual ~NCPulldownMenu();

    void setLabel( const std::string & label );
    int preferredWidth();
    int preferredHeight();
    void resizeToContent();

    virtual void setEnabled( bool enabled );


protected:

    virtual const char * location() const { return "NCPulldownMenu"; }
    virtual void wRedraw();


private:

    friend std::ostream & operator<<( std::ostream & str,
                                      const NCPulldownMenu & obj );

    // Disable unwanted copy constructor and assignment operator

    NCPulldownMenu & operator=( const NCPulldownMenu & );
    NCPulldownMenu( const NCPulldownMenu & );


    // Data members

    NClabel _label;
};

#endif  // NCPulldownMenu_h
