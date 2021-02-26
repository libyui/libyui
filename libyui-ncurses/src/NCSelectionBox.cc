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

   File:       NCSelectionBox.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCSelectionBox.h"



NCSelectionBox::NCSelectionBox( YWidget * parent, const std::string & nlabel )
	: YSelectionBox( parent, nlabel )
	, NCPadWidget( parent )
	, biglist( false )
{
    yuiDebug() << std::endl;
    InitPad();
    setLabel( nlabel );
}


NCSelectionBox::~NCSelectionBox()
{
    yuiDebug() << std::endl;
}


int NCSelectionBox::preferredWidth()
{
    wsze sze = ( biglist ) ? myPad()->tableSize() + 2 : wGetDefsze();
    return sze.W > ( int )( labelWidth() + 2 ) ? sze.W : ( labelWidth() + 2 );
}


int NCSelectionBox::preferredHeight()
{
    wsze sze = ( biglist ) ? myPad()->tableSize() + 2 : wGetDefsze();
    return sze.H;
}


void NCSelectionBox::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


void NCSelectionBox::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YSelectionBox::setEnabled( do_bv );
}


int NCSelectionBox::getCurrentItem() const
{
    if ( !myPad()->Lines() )
	return -1;

    yuiDebug() << "Current pos: " << myPad()->CurPos().L << std::endl;

    return myPad()->CurPos().L;
}


std::string NCSelectionBox::getLine( int index )
{
    NCTableLine * line = const_cast<NCTableLine*>( myPad()->GetLine( index ) );
    NCTableCol * value;
    std::string val;

    if ( line->Cols() == 1 )
    {
	value = line->GetItems()[0];
	const NClabel label = value->Label();
	const std::list<NCstring> text = label.getText();
	std::list<NCstring>::const_iterator it = text.begin();

	while ( it != text.end() )
	{
	    val += ( *it ).Str();
	    ++it;
	}
    }

    return val;
}


void NCSelectionBox::setCurrentItem( int index )
{
    myPad()->ScrlLine( index );
}


void NCSelectionBox::selectItem( YItem *item, bool selected )
{
    YSelectionBox::selectItem( item, selected );

    myPad()->ScrlLine( selected ? item->index() : -1 );
}


void NCSelectionBox::selectItem( int index )
{
    YSelectionBox::deselectAllItems();

    if ( hasItems() && index >= 0 )
    {
	YItem * item = YSelectionBox::itemAt( index );

	if ( item )
	{
	    yuiDebug() << "selectItem:	" << item->label().c_str() << std::endl;
	    item->setSelected( true );
	}
	else
	    YUI_THROW( YUIException( "Can't find selected item" ) );
    }
}


void NCSelectionBox::addItem( YItem * item )
{
    std::vector<NCTableCol*> Items( 1U, 0 );

    if ( item )
    {
	YSelectionBox::addItem( item );
	Items[0] = new NCTableCol( item->label() );
	myPad()->Append( Items );
	DrawPad();

	if ( item->selected() )
	    myPad()->ScrlLine( myPad()->Lines() );
    }
}


void NCSelectionBox::addItem( const std::string & description, bool selected )
{
    YSelectionWidget::addItem( description, selected );
}


void NCSelectionBox::setLabel( const std::string & nlabel )
{
    YSelectionBox::setLabel( nlabel );
    NCPadWidget::setLabel( NCstring( nlabel ) );
}


NCPad * NCSelectionBox::CreatePad()
{
    wsze    psze( defPadSze() );
    NCPad * npad = new NCTablePad( psze.H, psze.W, *this );
    npad->bkgd( listStyle().item.plain );

    return npad;
}


void NCSelectionBox::wRecoded()
{
    NCPadWidget::wRecoded();
}


NCursesEvent NCSelectionBox::wHandleInput( wint_t key )
{
    NCursesEvent ret = NCursesEvent::none;

    int oldItem = getCurrentItem();

    // handle key event first

    if ( sendKeyEvents() &&
	 ( key == KEY_LEFT || key == KEY_RIGHT ) )
    {
	ret = NCursesEvent::key;

	switch ( key )
	{
	    case KEY_LEFT:
		ret.keySymbol = "CursorLeft";
		break;

	    case KEY_RIGHT:
		ret.keySymbol = "CursorRight";
		break;
	}

	return ret;
    }

    // call handleInput of NCPad
    handleInput( key );

    int citem = getCurrentItem();

    selectItem( citem );

    switch ( key )
    {
	case KEY_SPACE:
	case KEY_RETURN:

	    if ( notify() && citem != -1 )
	    {
		return NCursesEvent::Activated;
	    }

	    break;
    }

    if ( notify() && immediateMode() && oldItem != citem )
    {
	ret = NCursesEvent::SelectionChanged;
    }

    return ret;
}


/**
 * Clear the table and the lists holding the values
 **/
void NCSelectionBox::deleteAllItems()
{
    YSelectionBox::deleteAllItems();
    clearTable();
    DrawPad();
}
