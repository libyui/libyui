/*************************************************************************************************************

 Copyright (C) 2000 - 2010 Novell, Inc.   All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation; either version 2 of the License, or (at your
 option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*************************************************************************************************************/



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////   __/\\\\\\_____________/\\\__________/\\\________/\\\___/\\\________/\\\___/\\\\\\\\\\\_           ////
 ////    _\////\\\____________\/\\\_________\///\\\____/\\\/___\/\\\_______\/\\\__\/////\\\///__          ////
 ////     ____\/\\\______/\\\__\/\\\___________\///\\\/\\\/_____\/\\\_______\/\\\______\/\\\_____         ////
 ////      ____\/\\\_____\///___\/\\\_____________\///\\\/_______\/\\\_______\/\\\______\/\\\_____        ////
 ////       ____\/\\\______/\\\__\/\\\\\\\\\_________\/\\\________\/\\\_______\/\\\______\/\\\_____       ////
 ////        ____\/\\\_____\/\\\__\/\\\////\\\________\/\\\________\/\\\_______\/\\\______\/\\\_____      ////
 ////         ____\/\\\_____\/\\\__\/\\\__\/\\\________\/\\\________\//\\\______/\\\_______\/\\\_____     ////
 ////          __/\\\\\\\\\__\/\\\__\/\\\\\\\\\_________\/\\\_________\///\\\\\\\\\/_____/\\\\\\\\\\\_    ////
 ////           _\/////////___\///___\/////////__________\///____________\/////////______\///////////__   ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                 widget abstraction library providing Qt, GTK and ncurses frontends                  ////
 ////                                                                                                     ////
 ////                                   3 UIs for the price of one code                                   ////
 ////                                                                                                     ////
 ////                                      ***  NCurses plugin  ***                                       ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                                                     ////
 ////                                                                              (C) SUSE Linux GmbH    ////
 ////                                                                                                     ////
 ////                                                              libYUI-AsciiArt (C) 2012 Björn Esser   ////
 ////                                                                                                     ////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-/

   File:       NCMultiSelectionBox.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCMultiSelectionBox_h
#define NCMultiSelectionBox_h

#include <iosfwd>

#include "YMultiSelectionBox.h"
#include "NCPadWidget.h"
#include "NCTablePad.h"






class NCMultiSelectionBox : public YMultiSelectionBox, public NCPadWidget
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCMultiSelectionBox & OBJ );

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

    NCMultiSelectionBox( YWidget * parent, const string & label );
    virtual ~NCMultiSelectionBox();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

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

    const NCTableLine * getLine( const int & index ) { return myPad()->GetLine( index ); }

    void clearItems() { return myPad()->ClearTable(); }
};


#endif // NCMultiSelectionBox_h
