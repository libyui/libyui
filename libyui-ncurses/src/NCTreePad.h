/*
  Copyright (C) 2000-2012 Novell, Inc
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


/**
 * An NCPad for an NCTree.
 *
 * Most of its former functionality is now handled in the NCTablePadBase base
 * class.
 **/
class NCTreePad : public NCTablePadBase
{
public:

    NCTreePad( int lines, int cols, const NCWidget & p );
    virtual ~NCTreePad();

    void ShowItem( const NCTableLine * item );

    virtual void Destwin( NCursesWindow * dwin );

    /**
     * Handle a keyboard input event. Return 'true' if the event is now
     * handled, 'false' if it should be propagated to the parent widget.
     *
     * Most of the keys are now handled in the NCTablePadBase base class or in
     * the individual items' handlers (NCTreeLine, NCTableLine). This method
     * is mostly here as a stub for future extensions.
     *
     * Reimplemented from NCTablePadBase and NCPad.
     **/
    virtual bool handleInput( wint_t key );


protected:

    /**
     * Redraw the pad.
     *
     * Reimplemented from NCTablePadBase and NCPad.
     **/
    virtual int DoRedraw();


private:

    NCTreePad & operator=( const NCTreePad & );
    NCTreePad( const NCTreePad & );
};


#endif // NCTreePad_h
