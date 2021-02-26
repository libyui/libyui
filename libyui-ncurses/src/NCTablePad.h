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

   File:       NCTablePad.h

   Authors:    Michael Andres <ma@suse.de>
               Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef NCTablePad_h
#define NCTablePad_h

#include <iosfwd>
#include <vector>

#include "NCTablePadBase.h"
#include "NCTableItem.h"
#include "NCstring.h"


/**
 * An NCPad for an NCTable.
 *
 * Most of its former functionality is now handled in the NCTablePadBase base
 * class.
 *
 * See also
 * https://github.com/libyui/libyui-ncurses/blob/master/doc/nctable-and-nctree.md
 *
 * Notice that this class is heavily used in other selection widgets like
 *
 * - NCFileSelection
 * - NCItemSelector
 * - NCMultiSelectionBox
 * - NCSelectionBox
 *
 * And the table classes from libyui-ncurses-pkg:
 * - NCPkgTable
 * - NCPkgLocaleTable
 * - NCPkgRepoTable
 *
 * So if you change anything here, make sure to test all those classes.
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
     *
     * Most of the keys are now handled in the NCTablePadBase base class or in
     * the individual items' handlers (NCTreeLine, NCTableLine). This method
     * is mostly here as a stub for future extensions.
     *
     * Reimplemented from NCTablePadBase and NCPad.
     **/
    virtual bool handleInput( wint_t key );

    bool setItemByKey( int key );

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
     * An item/line remembers its insertion index.
     *
     * @param id the index before sorting
     **/
    int findIndexById( int id ) const;

    void stripHotkeys();


protected:

    /**
     * Redraw the pad.
     *
     * Reimplemented from NCTablePadBase and NCPad.
     **/
    virtual int  DoRedraw();

    virtual void directDraw( NCursesWindow & w, const wrect at, unsigned lineno );


private:

    // Disable unwanted assignment operator and copy constructor

    NCTablePad & operator=( const NCTablePad & );
    NCTablePad( const NCTablePad & );
};


#endif // NCTablePad_h
