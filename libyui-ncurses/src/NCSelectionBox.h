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

   File:       NCSelectionBox.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCSelectionBox_h
#define NCSelectionBox_h

#include <iosfwd>

#include <yui/YSelectionBox.h>
#include "NCPadWidget.h"
#include "NCTablePad.h"


class NCSelectionBox : public YSelectionBox, public NCPadWidget
{

    friend std::ostream & operator<<( std::ostream & str, const NCSelectionBox & obj );

    NCSelectionBox & operator=( const NCSelectionBox & );
    NCSelectionBox( const NCSelectionBox & );

protected:

    /**
     * Overload myPad to narrow the type
     */
    virtual NCTablePad * myPad() const
    { return dynamic_cast<NCTablePad*>( NCPadWidget::myPad() ); }

    bool	  biglist;

protected:

    virtual const char * location() const { return "NCSelectionBox"; }

    virtual NCPad * CreatePad();
    virtual void    wRecoded();

public:

    NCSelectionBox( YWidget * parent, const std::string & label );
    virtual ~NCSelectionBox();

    bool bigList() const { return biglist; }

    void setBigList( bool big ) { biglist = big; }

    virtual void addItem( YItem *item );
    virtual void addItem( const std::string & itemLabel, bool selected = false );

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const std::string & nlabel );

    virtual int getCurrentItem() const;
    virtual void setCurrentItem( int index );

    virtual void selectItem( YItem *item, bool selected );
    virtual void selectItem( int index );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    std::string getLine( int index );
    void clearTable( ) { myPad()->ClearTable(); };

    void deleteAllItems();
};


#endif // NCSelectionBox_h
