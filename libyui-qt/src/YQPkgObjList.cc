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

  File:	      YQPkgObjList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <qpixmap.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include <qaction.h>

#include <Y2PM.h>
#include <y2pm/InstTarget.h>
#include "utf8.h"

#include "YQPkgObjList.h"
#include "YQPkgTextDialog.h"
#include "YQi18n.h"
#include "YQIconPool.h"


YQPkgObjList::YQPkgObjList( QWidget *parent )
    : QY2ListView( parent )
    , _editable( true )
    , _installedContextMenu( 0 )
    , _notInstalledContextMenu( 0 )
{
    // This class does not add any columns. This is the main reason why this is
    // an abstract base class: It doesn't know which columns are desired and in
    // what order.

    _statusCol		= -42;
    _nameCol		= -42;
    _versionCol		= -42;
    _instVersionCol	= -42;
    _summaryCol		= -42;
    _sizeCol		= -42;

    createActions();

    connect( this,	SIGNAL( columnClicked		( int, QListViewItem *, int, const QPoint & ) ),
	     this,	SLOT  ( pkgObjClicked		( int, QListViewItem *, int, const QPoint & ) ) );

    connect( this,	SIGNAL( columnDoubleClicked	( int, QListViewItem *, int, const QPoint & ) ),
	     this,	SLOT  ( pkgObjClicked		( int, QListViewItem *, int, const QPoint & ) ) );

    connect( this,	SIGNAL( selectionChanged	( QListViewItem * ) ),
	     this,	SLOT  ( selectionChangedInternal( QListViewItem * ) ) );
}


YQPkgObjList::~YQPkgObjList()
{
    // NOP
}


void
YQPkgObjList::addPkgObjItem( PMObjectPtr pmObj )
{
    if ( ! pmObj )
    {
	y2error( "Null PMObject!" );
	return;
    }

    new YQPkgObjListItem( this, pmObj );
}


void
YQPkgObjList::pkgObjClicked( int		button,
			     QListViewItem *	listViewItem,
			     int		col,
			     const QPoint &	pos )
{
    YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (listViewItem);

    if ( item )
    {
	if ( button == Qt::LeftButton )
	{
	    if ( col == statusCol() )
		// || col == nameCol() )
	    {
		if ( editable() && item->editable() )
		    item->cycleStatus();
	    }
	}
	else if ( button == Qt::RightButton )
	{
	    if ( editable() && item->editable() )
	    {
		updateActions( item );

		QPopupMenu * contextMenu =
		    item->pmObj()->hasInstalledObj() ?
		    installedContextMenu() : notInstalledContextMenu();

		if ( contextMenu )
		    contextMenu->popup( pos );
	    }
	}
    }
}


void
YQPkgObjList::selectionChangedInternal( QListViewItem * listViewItem )
{
    YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (listViewItem);

    emit selectionChanged( item ? item->pmObj() : PMObjectPtr() );
}


void
YQPkgObjList::clear()
{
    emit selectionChanged( PMObjectPtr() );
    QY2ListView::clear();
}


void
YQPkgObjList::setCurrentStatus( PMSelectable::UI_Status	newStatus,
				bool			doSelectNextItem )
{
    QListViewItem * listViewItem = selectedItem();

    if ( ! listViewItem )
	return;

    YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (listViewItem);

    if ( item && item->editable() && _editable )
    {
	if ( newStatus != item->status() )
	{
	    item->setStatus( newStatus );
	    item->showNotifyTexts( newStatus );
	    emit statusChanged();
	}
    }

    if ( doSelectNextItem )
	selectNextItem();
}


void
YQPkgObjList::setAllItemStatus( PMSelectable::UI_Status	newStatus )
{
    if ( ! _editable )
	return;

    QListViewItem * listViewItem = firstChild();

    while ( listViewItem )
    {
	YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (listViewItem);

	if ( item && item->editable() && newStatus != item->status() )
	{
	    item->setStatus( newStatus );
	}

	listViewItem = listViewItem->nextSibling();
    }

    emit statusChanged();
}


