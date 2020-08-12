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


/**
 * Specialized subclass of NCTableTag that can not only handle a boolean
 * "selected" flag (and accordingly set "[ ]" / "[x]" or "( )" / "(x)" as a
 * status indicator), but extended numeric status values and an assciated text.
 **/
class NCCustomStatusTableTag: public NCTableTag // base class defined in NCTablePad.h
{
public:

    NCCustomStatusTableTag( YItemSelector * parentSelector, YItem * item );
    virtual ~NCCustomStatusTableTag() {}

    virtual void DrawAt( NCursesWindow & w, const wrect at,
			 NCTableStyle & tableStyle,
			 NCTableLine::STATE linestate,
			 unsigned colidx ) const;

    virtual void SetSelected( bool sel );

    virtual bool Selected() const;

    virtual bool SingleSelection() const { return false; }

    /**
     * Return the numeric status value of the associated item.
     **/
    int status() const;

    /**
     * Set the numeric status value of the associated item and update the
     * status indicator.
     **/
    void setStatus( int newStatus );

    /**
     * Update the status indicator according to the status of the associated
     * item, i.e. display the status text for that custom status.
     **/
    void updateStatusIndicator();

protected:

    YItemSelector * _parentSelector;
};



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
     * Return 'true' if a status change (by user interaction) from status
     * 'fromStatus' to status 'toStatus' is allowed, 'false' if not.
     **/
    virtual bool statusChangeAllowed( int fromStatus, int toStatus );

    /**
     * Notification that a status value was just changed in the input handler
     * and the 'notify' flag is set.
     **/
    virtual NCursesEvent valueChangedNotify( YItem * item );

    /**
     * Return the tag cell (the cell with the "[x]" or "(x)" selector) for the
     * item with the specified index.
     **/
    virtual NCCustomStatusTableTag * tagCell( int index ) const;

    /**
     * Update the status indicator.
     * This is only called if custom status values are used.
     *
     * Reimplemented from YItemSelector.
     **/
    virtual void updateCustomStatusIndicator( YItem * item );


private:

    // Disable assignment operator and copy constructor

    NCCustomStatusItemSelector & operator=( const NCCustomStatusItemSelector & );
    NCCustomStatusItemSelector( const NCCustomStatusItemSelector & );
};


#endif // NCCustomStatusItemSelector_h
