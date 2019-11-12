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


class NCItemSelectorBase : public YItemSelector, public NCPadWidget
{
    friend std::ostream & operator<<( std::ostream & str, const NCItemSelectorBase & obj );

protected:

    /**
     * Standard constructor.
     **/
    NCItemSelectorBase( YWidget * parent, bool enforceSingleSelection );

    /**
     * Constructor for custom item status values.
     **/
    NCItemSelectorBase( YWidget *			parent,
			const YItemCustomStatusVector & customStates );

public:

    /**
     * Destructor.
     **/
    virtual ~NCItemSelectorBase();

    /**
     * Handle keyboard input.
     **/
    virtual NCursesEvent wHandleInput( wint_t key );

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
    virtual YItem * currentItem() const;

    /**
     * Set the current item, i.e. the item that currently has the keyboard
     * focus.
     **/
    virtual void setCurrentItem( YItem * item );

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
    int linesCount() const { return (int) myPad()->Lines(); }

    /**
     * Return number of the current line, i.e. the line that has the keyboard
     * focus.
     **/
    int currentLine() const { return myPad()->CurPos().L; }

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

    virtual const char * location() const { return "NCItemSelectorBase"; }


protected:

    /**
     * Create a tag cell for an item. This is the cell with the "[x]" or "(x)"
     * selector. It also stores the item pointer so the item can later be
     * referenced by this tag.
     *
     * Derived classes are required to implement this.
     **/
    virtual NCTableTag * createTagCell( YItem * item ) = 0;

    /**
     * Cycle the status of the current item through its possible values.
     * For a plain ItemSelector, this means true -> false -> true.
     *
     * Derived classes are required to implement this.
     **/
    virtual void cycleCurrentItemStatus() = 0;

    /**
     * Notification that a status value was just changed in the input handler
     * and the 'notify' flag is set. The returned event is used as the return
     * value of the input handler (unless it has event type 'none' which is
     * also returned by the default constructor of NCursesEvent), i.e. it is
     * sent to the application.
     *
     * Derived classes are required to implement this.
     **/
    virtual NCursesEvent valueChangedNotify( YItem * item ) = 0;

    /**
     * Return the desription text for an item. The result may contain newlines.
     **/
    std::string description( YItem * item ) const;

    /**
     * Return the description text for an item as multiple lines.
     **/
    std::vector<std::string> descriptionLines( YItem * item ) const;

    /**
     * If the cursor is not on the first line of an item (the line with the
     * "[x]" selector), scroll down to the next line that is the first line of
     * an item.
     **/
    YItem * scrollDownToNextItem();

    /**
     * If the cursor is not on the first line of an item (the line with the
     * "[x]" selector), scroll up to the next line that is the first line of
     * an item.
     **/
    YItem * scrollUpToPreviousItem();

    /**
     * Return the preferred size for this widget.
     **/
    virtual wsze preferredSize();

    /**
     * Return the tag cell (the cell with the "[x]" or "(x)" selector) for the
     * item with the specified index.
     **/
    virtual NCTableTag * tagCell( int index ) const;

    /**
     * Create the pad for this widget.
     **/
    virtual NCPad * CreatePad();

    /**
     * Return the pad for this widget; overloaded to narrow the type.
     */
    virtual NCTablePad * myPad() const
	{ return dynamic_cast<NCTablePad*>( NCPadWidget::myPad() ); }

    virtual void wRecoded() { NCPadWidget::wRecoded(); }


private:

    // Disable assignement operator and copy constructor

    NCItemSelectorBase & operator=( const NCItemSelectorBase & );
    NCItemSelectorBase( const NCItemSelectorBase & );


protected:

    // Data members

    wsze _prefSize;
    bool _prefSizeDirty;
    int	 _selectorWidth;

};	// class NCItemSelectorBase



class NCItemSelector: public NCItemSelectorBase
{
public:
    /**
     * Constructor.
     **/
    NCItemSelector( YWidget * parent, bool enforceSingleSelection );

    /**
     * Destructor.
     **/
    virtual ~NCItemSelector();

    virtual const char * location() const { return "NCItemSelector"; }


protected:

    /**
     * Create a tag cell for an item. This is the cell with the "[x]" or "(x)"
     * selector. It also stores the item pointer so the item can later be
     * referenced by this tag.
     **/
    virtual NCTableTag * createTagCell( YItem * item );

    /**
     * Notification that a status value was just changed in the input handler
     * and the 'notify' flag is set.
     **/
    virtual NCursesEvent valueChangedNotify( YItem * item );

    /**
     * Cycle the status of the current item through its possible values.
     * For a plain ItemSelector, this means true -> false -> true.
     **/
    virtual void cycleCurrentItemStatus();

    /**
     * Deselect all items except the specified one. This is used for single
     * selection.
     **/
    void deselectAllItemsExcept( YItem * exceptItem );


private:

    // Disable assignement operator and copy constructor

    NCItemSelector & operator=( const NCItemSelector & );
    NCItemSelector( const NCItemSelector & );


};      // class NCItemSelector


#endif // NCItemSelector_h
