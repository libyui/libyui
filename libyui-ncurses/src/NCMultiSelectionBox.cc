/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
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
|***************************************************************************/

/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCMultiSelectionBox.cc

   Author:     Michael Andres <ma@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "NCMultiSelectionBox.h"


NCMultiSelectionBox::NCMultiSelectionBox( YWidget * parent,
					  const string & nlabel )
	: YMultiSelectionBox( parent, nlabel )
	, NCPadWidget( parent )
{
    yuiDebug() << endl;
    InitPad();
    setLabel( nlabel );
}


NCMultiSelectionBox::~NCMultiSelectionBox()
{
    yuiDebug() << endl;
}


int NCMultiSelectionBox::preferredWidth()
{
    wsze sze = wGetDefsze();
    return sze.W > ( int )( labelWidth() + 2 ) ? sze.W : ( labelWidth() + 2 );
}


int NCMultiSelectionBox::preferredHeight()
{
    return wGetDefsze().H;
}


void NCMultiSelectionBox::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YMultiSelectionBox::setEnabled( do_bv );
}


void NCMultiSelectionBox::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


YItem * NCMultiSelectionBox::currentItem()
{
    if ( !myPad()->Lines() )
	return 0;

    int index = myPad()->CurPos().L;

    return itemAt( index );
}



void NCMultiSelectionBox::setCurrentItem( YItem * item )
{
    if ( item )
	myPad()->ScrlLine( item->index() );
}

void NCMultiSelectionBox::addItem( YItem * item )
{
    vector<NCTableCol*> Items( 2U, 0 );

    if ( item )
    {
	YMultiSelectionBox::addItem( item );
	Items[0] = new NCTableTag( item, item->selected() );
	
	// Do not set style to NCTableCol::PLAIN here, otherwise the current
	//item will not be highlighted if the cursor is not over the widget
	
	Items[1] = new NCTableCol( item->label() );
	myPad()->Append( Items );
	DrawPad();
    }
}


/**
 * Return pointer to current line tag 
 * (holds state and yitem pointer)
 **/
NCTableTag * NCMultiSelectionBox::tagCell( int index )
{
    NCTableLine * cl = myPad()->ModifyLine( index );

    if ( !cl )
	return 0;

    return static_cast<NCTableTag *>( cl->GetCol( 0 ) );
}


const NCTableTag * NCMultiSelectionBox::tagCell( int index ) const
{
    const NCTableLine * cl = myPad()->GetLine( index );

    if ( !cl )
	return 0;

    return static_cast<const NCTableTag *>( cl->GetCol( 0 ) );
}



void NCMultiSelectionBox::deleteAllItems()
{
    YMultiSelectionBox::deleteAllItems();
    myPad()->ClearTable();
    DrawPad();
}


bool NCMultiSelectionBox::isItemSelected( YItem *item )
{
    if ( item )
	return item->selected();
    else
	return false;
}


void NCMultiSelectionBox::selectItem( YItem *yitem, bool selected )
{
    if ( yitem )
    {
	YMultiSelectionBox::selectItem( yitem, selected );

	//retrieve pointer to the line tag associated with this item
	NCTableTag * tag = ( NCTableTag * )yitem->data();
	YUI_CHECK_PTR( tag );

	tag->SetSelected( selected );

	DrawPad();
    }
}


void NCMultiSelectionBox::deselectAllItems()
{
    YMultiSelectionBox::deselectAllItems();

    for ( unsigned int i = 0; i < getNumLines(); i++ )
    {
	NCTableTag *t = tagCell( i );
	YUI_CHECK_PTR( t );

	t->SetSelected( false );
    }

    DrawPad();
}



/**
 * Toggle item from selected -> deselected and vice versa
 **/
void NCMultiSelectionBox::toggleCurrentItem()
{
    YItem *it = currentItem();
    if ( it )
	selectItem( it, !( it->selected() ) );
}


void NCMultiSelectionBox::setLabel( const string & nlabel )
{
    YMultiSelectionBox::setLabel( nlabel );
    NCPadWidget::setLabel( NCstring( nlabel ) );
}


/**
 * Create empty MsB pad
 **/
NCPad * NCMultiSelectionBox::CreatePad()
{
    wsze psze( defPadSze() );
    NCTablePad * npad = new NCTablePad( psze.H, psze.W, *this );
    npad->bkgd( listStyle().item.plain );
    npad->SetSepChar( ' ' );
    return npad;
}


void NCMultiSelectionBox::wRecoded()
{
    NCPadWidget::wRecoded();
}


NCursesEvent NCMultiSelectionBox::wHandleInput( wint_t key )
{
    NCursesEvent ret;
    bool valueChanged = false;
    YItem *oldCurrentItem = currentItem();

    if ( ! handleInput( key ) )
    {
	YItem *citem = currentItem();

	switch ( key )
	{
	    case KEY_SPACE:

	    case KEY_RETURN:
		toggleCurrentItem();
		valueChanged = true;
		break;

	    case '+':

		if ( !isItemSelected( citem ) )
		{
		    selectItem( citem, true );
		    valueChanged = true;
		}

		myPad()->ScrlDown();

		break;

	    case '-':

		if ( isItemSelected( citem ) )
		{
		    selectItem( citem, false );
		    valueChanged = true;
		}

		myPad()->ScrlDown();

		break;
	}
    }

    if ( notify() )
    {
	if ( valueChanged )
	    ret = NCursesEvent::ValueChanged;
	else if ( oldCurrentItem != currentItem() )
	    ret = NCursesEvent::SelectionChanged;
    }

    return ret;
}

