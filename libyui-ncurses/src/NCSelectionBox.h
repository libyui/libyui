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

   File:       NCSelectionBox.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCSelectionBox_h
#define NCSelectionBox_h

#include <iosfwd>

#include "YSelectionBox.h"
#include "NCPadWidget.h"
#include "NCTablePad.h"


class NCSelectionBox : public YSelectionBox, public NCPadWidget
{

    friend std::ostream & operator<<( std::ostream & STREAM, const NCSelectionBox & OBJ );

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

    NCSelectionBox( YWidget * parent, const string & label );
    virtual ~NCSelectionBox();

    bool bigList() const { return biglist; }

    void setBigList( const bool big ) { biglist = big; }

    virtual void addItem( YItem *item );
    virtual void addItem( const string & itemLabel, bool selected = false );

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    virtual int getCurrentItem();
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

    string getLine( const int & index );
    void clearTable( ) { myPad()->ClearTable(); };

    void deleteAllItems();
};


#endif // NCSelectionBox_h