void
YQPkgObjList::selectNextItem()
{
    QListViewItem * item = selectedItem();

    if ( item && item->nextSibling() )
    {
	item->setSelected( false );			// Doesn't emit signals
	ensureItemVisible( item->nextSibling() );	// Scroll if necessary
	setSelected( item->nextSibling(), true );	// Emits signals
    }
}


void
YQPkgObjList::createActions()
{
    _actionSetCurrentInstall	   = createAction( YQIconPool::pkgInstall(),	   _( "&Install"	       ) + " [+]" );
    _actionSetCurrentDontInstall   = createAction( YQIconPool::pkgNoInst(),	   _( "Do&n't install"	       ) + " [-]" );
    _actionSetCurrentKeepInstalled = createAction( YQIconPool::pkgKeepInstalled(), _( "&Keep"		       ) + " [<], [-]");
    _actionSetCurrentDelete	   = createAction( YQIconPool::pkgDel(),	   _( "&Delete"		       ) + " [-]" );
    _actionSetCurrentUpdate	   = createAction( YQIconPool::pkgUpdate(),	   _( "&Update"		       ) + " [>], [+]");
    _actionSetCurrentTaboo	   = createAction( YQIconPool::pkgTaboo(),	   _( "&Taboo - never install" ) + " [!]");

    _actionSetCurrentAutoInstall   = createAction( YQIconPool::pkgAutoInstall(),   _( "(Auto-install)" ) );
    _actionSetCurrentAutoUpdate	   = createAction( YQIconPool::pkgAutoUpdate(),	   _( "(Auto-update)"  ) );
    _actionSetCurrentAutoDelete	   = createAction( YQIconPool::pkgAutoDel(),	   _( "(Auto-delete)"  ) );


    _actionSetListInstall          = createAction( YQIconPool::pkgInstall(),       _( "&Install"               ) + " [Ctrl][+]" );
    _actionSetListDontInstall      = createAction( YQIconPool::pkgNoInst(),        _( "Do&n't install"         ) + " [Ctrl][-]" );
    _actionSetListKeepInstalled    = createAction( YQIconPool::pkgKeepInstalled(), _( "&Keep"                  ) + " [Ctrl][<], [Ctrl][-]");
    _actionSetListDelete           = createAction( YQIconPool::pkgDel(),           _( "&Delete"                ) + " [Ctrl][-]" );
    _actionSetListUpdate           = createAction( YQIconPool::pkgUpdate(),        _( "&Update"                ) + " [Ctrl][>]");
    _actionSetListTaboo            = createAction( YQIconPool::pkgTaboo(),         _( "&Taboo - never install" ) + " [Ctrl][!]");


    connect( _actionSetCurrentInstall,	     SIGNAL( activated() ), this, SLOT( setCurrentInstall()	  ) );
    connect( _actionSetCurrentDontInstall,   SIGNAL( activated() ), this, SLOT( setCurrentDontInstall()	  ) );
    connect( _actionSetCurrentKeepInstalled, SIGNAL( activated() ), this, SLOT( setCurrentKeepInstalled() ) );
    connect( _actionSetCurrentDelete,	     SIGNAL( activated() ), this, SLOT( setCurrentDelete()	  ) );
    connect( _actionSetCurrentUpdate,	     SIGNAL( activated() ), this, SLOT( setCurrentUpdate()	  ) );
    connect( _actionSetCurrentTaboo,	     SIGNAL( activated() ), this, SLOT( setCurrentTaboo()	  ) );

    connect( _actionSetListInstall,	     SIGNAL( activated() ), this, SLOT( setListInstall()	  ) );
    connect( _actionSetListDontInstall,      SIGNAL( activated() ), this, SLOT( setListDontInstall()	  ) );
    connect( _actionSetListKeepInstalled,    SIGNAL( activated() ), this, SLOT( setListKeepInstalled() 	  ) );
    connect( _actionSetListDelete,	     SIGNAL( activated() ), this, SLOT( setListDelete()	          ) );
    connect( _actionSetListUpdate,	     SIGNAL( activated() ), this, SLOT( setListUpdate()	          ) );
    connect( _actionSetListTaboo,	     SIGNAL( activated() ), this, SLOT( setListTaboo()	          ) );
}


