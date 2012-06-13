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

   File:       NCCheckBoxFrame.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCCheckBoxFrame_h
#define NCCheckBoxFrame_h

#include <iosfwd>

#include <yui/YCheckBoxFrame.h>
#include "NCWidget.h"
#include "NCCheckBox.h"

class NCCheckBoxFrame;


class NCCheckBoxFrame : public YCheckBoxFrame, public NCWidget
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCCheckBoxFrame & OBJ );

    NCCheckBoxFrame & operator=( const NCCheckBoxFrame & );
    NCCheckBoxFrame( const NCCheckBoxFrame & );


    NClabel label;
    bool isEnabled;

protected:

    bool gotBuddy();

    virtual const char * location() const { return "NCCheckBoxFrame"; }

    virtual void wRedraw();

public:

    NCCheckBoxFrame( YWidget * parent, const string & label,
		     bool checked );
    virtual ~NCCheckBoxFrame();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    virtual void setEnabled( bool do_bv );

    virtual bool getValue() { return isEnabled; }

    // Do not forget to call Redraw(), so that UI::ChangeWidget works
    // correctly - bug #301370
    virtual void setValue( bool enable ) { isEnabled = enable; Redraw();}

    virtual bool setKeyboardFocus();

    virtual bool value();

    virtual NCursesEvent wHandleInput( wint_t key );
};


#endif // NCCheckBoxFrame_h
