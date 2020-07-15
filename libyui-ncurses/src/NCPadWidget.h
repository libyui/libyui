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

    friend std::ostream & operator<<( std::ostream & str, const NCPadWidget & obj );

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

    size_t Columns() { return minPadSze.W; }

    void setLabel( const NClabel & nlabel );

    virtual void setEnabled( bool do_bv ) { NCWidget::setEnabled( do_bv ); }
};


#endif // NCPadWidget_h