QAction *
YQPkgObjList::createAction( const QPixmap & icon, const QString & text, bool enabled )
{
    QAction * action = new QAction( text,		// text
				    QIconSet( icon ),	// icon set
				    text,		// menu text
				    0,			// accel key
				    this );		// parent
    CHECK_PTR( action );
    action->setEnabled( enabled );

    return action;
}


void
YQPkgObjList::createNotInstalledContextMenu()
{
    _notInstalledContextMenu = new QPopupMenu( this );
    CHECK_PTR( _notInstalledContextMenu );

    _actionSetCurrentInstall->addTo( _notInstalledContextMenu );
    _actionSetCurrentDontInstall->addTo( _notInstalledContextMenu );
    _actionSetCurrentTaboo->addTo( _notInstalledContextMenu );

    _notInstalledContextMenu->insertSeparator();
    _actionSetCurrentAutoInstall->addTo( _notInstalledContextMenu );

    addAllInListSubMenu( _notInstalledContextMenu );
}


void
YQPkgObjList::createInstalledContextMenu()
{
    _installedContextMenu = new QPopupMenu( this );
    CHECK_PTR( _installedContextMenu );

    _actionSetCurrentKeepInstalled->addTo( _installedContextMenu );
    _actionSetCurrentDelete->addTo( _installedContextMenu );
    _actionSetCurrentUpdate->addTo( _installedContextMenu );

    _installedContextMenu->insertSeparator();
    _actionSetCurrentAutoUpdate->addTo( _installedContextMenu );
    _actionSetCurrentAutoDelete->addTo( _installedContextMenu );

    addAllInListSubMenu( _installedContextMenu );
}


void
YQPkgObjList::addAllInListSubMenu( QPopupMenu * menu )
{
    QPopupMenu * submenu = new QPopupMenu( menu );

    _actionSetListInstall->addTo( submenu );
    _actionSetListDontInstall->addTo( submenu );
    _actionSetListKeepInstalled->addTo( submenu );
    _actionSetListDelete->addTo( submenu );
    _actionSetListUpdate->addTo( submenu );
    _actionSetListTaboo->addTo( submenu );
    
    menu->insertItem( _( "&All in this list" ), submenu );
}


QPopupMenu *
YQPkgObjList::notInstalledContextMenu()
{
    if ( ! _notInstalledContextMenu )
	createNotInstalledContextMenu();

    return _notInstalledContextMenu;
}


QPopupMenu *
YQPkgObjList::installedContextMenu()
{
    if ( ! _installedContextMenu )
	createInstalledContextMenu();

    return _installedContextMenu;
}


void
YQPkgObjList::updateActions( YQPkgObjListItem * item )
{
    if ( !item ) return;

    PMObjectPtr pmObj = item->pmObj();

    if ( pmObj->hasInstalledObj() )
    {
	_actionSetCurrentInstall->setEnabled( false );
	_actionSetCurrentDontInstall->setEnabled( false );
	_actionSetCurrentTaboo->setEnabled( false);

	_actionSetCurrentKeepInstalled->setEnabled( true );
	_actionSetCurrentDelete->setEnabled( true );
	_actionSetCurrentUpdate->setEnabled( pmObj->hasCandidateObj() );
    }
    else
    {
	_actionSetCurrentInstall->setEnabled( pmObj->hasCandidateObj() );
	_actionSetCurrentDontInstall->setEnabled( true );
	_actionSetCurrentTaboo->setEnabled( true );

	_actionSetCurrentKeepInstalled->setEnabled( false);
	_actionSetCurrentDelete->setEnabled( false );
	_actionSetCurrentUpdate->setEnabled( false );
    }
}


