/*
  Copyright (C) 2019 SUSE LLC
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

   File:       NCCustomStatusItemSelector.h

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef NCCustomStatusItemSelector_h
#define NCCustomStatusItemSelector_h

#include <iosfwd>
#include <string>

#include "NCItemSelector.h"


class NCCustomStatusItemSelector : public NCItemSelectorBase
{
    friend std::ostream & operator<<( std::ostream & str, const NCCustomStatusItemSelector & obj );

public:

    /**
     * Constructor.
     **/
    NCCustomStatusItemSelector( YWidget * parent,
                                const YItemCustomStatusVector & customStates );

    /**
     * Destructor.
     **/
    virtual ~NCCustomStatusItemSelector();

    /**
     * Handle keyboard input.
     **/
    virtual NCursesEvent wHandleInput( wint_t key );

    virtual const char * location() const { return "NCCustomStatusItemSelector"; }


protected:

    /**
     * Create a tag cell for an item. This is the cell with the "[x]" or "(x)"
     * selector. It also stores the item pointer so the item can later be
     * referenced by this tag.
     **/
    virtual NCTableTag * createTagCell( YItem * item );

    /**
     * Cycle the status of the current item through its possible values.
     * For a plain ItemSelector, this means true -> false -> true.
     **/
    virtual void cycleCurrentItemStatus();

    /**
     * Return the tag cell (the cell with the "[x]" or "(x)" selector) for the
     * item with the specified index.
     **/
    NCTableTag * tagCell( int index ) const;


private:

    // Disable assignement operator and copy constructor

    NCCustomStatusItemSelector & operator=( const NCCustomStatusItemSelector & );
    NCCustomStatusItemSelector( const NCCustomStatusItemSelector & );
};


#endif // NCCustomStatusItemSelector_h
