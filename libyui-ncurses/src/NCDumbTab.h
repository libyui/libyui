/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/



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

    friend std::ostream & operator<<( std::ostream & STREAM, const NCDumbTab & OBJ );

    NCDumbTab & operator=( const NCDumbTab & );
    NCDumbTab( const NCDumbTab & );

    unsigned int currentIndex;
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

    NCursesEvent createMenuEvent( unsigned int index);

    void setCurrentTab( wint_t key );
};


#endif // NCDumbTab_h