void
YQPkgObjList::keyPressEvent( QKeyEvent *event )
{
    if ( event )
    {
	QListViewItem * selectedListViewItem = selectedItem();

	if ( selectedListViewItem )
	{
	    YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (selectedListViewItem);

	    if ( item )
	    {
		bool installed = item->pmObj()->hasInstalledObj();
		PMSelectable::UI_Status status = item->status();

		switch( event->ascii() )
		{
		    case Qt::Key_Space:		// Cycle
			item->cycleStatus();
			event->accept();
			return;

		    case '+':	// Grab everything - install or update

			if ( installed )
			{
			    PMSelectable::UI_Status newStatus = PMSelectable::S_KeepInstalled;

			    if ( item->candidateIsNewer() )
				newStatus = PMSelectable::S_Update;

			    setCurrentStatus( newStatus );
			}
			else
			    setCurrentStatus( PMSelectable::S_Install );
			selectNextItem();
			event->accept();
			return;

		    case '-':	// Get rid of everything - don't install or delete
			setCurrentStatus( installed ? PMSelectable::S_Del : PMSelectable::S_NoInst );
			selectNextItem();
			event->accept();
			return;

		    case '!':	// Taboo

			if ( ! installed )
			    setCurrentStatus( PMSelectable::S_Taboo );
			selectNextItem();
			event->accept();
			return;

		    case '>':	// Update what is worth to be updated

			if ( installed && item->candidateIsNewer() )
			    setCurrentStatus( PMSelectable::S_Update );
			selectNextItem();
			event->accept();
			return;

		    case '<':	// Revert update

			if ( status == PMSelectable::S_Update ||
			     status == PMSelectable::S_AutoUpdate )
			{
			    setCurrentStatus( PMSelectable::S_KeepInstalled );
			}
			selectNextItem();
			event->accept();
			return;
		}
	    }
	}
    }

    QListView::keyPressEvent( event );
}


void
YQPkgObjList::message( const QString & text )
{
    QY2ListViewItem * item = new QY2ListViewItem( this );
    CHECK_PTR( item );

    item->setText( 0, text );
    item->setBackgroundColor( QColor( 0xE0, 0xE0, 0xF8 ) );
}




YQPkgObjListItem::YQPkgObjListItem( YQPkgObjList * pkgObjList, PMObjectPtr pm_obj )
    : QY2ListViewItem( pkgObjList )
    , _pkgObjList( pkgObjList )
    , _pmObj( pm_obj )
    , _editable( true )
{
    init();
}


YQPkgObjListItem::~YQPkgObjListItem()
{
    // NOP
}


void
YQPkgObjListItem::init()
{
    _candidateIsNewer = false;
    _installedIsNewer = false;

    PMObjectPtr candidate = _pmObj->getCandidateObj();
    PMObjectPtr installed = _pmObj->getInstalledObj();

    if ( candidate && installed )
    {
	_candidateIsNewer = candidate->edition() > installed->edition();
	_installedIsNewer = candidate->edition() < installed->edition();
	// Cache this information, it's expensive to obtain!
    }


    if ( nameCol()    >= 0 )	setText( nameCol(),	pmObj()->name()		);
    if ( summaryCol() >= 0 )	setText( summaryCol(),	pmObj()->summary()	);
    if ( sizeCol()    >= 0 )	setText( sizeCol(),	pmObj()->size().form() + "  " );

    if ( instVersionCol() >= 0 )
    {
	if ( pmObj()->hasInstalledObj() )
	     setText( instVersionCol(), pmObj()->getInstalledObj()->edition() );

	if ( ! pmObj()->isInstalledObj() &&
	     ! pmObj()->isCandidateObj()   )
	{
	    setText( versionCol(), pmObj()->edition() );
	}
	else if ( pmObj()->hasCandidateObj() )
	{
	    setText( versionCol(), pmObj()->getCandidateObj()->edition() );
	}
    }
    else
    {
	setText( versionCol(),	pmObj()->edition() );
    }

    setStatusIcon();
}


