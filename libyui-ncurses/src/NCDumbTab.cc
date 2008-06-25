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
    defsze.W = firstChild()->preferredWidth();

    vector<NClabel>::iterator listIt = tabList.begin();
    unsigned int tabBarWidth = 0;
    
    while ( listIt != tabList.end() )
    {
	tabBarWidth += (*listIt).width() + 1;
	++listIt;
    }
    ++tabBarWidth;
    
    if ( tabBarWidth > ( unsigned )defsze.W )
	defsze.W = tabBarWidth;

    defsze.W += framedim.Sze.W;

    return defsze.W;
}


int NCDumbTab::preferredHeight()
{
    defsze.H = firstChild()->preferredHeight() + framedim.Sze.H;

    return defsze.H;
}


void NCDumbTab::setEnabled( bool do_bv )
{
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
    YItem * item;

    switch ( key )
    {
	case KEY_LEFT:
	    if ( currentIndex > 0 && currentIndex <= tabList.size() -1 )
	    {
		currentIndex--;
		wRedraw();
	    }
	    break;

	case KEY_RIGHT:
	    if ( currentIndex < tabList.size()-1 && currentIndex >= 0 )
	    {
		currentIndex++;
		wRedraw();
	    }
	    break;

	case KEY_RETURN:
	    ret = NCursesEvent::menu;
	    item = itemAt( currentIndex );
	    if ( item )
	    {
		yuiMilestone() << "Show tab: " << item->label() << endl;
		ret.selection = (YMenuItem *)item;
	    }
	    break;
    }

    return ret;
}

void NCDumbTab::addItem( YItem * item )
{
    YDumbTab::addItem( item );

    NClabel tabLabel = NClabel( item->label() );
    yuiDebug() << "Add item: " << item->label() << endl;
    tabList.push_back( tabLabel );

    if ( item->selected() )
	currentIndex = item->index();
}

void NCDumbTab::selectItem( YItem * item, bool selected )
{
    if ( selected )
    {
	currentIndex = item->index();
	yuiMilestone() << "Select item: " << item->index() << endl;
    }

    YDumbTab::selectItem( item, selected );
    
    wRedraw();
}

void NCDumbTab::wRedraw()
 {
    if ( !win )
	return;
   
    const NCstyle::StWidget & style( widgetStyle(true) );
    win->bkgd( style.plain );
    win->box();

    vector<NClabel>::iterator listIt = tabList.begin();
    int winWidth = win->width() - 2;
    int labelPos = 1;
    unsigned int i = 0;
    bool nonActive = false;
    
    while ( listIt != tabList.end() )
    {
	nonActive = (i == currentIndex)?false:true; 
	(*listIt).drawAt( *win,
			  NCstyle::StWidget( widgetStyle( nonActive) ),
			  wpos( 0, labelPos ),
			  wsze( 1, winWidth ),
			  NC::TOPLEFT, false );

	labelPos += (*listIt).width() + 2;

	++listIt;
	++i;
	
	if ( listIt != tabList.end() )
	{
	    winWidth -= (*listIt).width() -1;
	}
    };
    NCWidget * child = dynamic_cast<NCWidget *>(firstChild());
    child->Redraw();
}
