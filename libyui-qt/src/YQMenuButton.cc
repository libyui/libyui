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

  File:	      YQMenuButton.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <unistd.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qsize.h>
#include <qtimer.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "YEvent.h"
#include "YQMenuButton.h"


YQMenuButton::YQMenuButton( QWidget * 		parent,
			    YWidgetOpt & 	opt,
			    YCPString 		label )
    : QWidget( parent )
    , YMenuButton( opt, label )
    , _selected_item_index( -1 )
{
    setWidgetRep( this );
    _qt_pushbutton = new QPushButton( fromUTF8( label->value() ), this );
    _qt_pushbutton->setFont( YUIQt::ui()->currentFont() );
    _qt_pushbutton->setMinimumSize( 2,2 );
    _qt_pushbutton->move( YQButtonBorder, YQButtonBorder );
    setMinimumSize( _qt_pushbutton->minimumSize() 
		    + 2 * QSize( YQButtonBorder, YQButtonBorder ) );
}

void
YQMenuButton::setEnabling( bool enabled )
{
    _qt_pushbutton->setEnabled( enabled );
}


long
YQMenuButton::nicesize( YUIDimension dim )
{
    return 2 * YQButtonBorder + ( dim == YD_HORIZ
				  ? _qt_pushbutton->sizeHint().width()
				  : _qt_pushbutton->sizeHint().height() );
}


void
YQMenuButton::setSize( long newWidth, long newHeight )
{
    _qt_pushbutton->resize( newWidth  - 2 * YQButtonBorder,
			    newHeight - 2 * YQButtonBorder );
    resize( newWidth, newHeight );
}


void
YQMenuButton::setLabel( const YCPString & label )
{
    _qt_pushbutton->setText( fromUTF8( label->value() ) );
}


bool
YQMenuButton::setKeyboardFocus()
{
    _qt_pushbutton->setFocus();

    return true;
}


void
YQMenuButton::menuEntryActivated( int menu_item_index )
{
    // y2debug( "Selected menu entry #%d", menu_item_index );
    _selected_item_index = menu_item_index;

    /*
     * Defer the real returnNow() until all popup related events have been
     * processed. This took me some hours to figure out; obviously exit_loop()
     * doesn't have any effect as long as there are still popups open. So be it
     * - use a zero timer to perform the real returnNow() later.
     */

    /*
     * the 100 delay is a ugly dirty workaround
     */
    QTimer::singleShot( 100, this, SLOT( returnNow() ) );

    // YUIQt::ui()->wakeUpGuiThread();
    // YUIQt::ui()->processEvents();
}


void
YQMenuButton::returnNow()
{
    YUIQt::ui()->sendEvent( new YMenuEvent( indexToId( _selected_item_index ) ) );
}


void
YQMenuButton::createMenu()
{
    QPopupMenu * popup = new QPopupMenu( _qt_pushbutton );
    _qt_pushbutton->setPopup( popup );
    connect( popup,	SIGNAL( activated(int) ),
	     this,	SLOT  ( menuEntryActivated(int) ) );

    createMenu( getToplevelMenu(), popup );
}


void
YQMenuButton::createMenu( YMenuItem * ymenu, QPopupMenu * qt_menu )
{
    YMenuItemListIterator it = ymenu->itemList().begin();

    while ( it != ymenu->itemList().end() )
    {
	YMenuItem * item = *it;

	if ( item->isMenu() )
	{
	    QPopupMenu * sub_menu = new QPopupMenu( qt_menu );
	    // y2debug( "Inserting sub menu '%s'", item->getLabel()->value().c_str() );
	    qt_menu->insertItem( fromUTF8( item->getLabel()->value() ), sub_menu );
	    connect( sub_menu,	SIGNAL( activated(int) ),
		     this,	SLOT  ( menuEntryActivated(int) ) );
	    createMenu( item, sub_menu );

	}
	else	// simple menu item, no submenu
	{
	    // y2debug( "Inserting menu item '%s'", item->getLabel()->value().c_str() );
	    qt_menu->insertItem( fromUTF8( item->getLabel()->value() ), item->getIndex() );
	}

	++it;
    }
}


#include "YQMenuButton.moc.cc"
