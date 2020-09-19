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

   File:       NCTablePad.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCTablePad_h
#define NCTablePad_h

#include <iosfwd>
#include <vector>
#include <memory>		// unique_ptr

#include "NCTablePadBase.h"
#include "NCTableItem.h"
#include "NCTableSort.h"
#include "NCstring.h"

class NCTableLine;
class NCTableCol;
class NCTableTag;



/**
 * An NCPad for an NCTable
 **/
class NCTablePad : public NCTablePadBase
{
public:

    NCTablePad( int lines, int cols, const NCWidget & p );
    virtual ~NCTablePad();

public:

    /**
     * Handle a keyboard input event. Return 'true' if the event is now
     * handled, 'false' if it should be propagated to the parent widget.
     **/
    virtual bool handleInput( wint_t key );

    bool setItemByKey( int key );

    void sort();

    /**
     * Sort by *column*; if that is the sorting column already, sort in
     * reverse order if *do_reverse*.
     * Do nothing if column < 0.
     **/
    void setOrder( int column, bool do_reverse = false );

    /**
     * @param newSortStrategy (we take ownership)
     **/
    void setSortStrategy ( NCTableSortStrategyBase * newSortStrategy ) // dyn. allocated
    {
        if ( newSortStrategy != 0 )
            sortStrategy.reset ( newSortStrategy );
    }

    void AssertMinCols( unsigned num )
    {
	_itemStyle.AssertMinCols( num );
    }

    void SetSepChar( const chtype colSepchar )
    {
	_itemStyle.SetSepChar( colSepchar );
    }

    void SetSepWidth( const unsigned sepwidth )
    {
	_itemStyle.SetSepWidth( sepwidth );
    }

    unsigned HotCol() const
    {
        return _itemStyle.HotCol();
    }

    void SetHotCol( int hcol )
    {
	_itemStyle.SetHotCol( hcol );
    }

    /**
     * Find the item index in a sorted table.
     * Return -1 if not found.
     * An item/line remembers its insertion index...
     *
     * @param id the index before sorting
     **/
    int findIndexById( int id ) const;

    void stripHotkeys();


protected:

    virtual int  setpos( const wpos & newpos );

    virtual int  DoRedraw();

    virtual void directDraw( NCursesWindow & w, const wrect at, unsigned lineno );


private:

    // Disable unwanted assignment operator and copy constructor

    NCTablePad & operator=( const NCTablePad & );
    NCTablePad( const NCTablePad & );

    //
    // Data members
    //

    std::unique_ptr<NCTableSortStrategyBase> sortStrategy;
};


#endif // NCTablePad_h
