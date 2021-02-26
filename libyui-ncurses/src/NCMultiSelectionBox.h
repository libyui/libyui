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

   File:       NCMultiSelectionBox.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCMultiSelectionBox_h
#define NCMultiSelectionBox_h

#include <iosfwd>

#include <yui/YMultiSelectionBox.h>
#include "NCPadWidget.h"
#include "NCTablePad.h"






class NCMultiSelectionBox : public YMultiSelectionBox, public NCPadWidget
{

    friend std::ostream & operator<<( std::ostream & str, const NCMultiSelectionBox & obj );

    NCMultiSelectionBox & operator=( const NCMultiSelectionBox & );
    NCMultiSelectionBox( const NCMultiSelectionBox & );

protected:

    /**
     * Overload myPad to narrow the type
     */
    virtual NCTablePad * myPad() const
    { return dynamic_cast<NCTablePad*>( NCPadWidget::myPad() ); }

    NCTableTag * tagCell( int index );
    const NCTableTag * tagCell( int index ) const;

    bool isItemSelected( YItem *item );

    void toggleCurrentItem();

public:

    virtual void startMultipleChanges() { startMultidraw(); }

    virtual void doneMultipleChanges()	{ stopMultidraw(); }

    virtual const char * location() const { return "NCMultiSelectionBox"; }

    virtual void addItem( YItem * item );

    virtual void deleteAllItems();

    virtual void selectItem( YItem * item, bool selected );

    virtual void deselectAllItems();

protected:

    virtual NCPad * CreatePad();
    virtual void    wRecoded();


public:

    NCMultiSelectionBox( YWidget * parent, const std::string & label );
    virtual ~NCMultiSelectionBox();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const std::string & nlabel );

    virtual YItem * currentItem();
    virtual void setCurrentItem( YItem * item );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual void setEnabled( bool do_bv );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    unsigned int getNumLines( ) { return myPad()->Lines(); }

    const NCTableLine * getLine( int index ) { return myPad()->GetLine( index ); }

    void clearItems() { return myPad()->ClearTable(); }
};


#endif // NCMultiSelectionBox_h
