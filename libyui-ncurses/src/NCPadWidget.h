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

   File:       NCPadWidget.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCPadWidget_h
#define NCPadWidget_h

#include <iosfwd>

#include "NCWidget.h"
#include "NCPad.h"

class NCPadWidget;
class NCScrollbar;


class NCPadWidget : public NCWidget, protected NCSchrollCB
{
private:

    friend std::ostream & operator<<( std::ostream & STREAM, const NCPadWidget & OBJ );

    NCPadWidget & operator=( const NCPadWidget & );
    NCPadWidget( const NCPadWidget & );


    NClabel	    label;
    NCursesWindow * padwin;
    NCScrollbar *   hsb;
    NCScrollbar *   vsb;

    wsze  minPadSze;
    bool  multidraw;
    NCPad * pad;

protected:

    /**
     * Return the current pad. Make it virtual so descendant classes
     * can narrow the return type.
     */
    virtual NCPad * myPad() const { return pad; }

    bool    hasHeadline;
    bool    activeLabelOnly;

    void startMultidraw()    { multidraw = true; }

    void stopMultidraw()     { multidraw = false; DrawPad(); }

    bool inMultidraw() const { return multidraw; }

protected:

    virtual const char * location() const { return "NCPadWidget"; }

    // widget stuff
    unsigned labelWidth() const { return label.width(); }

    virtual void wCreate( const wrect & newrect );
    virtual void wDelete();
    virtual void wRedraw();
    virtual void wRecoded();

    // pad stuff
    wsze defPadSze() const
    {
	if ( !padwin )
	    return 0;

	return wsze( padwin->height(), padwin->width() );
    }

    virtual NCPad * CreatePad();
    virtual void    DrawPad();

    void InitPad();
    void AdjustPad( wsze nsze );
    void DelPad();

    // scroll hints
    virtual void HScroll( unsigned total, unsigned visible, unsigned start );
    virtual void VScroll( unsigned total, unsigned visible, unsigned start );
    virtual void ScrollHead( NCursesWindow & w, unsigned ccol );

    // resize hints
    virtual void AdjustPadSize( wsze & minsze );

    // input
    virtual bool handleInput( wint_t key );

public:

    NCPadWidget( NCWidget * myparent = 0 );
    NCPadWidget( YWidget * parent );

    virtual ~NCPadWidget();

    size_t Columns( ) { return minPadSze.W; }

    void setLabel( const NClabel & nlabel );

    virtual void setEnabled( bool do_bv ) { NCWidget::setEnabled( do_bv ); }
};


#endif // NCPadWidget_h
