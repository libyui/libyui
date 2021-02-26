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

   File:       NCMenuButton.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCMenuButton_h
#define NCMenuButton_h

#include <iosfwd>

#include <yui/YMenuButton.h>
#include "NCApplication.h"
#include "NCWidget.h"


class NCMenuButton : public YMenuButton, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & str, const NCMenuButton & obj );

    NCMenuButton & operator=( const NCMenuButton & );
    NCMenuButton( const NCMenuButton & );

    NClabel label;
    bool haveUtf8() { return YUI::app()->hasFullUtf8Support(); }

protected:

    virtual const char * location() const { return "NCMenuButton"; }

    virtual void wRedraw();

    NCursesEvent postMenu();

public:

    NCMenuButton( YWidget * parent,
		  std::string label );
    virtual ~NCMenuButton();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void rebuildMenuTree();

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setLabel( const std::string & nlabel );

    virtual void setEnabled( bool do_bv );

    YMenuItem * findItem( int selection ) { return findMenuItem( selection ); }

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    /**
     * Activate the item selected in the tree. Can be used in tests to simulate user input.
     *
     * Derived classes are required to implement this.
     **/
    virtual void activateItem( YMenuItem * item );

};


#endif // NCMenuButton_h
