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

   File:       NCDumbTab.cc

   Author:     Gabriele Mohr <gs@suse.de>

/-*/

#define  YUILogComponent "ncurses"
#include <YUILog.h>
#include "YDialog.h"
#include "NCDialog.h"
#include "NCurses.h"
#include "NCDumbTab.h"
#include "NCPopupList.h"


NCDumbTab::NCDumbTab( YWidget * parent )
	: YDumbTab( parent )
	, NCWidget( parent )
	, currentIndex( 0 )
{
    framedim.Pos = wpos( 1 );
    framedim.Sze = wsze( 2 );
}


NCDumbTab::~NCDumbTab()
{
    yuiDebug() << endl;
}


int NCDumbTab::preferredWidth()
{
    defsze.W = hasChildren() ? firstChild()->preferredWidth() : 0;

    YItemIterator listIt = itemsBegin();
    
    unsigned int tabBarWidth = 0;
    NClabel tabLabel;
    
    while ( listIt != itemsEnd() )
    {
	tabLabel = NClabel( (*listIt)->label() );
	tabBarWidth += tabLabel.width() + 1;
	++listIt;
    }
    ++tabBarWidth;
    
    if ( tabBarWidth > ( unsigned )defsze.W )
	defsze.W = tabBarWidth;

    defsze.W += framedim.Sze.W;

    if ( defsze.W > NCurses::cols() )
	defsze.W = NCurses::cols();

    return defsze.W;
}


int NCDumbTab::preferredHeight()
{
    defsze.H  = hasChildren() ? firstChild()->preferredHeight() : 0;
    defsze.H += framedim.Sze.H;

    return defsze.H;
}


void NCDumbTab::setEnabled( bool do_bv )
{
    yuiDebug() << "Set enabled" << endl;
    NCWidget::setEnabled( do_bv );
    YDumbTab::setEnabled( do_bv );
}


void NCDumbTab::setSize( int newwidth, int newheight )
{
    wsze csze( newheight, newwidth );
    wRelocate( wpos( 0 ), csze );
    csze = wsze::max( 0, csze - framedim.Sze );

    if ( hasChildren() )
	firstChild()->setSize( csze.W, csze.H );
}

NCursesEvent NCDumbTab::wHandleInput( wint_t key )
{
    NCursesEvent ret = NCursesEvent::none;

    switch ( key )
    {
	case KEY_LEFT:
	    if ( currentIndex > 0 &&
		 currentIndex <= (unsigned)itemsCount() -1 )
	    {
		currentIndex--;
		wRedraw();

		ret = createMenuEvent( currentIndex );
	    }
	    break;

	case KEY_RIGHT:
	    if ( currentIndex < (unsigned)itemsCount()-1 &&
		 currentIndex >= 0 )
	    {
		currentIndex++;
		wRedraw();

		ret = createMenuEvent( currentIndex );
	    }
	    break;

	case KEY_HOTKEY:
	    setCurrentTab( hotKey );

	case KEY_RETURN:
	    ret = createMenuEvent( currentIndex );
	    break;

    }

    return ret;
}

void NCDumbTab::setCurrentTab( wint_t key )
{
        
    YItemIterator listIt = itemsBegin();
    NClabel tablabel;
    unsigned int i = 0;
    
    while ( listIt != itemsEnd() )
    {
	tablabel = NCstring( (*listIt)->label() );
	tablabel.stripHotkey();
	yuiDebug() << "HOTkey: " <<  tablabel.hotkey() << " key: " << key << endl;
	if ( tolower ( tablabel.hotkey() )  == tolower ( key ) )
	{
	    currentIndex = i;
	    break;
	}
	++listIt;
	++i;
    }
}

NCursesEvent NCDumbTab::createMenuEvent( unsigned int index )
{
    NCursesEvent ret = NCursesEvent::menu;
    YItem * item;

    item = itemAt( index );
    if ( item )
    {
	yuiMilestone() << "Show tab: " << item->label() << endl;
	ret.selection = (YMenuItem *)item;
    }
    
    return ret;
}

