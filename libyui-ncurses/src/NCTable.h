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

   File:       NCTable.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCTable_h
#define NCTable_h

#include <iosfwd>

#include <yui/YTable.h>
#include "NCPadWidget.h"
#include "NCTablePad.h"


class NCTable : public YTable, public NCPadWidget
{
public:

    NCTable( YWidget * parent, YTableHeader *tableHeader, bool multiSelection = false );

    virtual ~NCTable();

    bool bigList() const { return biglist; }

    void setHeader( vector <string> head );
    void getHeader( vector <string> & head );

    //vector<NCstring> getHeader( ) const { return _header };

    virtual void setAlignment( int col, YAlignmentType al );

    void setBigList( const bool big ) { biglist = big; }

    void SetSepChar( const chtype colSepchar )	{ myPad()->SetSepChar( colSepchar ); }

    void SetSepWidth( const unsigned sepwidth ) { myPad()->SetSepWidth( sepwidth ); }

    void SetHotCol( const int hcol )		{ myPad()->SetHotCol( hcol ); }

    virtual void addItem( YItem *yitem );
    virtual void addItems( const YItemCollection & itemCollection );
    virtual void deleteAllItems( );

    virtual int getCurrentItem();
    YItem * getCurrentItemPointer();

    virtual void setCurrentItem( int index );
    virtual void selectItem( YItem *yitem, bool selected );
    void selectCurrentItem();
    virtual void deselectAllItems();

    virtual int preferredWidth();
    virtual int preferredHeight();

    virtual void setSize( int newWidth, int newHeight );

    virtual void setLabel( const string & nlabel );

    virtual void setEnabled( bool do_bv );

    bool setItemByKey( int key );

    virtual NCursesEvent wHandleInput( wint_t key );

    virtual bool setKeyboardFocus()
    {
	if ( !grabFocus() )
	    return YWidget::setKeyboardFocus();

	return true;
    }

    void stripHotkeys() { myPad()->stripHotkeys(); }

    void setSortStrategy( NCTableSortStrategyBase * newStrategy ) { myPad()->setSortStrategy( newStrategy ); }

protected:

    /**
     * Overload myPad to narrow the type
     */
    virtual NCTablePad * myPad() const
	{ return dynamic_cast<NCTablePad*>( NCPadWidget::myPad() ); }

    bool	  biglist;
    bool 	  multiselect;


protected:

    virtual const char * location() const { return "NCTable"; }

    virtual NCPad * CreatePad();

    virtual void cellChanged( int index, int colnum, const string & newtext );
    virtual void cellChanged( const YTableCell *cell );

    virtual void startMultipleChanges() { startMultidraw(); }
    virtual void doneMultipleChanges()	{ stopMultidraw(); }

    //internal overloaded version of addItem - both addItem(yitem)
    //and addItems(itemCollection) use it, but in different mode
    virtual void addItem( YItem *yitem, bool allAtOnce );
    void toggleCurrentItem();

private:

    vector<NCstring> _header;

    friend std::ostream & operator<<( std::ostream & STREAM, const NCTable & OBJ );

    NCTable & operator=( const NCTable & );
    NCTable( const NCTable & );

};


#endif // NCTable_h
