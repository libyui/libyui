/*
  Copyright (C) 2019 SUSE LLC
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

   File:       NCItemSelector.cc

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#include <boost/algorithm/string.hpp>
#include <vector>

#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCItemSelector.h"

using std::string;



NCItemSelector::NCItemSelector( YWidget * parent, bool enforceSingleSelection )
    : YItemSelector( parent, enforceSingleSelection )
    , NCPadWidget( parent )
{
    yuiDebug() << endl;
    InitPad();
}


NCItemSelector::~NCItemSelector()
{
    yuiDebug() << endl;
}


int NCItemSelector::preferredWidth()
{
    wsze sze = wGetDefsze();

#if 0
    return sze.W > (int)( labelWidth() + 2 ) ? sze.W : ( labelWidth() + 2 );
#endif
    // FIXME: TO DO
    // FIXME: TO DO
    // FIXME: TO DO
    return 42;
}


int NCItemSelector::preferredHeight()
{
#if 0
    return wGetDefsze().H;
#endif
    // FIXME: TO DO
    // FIXME: TO DO
    // FIXME: TO DO

    return 12;
}


void NCItemSelector::setEnabled( bool do_bv )
{
    NCWidget::setEnabled( do_bv );
    YItemSelector::setEnabled( do_bv );
}


void NCItemSelector::setSize( int newwidth, int newheight )
{
    wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
}


YItem * NCItemSelector::currentItem()
{
    if ( !myPad()->Lines() )
	return 0;

    int index = myPad()->CurPos().L;

    return itemAt( index );
}


void NCItemSelector::setCurrentItem( YItem * item )
{
    if ( item )
	myPad()->ScrlLine( item->index() );
}


void NCItemSelector::addItem( YItem * item )
{
    std::vector<NCTableCol*> cells( 2U, 0 );

    if ( item )
    {
        int lineNo = myPad()->Lines();
        yuiMilestone() << "Adding new item " << item->label() << " at line #" << lineNo << endl;

	YItemSelector::addItem( item );
	cells[0] = new NCTableTag( item, item->selected(), enforceSingleSelection() );

	// Do not set style to NCTableCol::PLAIN here, otherwise the current
	// item will not be highlighted if the cursor is not over the widget

	cells[1] = new NCTableCol( item->label() );

        NCTableLine * tableLine = new NCTableLine( cells );
	myPad()->Append( tableLine );

        YDescribedItem * descItem = dynamic_cast<YDescribedItem *>( item );

        if ( descItem )
        {
            string description = descItem->description();

            if ( ! description.empty() )
            {
                std::vector<string> lines;
                boost::split( lines, description, boost::is_any_of( "\n" ) );

                // Add each description line

                for ( const string & line: lines )
                {
                    yuiMilestone() << "  Adding description line:" << line << endl;
                    cells[0] = new NCTableCol( "",   NCTableCol::PLAIN );
                    cells[1] = new NCTableCol( line, NCTableCol::PLAIN );
                    myPad()->Append( cells );
                }
            }

            // Add a blank line after the description as a separator from the next item

            cells.clear();
            cells[0] = new NCTableCol( "",   NCTableCol::SEPARATOR );
            cells[1] = new NCTableCol( "",   NCTableCol::SEPARATOR );
            myPad()->Append( cells );
        }

	DrawPad();
    }

    yuiMilestone() << "Finished" << endl;
}


/**
 * Return pointer to current line tag
 * (holds state and YItem pointer)
 **/
NCTableTag * NCItemSelector::tagCell( int index )
{
    NCTableLine * tableLine = myPad()->ModifyLine( index );

    if ( ! tableLine )
	return 0;

    return dynamic_cast<NCTableTag *> ( tableLine->GetCol( 0 ) );
}


const NCTableTag * NCItemSelector::tagCell( int index ) const
{
    const NCTableLine * tableLine = myPad()->GetLine( index );

    if ( ! tableLine )
	return 0;

    return dynamic_cast<const NCTableTag *>( tableLine->GetCol( 0 ) );
}


void NCItemSelector::deleteAllItems()
{
    YItemSelector::deleteAllItems();
    myPad()->ClearTable();
    DrawPad();
}


bool NCItemSelector::isItemSelected( YItem *item )
{
    if ( item )
	return item->selected();
    else
	return false;
}


void NCItemSelector::selectItem( YItem *yitem, bool selected )
{
    if ( yitem )
    {
	YItemSelector::selectItem( yitem, selected );

        yuiMilestone() << "Fetching data()" << endl;
	// retrieve pointer to the line tag associated with this item
	NCTableTag * tag = (NCTableTag *) yitem->data();
	YUI_CHECK_PTR( tag );
        yuiMilestone() << "  got data()" << endl;

	tag->SetSelected( selected );

	DrawPad();
    }
}


void NCItemSelector::deselectAllItems()
{
    YItemSelector::deselectAllItems();

    for ( unsigned int i = 0; i < getNumLines(); i++ )
    {
	NCTableTag * tag = tagCell( i );

        if ( tag )
            tag->SetSelected( false );
    }

    DrawPad();
}


void NCItemSelector::toggleCurrentItem()
{
    YItem *it = currentItem();
    if ( it )
	selectItem( it, !( it->selected() ) );
}


/**
 * Create empty MsB pad
 **/
NCPad * NCItemSelector::CreatePad()
{
    wsze psze( defPadSze() );
    NCTablePad * npad = new NCTablePad( psze.H, psze.W, *this );
    npad->bkgd( listStyle().item.plain );
    npad->SetSepChar( ' ' );
    return npad;
}


void NCItemSelector::wRecoded()
{
    NCPadWidget::wRecoded();
}


NCursesEvent NCItemSelector::wHandleInput( wint_t key )
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
