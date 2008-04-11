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


#include <qpushbutton.h>
#include <QMenu>
#include <qsize.h>
#include <qtimer.h>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "utf8.h"
#include "YQUI.h"
#include "YQMenuButton.h"
#include "YEvent.h"



YQMenuButton::YQMenuButton( YWidget * 		parent,
			    const string &	label )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YMenuButton( parent, label )
    , _selectedItem( 0 )
{
    setWidgetRep( this );
    _qt_button = new QPushButton( fromUTF8( label ), this );
    // _qt_button->setMinimumSize( 2,2 );
    _qt_button->move( YQButtonBorder, YQButtonBorder );
    setMinimumSize( _qt_button->minimumSize()
		    + 2 * QSize( YQButtonBorder, YQButtonBorder ) );
}


YQMenuButton::~YQMenuButton()
{
    // NOP
}


void
YQMenuButton::setLabel( const string & label )
{
    _qt_button->setText( fromUTF8( label ) );
    YMenuButton::setLabel( label );
}


void
YQMenuButton::rebuildMenuTree()
{
    //
    // Delete any previous menu
    // (in case the menu items got replaced)
    //

    if ( _qt_button->menu() )
	delete _qt_button->menu();

    //
    // Create toplevel menu
    //

    QMenu * menu = new QMenu( _qt_button );
    YUI_CHECK_NEW( menu );
    _qt_button->setMenu( menu );
    menu->setProperty( "class", "ymenubutton QMenu" );

    connect( menu,	SIGNAL( triggered         ( QAction* ) ),
	     this,	SLOT  ( menuEntryActivated( QAction* ) ) );

    //
    // Recursively add Qt menu items from the YMenuItems
    //

    rebuildMenuTree( menu, itemsBegin(), itemsEnd() );
}


void
YQMenuButton::rebuildMenuTree( QMenu * parentMenu, YItemIterator begin, YItemIterator end )
{
    for ( YItemIterator it = begin; it != end; ++it )
    {
	YMenuItem * item = dynamic_cast<YMenuItem *> (*it);
	YUI_CHECK_PTR( item );

	QPixmap icon;

	if ( item->hasIconName() )
	{
	    string iconName = iconFullPath( item );
	    icon = QPixmap( iconName.c_str() );

	    if ( icon.isNull() )
		yuiWarning() << "Can't load icon " << iconName << endl;
	}

	if ( item->hasChildren() )
	{
	    QMenu * subMenu;

	    if ( icon.isNull() )
		subMenu = parentMenu->addMenu( fromUTF8( item->label() ));
	    else
		subMenu = parentMenu->addMenu( QIcon( icon ), fromUTF8( item->label() ));

	    connect( subMenu,	SIGNAL( triggered         ( QAction* ) ),
		     this,	SLOT  ( menuEntryActivated( QAction* ) ) );

	    rebuildMenuTree( subMenu, item->childrenBegin(), item->childrenEnd() );
	}
	else // No children - leaf entry
	{
	    // item->index() is guaranteed to be unique within this YMenuButton's items,
	    // so it can easily be used as unique ID in all Q3PopupMenus that belong
	    // to this YQMenuButton.

            QAction *act;
	    if ( icon.isNull() )
		act = parentMenu->addAction( fromUTF8( item->label() ) );
	    else
		act = parentMenu->addAction( QIcon( icon ), fromUTF8( item->label() ) );
            _serials[act] = item->index();
	}
    }
}


void
YQMenuButton::menuEntryActivated( QAction* action )
{
    int serialNo = -1;
    if ( _serials.contains( action ) )
        serialNo = _serials[action];

    // yuiDebug() << "Selected menu entry #" << menu_item_index << endl;
    _selectedItem = findMenuItem( serialNo );

    if ( _selectedItem )
    {
	/*
	 * Defer the real returnNow() until all popup related events have been
	 * processed. This took me some hours to figure out; obviously
	 * exit_loop() doesn't have any effect as long as there are still
	 * popups open. So be it - use a zero timer to perform the real
	 * returnNow() later.
	 */

	/*
	 * the 100 delay is a ugly dirty workaround
	 */
	QTimer::singleShot( 100, this, SLOT( returnNow() ) );
    }
    else
    {
	yuiError() << "No menu item with serial no. " << serialNo << endl;
    }
}


void
YQMenuButton::returnNow()
{
    if ( _selectedItem )
    {
	YQUI::ui()->sendEvent( new YMenuEvent( _selectedItem ) );
	_selectedItem = 0;
    }
}



void
YQMenuButton::setEnabled( bool enabled )
{
    _qt_button->setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQMenuButton::preferredWidth()
{
    return 2*YQButtonBorder + _qt_button->sizeHint().width();
}


int YQMenuButton::preferredHeight()
{
    return 2*YQButtonBorder + _qt_button->sizeHint().height();
}


void
YQMenuButton::setSize( int newWidth, int newHeight )
{
    _qt_button->resize( newWidth  - 2 * YQButtonBorder,
			    newHeight - 2 * YQButtonBorder );
    resize( newWidth, newHeight );
}


bool
YQMenuButton::setKeyboardFocus()
{
    _qt_button->setFocus();

    return true;
}


#include "YQMenuButton.moc"
