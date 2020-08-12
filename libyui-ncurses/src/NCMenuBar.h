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

   File:       NCMenuBar.h

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef NCMenuBar_h
#define NCMenuBar_h

#include <yui/YMenuBar.h>
#include "NCWidget.h"
#include "CyclicContainer.h"


class NCMenuBar: public YMenuBar, public NCWidget
{
public:
    /**
     * Constructor.
     **/
    NCMenuBar( YWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~NCMenuBar();

    /**
     * Rebuild the displayed menu tree from the internally stored YMenuItems.
     *
     * Implemented from YMenuWidget.
     **/
    virtual void rebuildMenuTree();

    /**
     * Enable or disable an item.
     *
     * Reimplemented from YMenuWidget.
     **/
    virtual void setItemEnabled( YMenuItem * item, bool enabled );

    /**
     * Support for the Rest API for UI testing:
     *
     * Activate the item selected in the tree.
     * This can be used in tests to simulate user input.
     *
     * Implemented from YMenuWidget.
     **/
    virtual void activateItem( YMenuItem * item );

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
     * Enable or disable this widget.
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Set the keyboard focus to this widget.
     * Reimplemented from YWidget.
     **/
    virtual bool setKeyboardFocus();

    virtual NCursesEvent wHandleHotkey( wint_t key );

    virtual bool HasHotkey( int key ) ;

protected:

    /**
     * Clear all content.
     **/
    void clear();

    virtual const char * location() const { return "NCMenuBar"; }

    virtual void wRedraw();

    NCursesEvent postMenu();

private:

    struct Menu;

    friend std::ostream & operator<<( std::ostream & str,
                                      const NCMenuBar & obj );

    // Disable assignment operator and copy constructor

    NCMenuBar & operator=( const NCMenuBar & );
    NCMenuBar( const NCMenuBar & );

    NCursesEvent handlePostMenu( const NCursesEvent & event, int selectedIndex );

    Menu * selectedMenu();

    void selectNextMenu();
    void selectPreviousMenu();

    CyclicContainer<Menu>::Iterator findMenuWithHotkey( wint_t key );

    const NCstyle::StWidget & menuStyle( const Menu * menu );

    CyclicContainer<Menu> _menus;

};      // NCMenuBar

#endif  // NCMenuBar_h
