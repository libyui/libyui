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

   File:       NCTree.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCTree_h
#define NCTree_h

#include <iosfwd>

#include "YTree.h"
#include "NCPadWidget.h"
#include "NCTreePad.h"
#include "NCTablePad.h"

class NCTreeLine;


class NCTree : public YTree, public NCPadWidget
{
private:
    friend std::ostream & operator<<( std::ostream & STREAM, const NCTree & OBJ );

    NCTree & operator=( const NCTree & );
    NCTree( const NCTree & );

    int idx;
    bool multiSel;
    
    void CreateTreeLines( NCTreeLine * p, NCTreePad * pad, YItem * item );

protected:

    virtual NCTreePad * myPad() const
    { return dynamic_cast<NCTreePad*>( NCPadWidget::myPad() ); }

    const NCTreeLine * getTreeLine( unsigned idx ) const;
    NCTreeLine *       modifyTreeLine( unsigned idx );

    virtual const char * location() const { return "NCTree"; }

    virtual NCPad * CreatePad();
    virtual void    DrawPad();

    virtual void startMultipleChanges() { startMultidraw(); }
    virtual void doneMultipleChanges()	{ stopMultidraw(); }

public:

    NCTree( YWidget * parent, const string & label, bool multiselection=false, bool recursiveselection=false );
    virtual ~NCTree();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );
    virtual void rebuildTree();

    virtual YTreeItem * getCurrentItem() const;

    virtual YTreeItem * currentItem();

    virtual void deselectAllItems();
    
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

    void deleteAllItems();
};


#endif // NCTree_h
