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

   File:       NCDumbTab.h

   Author:     Gabriele Strattner <gs@suse.de>

/-*/

#ifndef NCDumbTab_h
#define NCDumbTab_h

#include <iosfwd>

#include <yui/YDumbTab.h>
#include "NCWidget.h"


class NCDumbTab : public YDumbTab, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & str, const NCDumbTab & obj );

    NCDumbTab & operator=( const NCDumbTab & );
    NCDumbTab( const NCDumbTab & );

    unsigned currentIndex;
    wint_t hotKey;

protected:

    virtual const char * location() const { return "NCDumbTab"; }

    virtual void wRedraw();
    void redrawChild( YWidget *widget );

public:

    NCDumbTab( YWidget * parent );
    virtual ~NCDumbTab();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void addItem( YItem * item );
    virtual void selectItem( YItem * item, bool selected );

    virtual void setSize( int newWidth, int newHeight );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual void shortcutChanged();

    virtual bool HasHotkey( int key );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    NCursesEvent createMenuEvent( unsigned index);

    void setCurrentTab( wint_t key );

    /**
    * Activate selected tab. Can be used in tests to simulate user input.
    *
    * Derived classes are required to implement this.
    **/
    virtual void activate();
};


#endif // NCDumbTab_h