void NCDumbTab::addItem( YItem * item )
{
    YDumbTab::addItem( item );

    NClabel tabLabel = NCstring( item->label() );
    yuiDebug() << "Add item: " << item->label() << endl;

    if ( item->selected() )
	currentIndex = item->index();
}

void NCDumbTab::selectItem( YItem * item, bool selected )
{
    if ( selected )
    {
	currentIndex = item->index();
	yuiDebug() << "Select item: " << item->index() << endl;
    }

    YDumbTab::selectItem( item, selected );
    
    wRedraw();
}

void NCDumbTab::shortcutChanged()
{
    // Any of the items might have its keyboard shortcut changed, but we don't
    // know which one. So let's simply set all tab labels again.
    
    wRedraw();
}

void NCDumbTab::wRedraw()
 {
    if ( !win )
	return;

    const NCstyle::StWidget & style( widgetStyle(true) );
    win->bkgd( style.plain );
    win->box();

    YItemIterator listIt = itemsBegin();
    
    int winWidth = win->width() - 2;
    unsigned int labelPos = 1;
    unsigned int i = 0;
    bool nonActive = false;
    NClabel tablabel;
    
    while ( listIt != itemsEnd() )
    {
	tablabel = NCstring( (*listIt)->label() );
	tablabel.stripHotkey();
	hotlabel = &tablabel;

	nonActive = (i == currentIndex)?false:true;

	if ( GetState() == NC::WSactive )
	{

	    tablabel.drawAt( *win,
			      NCstyle::StWidget( widgetStyle( nonActive) ),
			      wpos( 0, labelPos ),
			      wsze( 1, winWidth ),
			      NC::TOPLEFT, false );
	}
	else
	{
	    if ( !nonActive )
	    {
		tablabel.drawAt( *win,
				  widgetStyle( ).data,
				  widgetStyle( ).data,
				  wpos( 0, labelPos ),
				  wsze( 1, winWidth ),
				  NC::TOPLEFT, false );
	    }
	    else
	    {
		tablabel.drawAt( *win,
				  NCstyle::StWidget( frameStyle() ),
				  wpos( 0, labelPos ),
				  wsze( 1, winWidth ),
				  NC::TOPLEFT, false );	
	    }
	}

	labelPos += tablabel.width() + 2;

	++listIt;
	++i;
	
	if ( listIt != itemsEnd() )
	{
	    winWidth -= tablabel.width() -1;
	}
    };
    
    if ( firstChild() )
    {
	NCWidget * child = dynamic_cast<NCWidget *>( firstChild() );
	
	if ( child )
	    child->Redraw();

	redrawChild( firstChild() );
    }
}

bool NCDumbTab::HasHotkey( int key )
{
    bool ret = false;
    
    YItemIterator listIt = itemsBegin();
    NClabel tablabel;
    
    while ( listIt != itemsEnd() )
    {
	tablabel = NCstring( (*listIt)->label() );
	tablabel.stripHotkey();
	if ( tablabel.hasHotkey() && tolower ( tablabel.hotkey() ) == tolower ( key ) )
	{
	    hotKey = tolower ( key ) ;
	    ret = true;
	}
	++listIt;
    }

    yuiDebug() << "Has hot key: " << key << " " << (ret?"yes":"no") << endl;
    
    return ret;
}

void NCDumbTab::redrawChild( YWidget *widget )
{
    NCWidget * child;

    if ( widget->hasChildren() )
    {
	YWidgetListConstIterator widgetIt = widget->childrenBegin();
	while ( widgetIt != widget->childrenEnd() )
	{
	    child = dynamic_cast<NCWidget *>(*widgetIt);
	    if ( child )
		child->Redraw();
	    redrawChild( *widgetIt );
	    ++widgetIt;
	}
    }
}