void
YQPkgObjListItem::updateData()
{
    init();
}


void
YQPkgObjListItem::setText( int column, const std::string text )
{
    QListViewItem::setText( column, fromUTF8( text.c_str() ) );
}


void
YQPkgObjListItem::setText( int column, const PkgEdition & edition )
{
    setText( column, edition.asString() );
}


PMSelectable::UI_Status
YQPkgObjListItem::status() const
{
    PMSelectablePtr selectable = constPMObj()->getSelectable();

    if ( ! selectable )
	return PMSelectable::S_NoInst;

    return selectable->status();
}


void
YQPkgObjListItem::setStatus( PMSelectable::UI_Status newStatus )
{
    pmObj()->getSelectable()->set_status( newStatus );
    setStatusIcon();
}


void
YQPkgObjListItem::updateStatus()
{
    setStatusIcon();
}


void
YQPkgObjListItem::setStatusIcon()
{
    if ( statusCol() < 0 )
	return;

    QPixmap icon = YQIconPool::pkgNoInst();

    if ( editable() && _pkgObjList->editable() )
    {
	switch ( status() )
	{
	    case PMSelectable::S_Taboo:		icon = YQIconPool::pkgTaboo();		break;
	    case PMSelectable::S_Del:		icon = YQIconPool::pkgDel();		break;
	    case PMSelectable::S_Update:	icon = YQIconPool::pkgUpdate();		break;
	    case PMSelectable::S_Install:	icon = YQIconPool::pkgInstall();	break;
	    case PMSelectable::S_AutoDel:	icon = YQIconPool::pkgAutoDel();	break;
	    case PMSelectable::S_AutoInstall:	icon = YQIconPool::pkgAutoInstall();	break;
	    case PMSelectable::S_AutoUpdate:	icon = YQIconPool::pkgAutoUpdate();	break;
	    case PMSelectable::S_KeepInstalled:	icon = YQIconPool::pkgKeepInstalled();	break;
	    case PMSelectable::S_NoInst:	icon = YQIconPool::pkgNoInst();		break;

		// Intentionally omitting 'default' branch so the compiler can
		// catch unhandled enum states
	}
    }
    else
    {
	switch ( status() )
	{
	    case PMSelectable::S_Taboo:		icon = YQIconPool::disabledPkgTaboo();		break;
	    case PMSelectable::S_Del:		icon = YQIconPool::disabledPkgDel();		break;
	    case PMSelectable::S_Update:	icon = YQIconPool::disabledPkgUpdate();		break;
	    case PMSelectable::S_Install:	icon = YQIconPool::disabledPkgInstall();	break;
	    case PMSelectable::S_AutoDel:	icon = YQIconPool::disabledPkgAutoDel();	break;
	    case PMSelectable::S_AutoInstall:	icon = YQIconPool::disabledPkgAutoInstall();	break;
	    case PMSelectable::S_AutoUpdate:	icon = YQIconPool::disabledPkgAutoUpdate();	break;
	    case PMSelectable::S_KeepInstalled:	icon = YQIconPool::disabledPkgKeepInstalled();	break;
	    case PMSelectable::S_NoInst:	icon = YQIconPool::disabledPkgNoInst();		break;

		// Intentionally omitting 'default' branch so the compiler can
		// catch unhandled enum states
	}
    }

    setPixmap( statusCol(), icon );
}


