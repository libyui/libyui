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

   File:       NCCustomStatusItemSelector.cc

   Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define	 YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include "NCLabel.h"
#include "NCCustomStatusItemSelector.h"

using std::string;


NCCustomStatusItemSelector::NCCustomStatusItemSelector( YWidget * parent,
                                                        const YItemCustomStatusVector & customStates )
    : NCItemSelectorBase( parent, customStates )
{
    yuiDebug() << endl;
}


NCCustomStatusItemSelector::~NCCustomStatusItemSelector()
{
    yuiDebug() << endl;
}


NCTableTag * NCCustomStatusItemSelector::createTagCell( YItem * item )
{
    NCTableTag * tag = new NCCustomStatusTableTag( this, item );
    YUI_CHECK_NEW( tag );

    return tag;
}


NCCustomStatusTableTag *
NCCustomStatusItemSelector::tagCell( int index ) const
{
    NCTableTag * tag = NCItemSelectorBase::tagCell( index );

    return tag ? dynamic_cast<NCCustomStatusTableTag *>( tag ) : 0;
}


void NCCustomStatusItemSelector::updateCustomStatusIndicator( YItem * item )
{
    if ( ! item )
        return;

    yuiMilestone() << "Updating status indicator for \"" << item->label() << "\"" << endl;
    NCCustomStatusTableTag * tag = (NCCustomStatusTableTag *) item->data();
    YUI_CHECK_PTR( tag );

    tag->updateStatusIndicator();
    DrawPad();
}


void NCCustomStatusItemSelector::cycleCurrentItemStatus()
{
    YItem *item = currentItem();

    if ( item )
    {
        int oldStatus = item->status();
        int newStatus = customStatus( oldStatus ).nextStatus();

        yuiMilestone() << "Cycling status of item \""
                       << item->label() << "\": "
                       << oldStatus << " -> " << newStatus
                       << endl;

        if ( newStatus != -1 && oldStatus != newStatus )
        {
            item->setStatus( newStatus );
            updateCustomStatusIndicator( item );
        }
    }
}


NCursesEvent
NCCustomStatusItemSelector::valueChangedNotify( YItem * item )
{
    NCursesEvent event( NCursesEvent::menu );
    event.selection = (YMenuItem *) item;

    return event;
}


// ----------------------------------------------------------------------


NCCustomStatusTableTag::NCCustomStatusTableTag( YItemSelector * parentSelector, YItem * item )
    : NCTableTag( item )
    , _parentSelector( parentSelector )
{
    YUI_CHECK_PTR( _parentSelector );
    updateStatusIndicator();
}


void NCCustomStatusTableTag::updateStatusIndicator()
{
    YItem * item = origItem();

    if ( item )
    {
        string statusText = _parentSelector->customStatus( item->status() ).textIndicator();
yuiMilestone() << "New status text: \"" << statusText << "\"" << endl;

        // Since the parent class overwrote SetLabel() to do nothing,
        // we need to go one class up the class hierarchy to set the text.
        NCTableCol::SetLabel( NCstring( statusText ) );
    }
}


void NCCustomStatusTableTag::DrawAt( NCursesWindow & w,
                                     const wrect at,
                                     NCTableStyle & tableStyle,
                                     NCTableLine::STATE linestate,
                                     unsigned colidx ) const
{
    // Undo the parent class's overwriting DrawAt():
    // Call the next superclass in the class hierarchy
    NCTableCol::DrawAt( w, at, tableStyle, linestate, colidx );
}


int NCCustomStatusTableTag::status() const
{
    YItem * item = origItem();

    return item ? item->status() : 0;
}


void NCCustomStatusTableTag::setStatus( int newStatus )
{
    YItem * item = origItem();

    if ( item )
    {
        item->setStatus( newStatus );
        updateStatusIndicator();
    }
}


void NCCustomStatusTableTag::SetSelected( bool sel )
{
    setStatus( sel ? 1 : 0 );
}


bool NCCustomStatusTableTag::Selected() const
{
    return status() != 0;
}

