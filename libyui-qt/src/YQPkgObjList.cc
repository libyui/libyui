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
				bool			selectNextItem )
{
    QListViewItem * listViewItem = selectedItem();

    if ( ! listViewItem )
	return;

    YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (listViewItem);

    if ( item )
    {
	if ( newStatus != item->status() )
	{
	    item->setStatus( newStatus );
	    emit statusChanged( item->pmObj() );
	}

	if ( selectNextItem && item->nextSibling() )
	{
	    item->setSelected( false );			// doesn't emit signals
	    setSelected( item->nextSibling(), true );	// emits signals
	}
    }
}


void
YQPkgObjList::createActions()
{
    _actionSetCurrentInstall	   = createAction( YQIconPool::pkgInstall(),	   _( "&Install"	       ) );
    _actionSetCurrentDontInstall   = createAction( YQIconPool::pkgNoInst(),	   _( "Do&n't install"	       ) );
    _actionSetCurrentKeepInstalled = createAction( YQIconPool::pkgKeepInstalled(), _( "&Keep"		       ) );
    _actionSetCurrentDelete	   = createAction( YQIconPool::pkgDel(),	   _( "&Delete"		       ) );
    _actionSetCurrentUpdate	   = createAction( YQIconPool::pkgUpdate(),	   _( "&Update"		       ) );
    _actionSetCurrentTaboo	   = createAction( YQIconPool::pkgTaboo(),	   _( "&Taboo - never install" ) );

    _actionSetCurrentAutoInstall   = createAction( YQIconPool::pkgAutoInstall(),   _( "Auto-install" ), false );
    _actionSetCurrentAutoUpdate	   = createAction( YQIconPool::pkgAutoUpdate(),	   _( "Auto-update"  ), false );
    _actionSetCurrentAutoDelete	   = createAction( YQIconPool::pkgAutoDel(),	   _( "Auto-delete"  ), false );

    connect( _actionSetCurrentInstall,	     SIGNAL( activated() ), this, SLOT( setCurrentInstall()	  ) );
    connect( _actionSetCurrentDontInstall,   SIGNAL( activated() ), this, SLOT( setCurrentDontInstall()	  ) );
    connect( _actionSetCurrentKeepInstalled, SIGNAL( activated() ), this, SLOT( setCurrentKeepInstalled() ) );
    connect( _actionSetCurrentDelete,	     SIGNAL( activated() ), this, SLOT( setCurrentDelete()	  ) );
    connect( _actionSetCurrentUpdate,	     SIGNAL( activated() ), this, SLOT( setCurrentUpdate()	  ) );
    connect( _actionSetCurrentTaboo,	     SIGNAL( activated() ), this, SLOT( setCurrentTaboo()	  ) );
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
    _actionSetCurrentAutoInstall->addTo( _notInstalledContextMenu );
}


void
YQPkgObjList::createInstalledContextMenu()
{
    _installedContextMenu = new QPopupMenu( this );
    CHECK_PTR( _installedContextMenu );

    _actionSetCurrentKeepInstalled->addTo( _installedContextMenu );
    _actionSetCurrentDelete->addTo( _installedContextMenu );
    _actionSetCurrentUpdate->addTo( _installedContextMenu );
    _actionSetCurrentAutoUpdate->addTo( _installedContextMenu );
    _actionSetCurrentAutoDelete->addTo( _installedContextMenu );
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





YQPkgObjListItem::YQPkgObjListItem( YQPkgObjList * pkgObjList, PMObjectPtr pm_obj )
    : QY2ListViewItem( pkgObjList )
    , _pkgObjList( pkgObjList )
    , _pmObj( pm_obj )
    , _editable( true )
{
    if ( nameCol()    >= 0 )	setText( nameCol(),	pmObj()->name()		);
    if ( summaryCol() >= 0 )	setText( summaryCol(),	pmObj()->summary()	);
    if ( sizeCol()    >= 0 )	setText( sizeCol(),	pmObj()->size().form() + "  " );

    if ( instVersionCol() >= 0 )
    {
	if ( pmObj()->hasInstalledObj() )
	     setText( instVersionCol(), pmObj()->getInstalledObj()->edition() );

	if ( pmObj()->hasCandidateObj() )
	    setText( versionCol(), pmObj()->getCandidateObj()->edition() );
    }
    else
    {
	setText( versionCol(),	pmObj()->edition() );
    }

    setStatusIcon();
}



YQPkgObjListItem::~YQPkgObjListItem()
{
    // NOP
}


void
YQPkgObjListItem::setText( int column, const std::string text )
{
    QListViewItem::setText( column, fromUTF8( text.c_str() ) );
}


void
YQPkgObjListItem::setText( int column, const PkgEdition & edition )
{
    QListViewItem::setText( column,
			    fromUTF8( edition.version() )
			    + "-" +
			    fromUTF8( edition.release() )
			    );
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

    switch ( status() )
    {
	case PMSelectable::S_Taboo:		icon = YQIconPool::pkgTaboo();		break;
	case PMSelectable::S_Del:		icon = YQIconPool::pkgDel();		break;
	case PMSelectable::S_Update:		icon = YQIconPool::pkgUpdate();		break;
	case PMSelectable::S_Install:		icon = YQIconPool::pkgInstall();	break;
	case PMSelectable::S_AutoDel:		icon = YQIconPool::pkgAutoDel();	break;
	case PMSelectable::S_AutoInstall:	icon = YQIconPool::pkgAutoInstall();	break;
	case PMSelectable::S_AutoUpdate:	icon = YQIconPool::pkgAutoUpdate();	break;
	case PMSelectable::S_KeepInstalled:	icon = YQIconPool::pkgKeepInstalled();	break;
	case PMSelectable::S_NoInst:		icon = YQIconPool::pkgNoInst();		break;

	    // Intentionally omitting 'default' branch so the compiler can
	    // catch unhandled enum states
    }

    setPixmap( statusCol(), icon );
}


void
YQPkgObjListItem::cycleStatus()
{
    PMSelectable::UI_Status oldStatus = status();
    PMSelectable::UI_Status newStatus = oldStatus;

    if ( pmObj()->hasInstalledObj() )
    {
	switch ( oldStatus )
	{
	    case PMSelectable::S_KeepInstalled:	newStatus = pmObj()->hasCandidateObj() ?
						    PMSelectable::S_Update : PMSelectable::S_Del;
						break;
	    case PMSelectable::S_Update:	newStatus = PMSelectable::S_Del;		break;
	    case PMSelectable::S_Del:		newStatus = PMSelectable::S_KeepInstalled;	break;
#warning TODO: Show DELNOTIFY
	    default:				newStatus = PMSelectable::S_KeepInstalled;	break;
	}
    }
    else	// pkg not installed
    {
	switch ( oldStatus )
	{
	    case PMSelectable::S_NoInst:	newStatus = pmObj()->hasCandidateObj() ?
						    PMSelectable::S_Install : PMSelectable::S_NoInst;
#warning TODO: Show INSNOTIFY
						break;
	    default:				newStatus = PMSelectable::S_NoInst;	break;

		// Intentionally NOT cycling through YQPkgTaboo:
		// This status is not common enough for that.
	}
    }

    if ( oldStatus != newStatus )
    {
	setStatus( newStatus );
	_pkgObjList->sendStatusChanged( pmObj() );
    }
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