void
YQPkgObjListItem::cycleStatus()
{
    if ( ! _editable || ! _pkgObjList->editable() )
	return;

    PMSelectable::UI_Status oldStatus = status();
    PMSelectable::UI_Status newStatus = oldStatus;
    bool showInsNotify = false;
    bool showDelNotify = false;

    if ( pmObj()->hasInstalledObj() )
    {
	switch ( oldStatus )
	{
	    case PMSelectable::S_KeepInstalled:
		newStatus = pmObj()->hasCandidateObj() ?
		    PMSelectable::S_Update : PMSelectable::S_Del;
		break;

	    case PMSelectable::S_Update:
		newStatus = PMSelectable::S_Del;
		showDelNotify = true;
		break;

	    case PMSelectable::S_Del:
		newStatus = PMSelectable::S_KeepInstalled;
		break;

	    default:
		newStatus = PMSelectable::S_KeepInstalled;
		break;
	}
    }
    else	// Pkg not installed
    {
	switch ( oldStatus )
	{
	    case PMSelectable::S_NoInst:
		if ( pmObj()->hasCandidateObj() )
		{
		    newStatus = PMSelectable::S_Install;
		    showInsNotify = true;
		}
		else
		{
		    newStatus = PMSelectable::S_NoInst;
		}
		break;

	    case PMSelectable::S_AutoInstall:
		newStatus =  PMSelectable::S_Taboo;
		break;

	    default:
		newStatus = PMSelectable::S_NoInst;
		break;
	}
    }

    if ( oldStatus != newStatus )
    {
	setStatus( newStatus );
	showNotifyTexts( newStatus );
	_pkgObjList->sendStatusChanged();
    }
}


void
YQPkgObjListItem::showNotifyTexts( PMSelectable::UI_Status status )
{
    std::list<std::string> text;

    switch ( status )
    {
	case PMSelectable::S_Install:
	    if ( _pmObj->hasCandidateObj() )
		text = _pmObj->getCandidateObj()->insnotify();
	    break;

	case PMSelectable::S_NoInst:
	case PMSelectable::S_Del:
	case PMSelectable::S_Taboo:
	    if ( _pmObj->hasCandidateObj() )
		text = _pmObj->getCandidateObj()->delnotify();
	    break;

	default: break;
    }

    if ( ! text.empty() )
    {
	y2debug( "Showing notify text" );
	YQPkgTextDialog::showText( _pkgObjList, _pmObj, text );
    }
}


QString
YQPkgObjListItem::toolTip( int col )
{
    QString text;
    
    if ( col == statusCol() )
    {
	switch ( status() )
	{
	    case PMSelectable::S_Taboo:		return _( "Taboo - never install" 	);
	    case PMSelectable::S_Del:		return _( "Delete" 			);
	    case PMSelectable::S_Update:	return _( "Update" 			);
	    case PMSelectable::S_Install:	return _( "Install" 			);
	    case PMSelectable::S_AutoDel:	return _( "Auto-delete" 		);
	    case PMSelectable::S_AutoInstall:	return _( "Auto-install" 		);
	    case PMSelectable::S_AutoUpdate:	return _( "Auto-update" 		);
	    case PMSelectable::S_KeepInstalled:	return _( "Keep" 			);
	    case PMSelectable::S_NoInst:	return _( "Don't install" 		);
	}
    }

    return QString();
}


/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
YQPkgObjListItem::compare( QListViewItem *	otherListViewItem,
			   int			col,
			   bool			ascending ) const
{
    YQPkgObjListItem * other = dynamic_cast<YQPkgObjListItem *> (otherListViewItem);

    if ( other )
    {
	if ( col == sizeCol() )
	{
	    // Numeric sort by size

	    if ( this->constPMObj()->size() < other->constPMObj()->size() ) return -1;
	    if ( this->constPMObj()->size() > other->constPMObj()->size() ) return 1;
	    return 0;
	}
	else if ( col == statusCol() )
	{
	    // Sorting by status depends on the numeric value of the
	    // PMSelectable::UI_Status enum, thus it is important to insert new
	    // package states there where they make most sense. We want to show
	    // dangerous or noteworthy states first - e.g., "taboo" which should
	    // seldeom occur, but when it does, it is important.

	    if ( this->status() < other->status() ) return -1;
	    if ( this->status() > other->status() ) return 1;
	    return 0;
	}
    }

    // Fallback: Use parent class method
    return QY2ListViewItem::compare( otherListViewItem, col, ascending );
}




#include "YQPkgObjList.moc.cc"
