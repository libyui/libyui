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

   File:       NCItemSelector.h

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef NCItemSelector_h
#define NCItemSelector_h

#include <iosfwd>
#include <string>
#include <vector>

#include <yui/YItemSelector.h>
#include "NCPadWidget.h"
#include "NCTablePad.h"


class NCItemSelector : public YItemSelector, public NCPadWidget
{
    friend std::ostream & operator<<( std::ostream & str, const NCItemSelector & obj );

public:

    /**
     * Constructor.
     **/
    NCItemSelector( YWidget * parent, bool enforceSingleSelection );

    /**
     * Destructor.
     **/
    virtual ~NCItemSelector();

    /**
     * Return the preferred width for this widget.
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Return the preferred height for this widget.
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the size of this widget.
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Return the current item, i.e. the item that currently has the keyboard
     * focus. Not to be confused with the selected item.
     **/
    virtual YItem * currentItem();

    /**
     * Set the current item, i.e. the item that currently has the keyboard
     * focus.
     **/
    virtual void setCurrentItem( YItem * item );

    /**
     * Handle keyboard input.
     **/
    virtual NCursesEvent wHandleInput( wint_t key );

    /**
     * Enable or disable this widget.
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool do_bv );

    /**
     * Set the keyboard focus to this widget.
     * Reimplemented from YWidget.
     **/
    virtual bool setKeyboardFocus();

    /**
     * Set the number of visible items for this widget.
     * Reimplemented from YItemSelector.
     **/
    virtual void setVisibleItems( int newVal );

    /**
     * Return the number of lines in this widget. This is different from the
     * number of items because each item always has one line for the item
     * label, optionally multiple lines for the description, and optionally a
     * separator line between it and the next item.
     **/
    unsigned int getNumLines() { return myPad()->Lines(); }

    /**
     * Add an item to this widget.
     * Reimplemented from YSelectionWidget.
     **/
    virtual void addItem( YItem * item );

    /**
     * Delete all items.
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deleteAllItems();

    /**
     * Select or deselect an item.
     * Reimplemented from YSelectionWidget.
     **/
    virtual void selectItem( YItem * item, bool selected );

    /**
     * Deselect all items.
     **/
    virtual void deselectAllItems();

    /**
     * Return the text line with the specified line number. Notice that this is
     * different from the item index (see getNumLines()).
     **/
    const NCTableLine * getLine( int lineNo ) { return myPad()->GetLine( lineNo ); }


    virtual void startMultipleChanges() { startMultidraw(); }

    virtual void doneMultipleChanges()	{ stopMultidraw(); }

    virtual const char * location() const { return "NCItemSelector"; }


protected:

    /**
     * Return the desription text for an item. The result may contain newlines.
     **/
    std::string description( YItem * item ) const;

    /**
     * Return the description text for an item as multiple lines.
     **/
    std::vector<std::string> descriptionLines( YItem * item ) const;

    /**
     * Deselect all items except the specified one. This is used for single
     * selection.
     **/
    void deselectAllItemsExcept( YItem * exceptItem );

    /**
     * Return 'true' if an item is selected, 'false' otherwise.
     **/
    bool isItemSelected( YItem *item );

    /**
     * Toggle the status of the current item from 'true' to 'false' and the
     * other way around.
     **/
    void toggleCurrentItem();

    /**
     * Return the preferred size for this widget.
     **/
    virtual wsze preferredSize();

    /**
     * Create the pad for this widget.
     **/
    virtual NCPad * CreatePad();

    /**
     * Return the tag cell (the cell with the "[x]" or "(x)" selector) for the
     * item with the specified index.
     **/
    NCTableTag * tagCell( int index );

    /**
     * Return the tag cell (the cell with the "[x]" or "(x)" selector) for the
     * item with the specified index.
     **/
    const NCTableTag * tagCell( int index ) const;

    /**
     * Return the pad for this widget; overloaded to narrow the type.
     */
    virtual NCTablePad * myPad() const
        { return dynamic_cast<NCTablePad*>( NCPadWidget::myPad() ); }

    virtual void wRecoded() { NCPadWidget::wRecoded(); }


private:

    // Disable assignement operator and copy constructor

    NCItemSelector & operator=( const NCItemSelector & );
    NCItemSelector( const NCItemSelector & );


protected:

    // Data members

    wsze _prefSize;
    bool _prefSizeDirty;
    int  _selectorWidth;
};


#endif // NCItemSelector_h
