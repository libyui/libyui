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

   File:       NCTreePad.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCTreePad_h
#define NCTreePad_h

#include <iosfwd>
#include <vector>

#include "NCTableItem.h"
#include "NCTablePadBase.h"
#include "NCstring.h"

class NCTableLine;
class NCTableCol;


/// An NCPad for an NCTree
class NCTreePad : public NCTablePadBase
{
public:

    NCTreePad( int lines, int cols, const NCWidget & p );
    virtual ~NCTreePad();

    unsigned visibleLines() const { return visibleItems.size(); }

    const NCTableLine * GetCurrentLine() const ;

    void ShowItem( const NCTableLine * item );

    virtual void Destwin( NCursesWindow * dwin );

    virtual bool handleInput( wint_t key );


protected:

    virtual wsze UpdateFormat();

    virtual int  setpos( const wpos & newpos );

    virtual int  DoRedraw();


private:

    NCTreePad & operator=( const NCTreePad & );
    NCTreePad( const NCTreePad & );


    //
    // Data members
    //

    std::vector<NCTableLine*> visibleItems;

};


#endif // NCTreePad_h
