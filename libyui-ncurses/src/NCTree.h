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

   File:       NCTree.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCTree_h
#define NCTree_h

#include <iosfwd>

#include <yui/YTree.h>
#include "NCPadWidget.h"
#include "NCTreePad.h"
#include "NCTablePad.h"

class NCTreeLine;


class NCTree : public YTree, public NCPadWidget
{
    friend std::ostream & operator<<( std::ostream & str, const NCTree & obj );

public:

    NCTree( YWidget *           parent,
            const std::string & label,
            bool                multiselection     = false,
            bool                recursiveselection = false );

    virtual ~NCTree();

    /**
     * Recursively build the tree in this widget according to the items.
     *
     * Implemented from YTree.
     **/
    virtual void rebuildTree();

    /**
     * Return a pointer to the current item (the item under the cursor).
     **/
    virtual YTreeItem * getCurrentItem() const;

    /**
     * Get the current item. This is an alias for getCurrentItem.
     *
     * Implemented from YSelectionWidget.
     **/
    virtual YTreeItem * currentItem();

    virtual void deselectAllItems();

    /**
     * Select or deselect an item.
     *
     * Implemented from YSelectionWidget.
     **/
    virtual void selectItem( YItem *item, bool selected );

    /**
     * Select the item with the specified index.
     **/
    virtual void selectItem( int index );

    /**
     * Delete all items and clear the TreePad.
     *
     * Implemented from YSelectionWidget.
     **/
    virtual void deleteAllItems();

    /**
     * libyui geometry management:
     * Return the preferred width for this widget.
     *
     * Implemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * libyui geometry management:
     * Return the preferred height for this widget.
     *
     * Implemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * libyui geometry management:
     * Apply the width and height assigned from the parent layout widget.
     *
     * Implemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Set the label (the caption) above the tree.
     *
     * Implemented from YTree.
     **/
    virtual void setLabel( const std::string & nlabel );

    /**
     * Enable or disable this widget.
     *
     * Implemented from YWidget.
     **/
    virtual void setEnabled( bool do_bv );

    /**
     * Set the keyboard focus to this widget.
     *
     * Implemented from YWidget.
     **/
    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    /**
     * Activate the item selected in the tree.
     * This can be used in tests to simulate user input.
     *
     * Implemented from YSelectionWidget.
     **/
    virtual void activate();

    /**
     * Keyboard input handler.
     *
     * Implemented from NCWidget.
     **/
    virtual NCursesEvent wHandleInput( wint_t key );


protected:

    /**
     * Code location for logging.
     *
     * Implemented from NCWidget.
     **/
    virtual const char * location() const { return "NCTree"; }

    /**
     * Create an empty pad.
     **/
    virtual NCPad * CreatePad();

    /**
     * Return the TreePad that belongs to this widget.
     *
     * Overloaded from NCPadWidget to narrow the type to the actual one used in
     * this widget.
     **/
    virtual NCTreePad * myPad() const
        { return dynamic_cast<NCTreePad*>( NCPadWidget::myPad() ); }

    /**
     * Fill the TreePad with lines (using CreateTreeLines to create them)
     **/
    virtual void DrawPad();


    /**
     * Return a const pointer to the tree line at the specified index for
     * read-only operations.
     **/
    const NCTreeLine * getTreeLine( unsigned idx ) const;

    /**
     * Return a non-const pointer to the tree line at the specified index for
     * read-write operations. This also marks that line as "dirty", i.e. it
     * needs to be updated on the screen.
     **/
    NCTreeLine * modifyTreeLine( unsigned idx );

    /**
     * Optimization for NCurses from libyui:
     * Notification that multiple changes are about to come.
     *
     * Implemented from YWidget.
     **/
    virtual void startMultipleChanges() { startMultidraw(); }

    /**
     * Optimization for NCurses from libyui:
     * Notification that multiple changes are now finished.
     *
     * Implemented from YWidget.
     **/
    virtual void doneMultipleChanges()	{ stopMultidraw(); }

    /**
     * Create TreeLines and append them to the TreePad.
     * If 'item' has any children, this is called recursively for them.
     **/
    void CreateTreeLines( NCTreeLine * parentLine,
                          NCTreePad  * pad,
                          YItem      * item );

private:

    // Disable unwanted assignment operator and copy constructor

    NCTree & operator=( const NCTree & );
    NCTree( const NCTree & );


    //
    // Data members
    //

    bool _multiSelect;
    int  _nextItemIndex; // Only used in CreateTreeLines()
};


/**
 * One line in a tree widdget
 **/
class NCTreeLine : public NCTableLine
{
public:

    NCTreeLine( NCTreeLine * parentLine,
                YTreeItem  * origItem,
                bool         multiSelection );

    virtual ~NCTreeLine();

public:

    /**
     * Return the corresponding YTreeItem.
     **/
    YTreeItem * YItem() const { return dynamic_cast<YTreeItem *>( _yitem ); }

    virtual bool isVisible() const;

    /**
     * Open this tree branch
     **/
    void openBranch();

    /**
     * Close this tree branch
     **/
    void closeBranch();

    /**
     * Toggle the open/closed state of this branch
     **/
    void toggleOpenClosedState();

    /**
     * Change a line that may have been invisible until now to be visible.
     *
     * This also makes the parent lines (and its parent line until the
     * toplevel) visible as well as all sibling lines of this line.
     *
     * Return 'true' if there was a status change, i.e. if it was invisible
     * before, 'false' otherwise.
     *
     * Reimplemented from NCTableLine.
     **/
    virtual bool ChangeToVisible();

    virtual unsigned Hotspot( unsigned & at ) const;

    virtual int	 handleInput( wint_t key );

    virtual void DrawAt( NCursesWindow & w,
                         const wrect     at,
			 NCTableStyle  & tableStyle,
			 bool            active ) const;

    //
    // Some covariants of the tree operations returning this derived class
    //

    virtual NCTreeLine * parent()      const { return dynamic_cast<NCTreeLine *>( _parent      ); }
    virtual NCTreeLine * firstChild()  const { return dynamic_cast<NCTreeLine *>( _firstChild  ); }
    virtual NCTreeLine * nextSibling() const { return dynamic_cast<NCTreeLine *>( _nextSibling ); }


private:

    /**
     * Return a placeholder for the prefix string for this line consisting of
     * enough blanks for the tree hierarchy line graphics and a "[ ] " marker
     * for multiselection if needed.
     *
     * The real line graphics will be drawn over this in DrawAt().
     **/
    std::string prefixStr() const;

    int prefixLen() const { return treeLevel() + 3; }

    //
    // Data members
    //

    mutable chtype * _prefix;
    bool             _multiSelect;
};


#endif // NCTree_h
