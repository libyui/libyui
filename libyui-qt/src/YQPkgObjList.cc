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
#include <y2pm/PMPackage.h>
#include "utf8.h"

#include "YQPkgObjList.h"
#include "YQPkgTextDialog.h"
#include "YQi18n.h"
#include "YQIconPool.h"
#include "YQUI.h"

using std::list;
using std::string;


YQPkgObjList::YQPkgObjList( QWidget * parent )
    : QY2ListView( parent )
    , _editable( true )
    , _installedContextMenu(0)
    , _notInstalledContextMenu(0)
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
YQPkgObjList::addPassiveItem( const QString & 	name,
			      const QString & 	summary,
			      FSize		size )
{
    QY2ListViewItem * item = new QY2ListViewItem( this, QString::null, true );

    if ( item )
    {
	if ( nameCol()    >= 0 && ! name.isEmpty()    )	item->setText( nameCol(),	name    );
	if ( summaryCol() >= 0 && ! summary.isEmpty() )	item->setText( summaryCol(),	summary );
	if ( sizeCol()    >= 0 && size > 0L	      )
	{
	    QString sizeStr = size.form().c_str();
	    sizeStr += "  ";
	    item->setText( sizeCol(), sizeStr );
	}
    }
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


QPixmap
YQPkgObjList::statusIcon( PMSelectable::UI_Status status, bool enabled, bool bySelection )
{
    QPixmap icon = YQIconPool::pkgNoInst();

    if ( enabled )
    {
	switch ( status )
	{
	    case PMSelectable::S_Del:		icon = YQIconPool::pkgDel();		break;
	    case PMSelectable::S_Install:	icon = YQIconPool::pkgInstall();	break;
	    case PMSelectable::S_KeepInstalled: icon = YQIconPool::pkgKeepInstalled();	break;
	    case PMSelectable::S_NoInst:	icon = YQIconPool::pkgNoInst();		break;
	    case PMSelectable::S_Protected:	icon = YQIconPool::pkgProtected();	break;
	    case PMSelectable::S_Taboo:		icon = YQIconPool::pkgTaboo();		break;
	    case PMSelectable::S_Update:	icon = YQIconPool::pkgUpdate();		break;

	    case PMSelectable::S_AutoDel:	icon = bySelection ?
						    YQIconPool::pkgSelAutoDel() :
						    YQIconPool::pkgAutoDel();		break;

	    case PMSelectable::S_AutoInstall:	icon = bySelection ?
						    YQIconPool::pkgSelAutoInstall() :
						    YQIconPool::pkgAutoInstall();	break;

	    case PMSelectable::S_AutoUpdate:	icon = bySelection ?
						    YQIconPool::pkgSelAutoUpdate() :
						    YQIconPool::pkgAutoUpdate();	break;


		// Intentionally omitting 'default' branch so the compiler can
		// catch unhandled enum states
	}
    }
    else
    {
	switch ( status )
	{
	    case PMSelectable::S_Del:		icon = YQIconPool::disabledPkgDel();		break;
	    case PMSelectable::S_Install:	icon = YQIconPool::disabledPkgInstall();	break;
	    case PMSelectable::S_KeepInstalled: icon = YQIconPool::disabledPkgKeepInstalled();	break;
	    case PMSelectable::S_NoInst:	icon = YQIconPool::disabledPkgNoInst();		break;
	    case PMSelectable::S_Protected:	icon = YQIconPool::disabledPkgProtected();	break;
	    case PMSelectable::S_Taboo:		icon = YQIconPool::disabledPkgTaboo();		break;
	    case PMSelectable::S_Update:	icon = YQIconPool::disabledPkgUpdate();		break;

	    case PMSelectable::S_AutoDel:	icon = bySelection ?
						    YQIconPool::disabledPkgSelAutoDel() :
						    YQIconPool::disabledPkgAutoDel();		break;

	    case PMSelectable::S_AutoInstall:	icon = bySelection ?
						    YQIconPool::disabledPkgSelAutoInstall() :
						    YQIconPool::disabledPkgAutoInstall();	break;

	    case PMSelectable::S_AutoUpdate:	icon = bySelection ?
						    YQIconPool::disabledPkgSelAutoUpdate() :
						    YQIconPool::disabledPkgAutoUpdate();	break;

		// Intentionally omitting 'default' branch so the compiler can
		// catch unhandled enum states
	}
    }

    return icon;
}


QString
YQPkgObjList::statusText( PMSelectable::UI_Status status ) const
{
    switch ( status )
    {
	case PMSelectable::S_AutoDel:		return _( "Autodelete" );
	case PMSelectable::S_AutoInstall:	return _( "Autoinstall" );
	case PMSelectable::S_AutoUpdate:	return _( "Autoupdate" );
	case PMSelectable::S_Del:		return _( "Delete" );
	case PMSelectable::S_Install:		return _( "Install" );
	case PMSelectable::S_KeepInstalled:	return _( "Keep" );
	case PMSelectable::S_NoInst:		return _( "Do Not Install" );
	case PMSelectable::S_Protected:		return _( "Protected -- Do Not Modify" );
	case PMSelectable::S_Taboo:		return _( "Taboo -- Never Install" );
	case PMSelectable::S_Update:		return _( "Update" );
    }

    return QString::null;
}


void
YQPkgObjList::setCurrentStatus( PMSelectable::UI_Status newStatus,
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
	    if ( item->showLicenseAgreement( newStatus ) )
	    {
		item->showNotifyTexts( newStatus );
		item->setStatus( newStatus );
	    }

	    emit statusChanged();
	}
    }

    if ( doSelectNextItem )
	selectNextItem();
}


void
YQPkgObjList::setAllItemStatus( PMSelectable::UI_Status newStatus, bool force )
{
    if ( ! _editable )
	return;

    YQUI::ui()->busyCursor();
    QListViewItem * listViewItem = firstChild();

    while ( listViewItem )
    {
	YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (listViewItem);

	if ( item && item->editable() && newStatus != item->status() )
	{
	    if ( newStatus == PMSelectable::S_Update )
	    {
		if ( item->candidateIsNewer() || force )
		    item->setStatus( newStatus );
	    }
	    else
	    {
		item->setStatus( newStatus );
	    }
	}

	listViewItem = listViewItem->nextSibling();
    }

    YQUI::ui()->normalCursor();
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
    actionSetCurrentInstall		= createAction( PMSelectable::S_Install,	"[+]"		);
    actionSetCurrentDontInstall		= createAction( PMSelectable::S_NoInst,		"[-]"		);
    actionSetCurrentKeepInstalled	= createAction( PMSelectable::S_KeepInstalled,	"[<], [-]"	);
    actionSetCurrentDelete		= createAction( PMSelectable::S_Del,		"[-]"		);
    actionSetCurrentUpdate		= createAction( PMSelectable::S_Update,		"[>], [+]"	);
    actionSetCurrentTaboo		= createAction( PMSelectable::S_Taboo,		"[!]"		);
    actionSetCurrentProtected		= createAction( PMSelectable::S_Protected, 	"[*]" 		);

    actionSetListInstall		= createAction( PMSelectable::S_Install,	"", true );
    actionSetListDontInstall		= createAction( PMSelectable::S_NoInst,		"", true );
    actionSetListKeepInstalled		= createAction( PMSelectable::S_KeepInstalled,	"", true );
    actionSetListDelete			= createAction( PMSelectable::S_Del,		"", true );
    actionSetListProtected		= createAction( PMSelectable::S_Protected, 	"", true );

    actionSetListUpdate			= createAction( _( "Update if newer version available" ),
							statusIcon( PMSelectable::S_Update, true ),
							statusIcon( PMSelectable::S_Update, false ),
							"",
							true );

    actionSetListUpdateForce		= createAction( _( "Update unconditionally" ),
							statusIcon( PMSelectable::S_Update, true ),
							statusIcon( PMSelectable::S_Update, false ),
							"",
							true );

    actionSetListTaboo			= createAction( PMSelectable::S_Taboo,		"", true );

    connect( actionSetCurrentInstall,	     SIGNAL( activated() ), this, SLOT( setCurrentInstall()	  ) );
    connect( actionSetCurrentDontInstall,    SIGNAL( activated() ), this, SLOT( setCurrentDontInstall()	  ) );
    connect( actionSetCurrentKeepInstalled,  SIGNAL( activated() ), this, SLOT( setCurrentKeepInstalled() ) );
    connect( actionSetCurrentDelete,	     SIGNAL( activated() ), this, SLOT( setCurrentDelete()	  ) );
    connect( actionSetCurrentUpdate,	     SIGNAL( activated() ), this, SLOT( setCurrentUpdate()	  ) );
    connect( actionSetCurrentTaboo,	     SIGNAL( activated() ), this, SLOT( setCurrentTaboo()	  ) );
    connect( actionSetCurrentProtected,	     SIGNAL( activated() ), this, SLOT( setCurrentProtected()	  ) );

    connect( actionSetListInstall,	     SIGNAL( activated() ), this, SLOT( setListInstall()	  ) );
    connect( actionSetListDontInstall,	     SIGNAL( activated() ), this, SLOT( setListDontInstall()	  ) );
    connect( actionSetListKeepInstalled,     SIGNAL( activated() ), this, SLOT( setListKeepInstalled()	  ) );
    connect( actionSetListDelete,	     SIGNAL( activated() ), this, SLOT( setListDelete()		  ) );
    connect( actionSetListUpdate,	     SIGNAL( activated() ), this, SLOT( setListUpdate()		  ) );
    connect( actionSetListUpdateForce,	     SIGNAL( activated() ), this, SLOT( setListUpdateForce()	  ) );
    connect( actionSetListTaboo,	     SIGNAL( activated() ), this, SLOT( setListTaboo()		  ) );
    connect( actionSetListProtected,	     SIGNAL( activated() ), this, SLOT( setListProtected()	  ) );
}



QAction *
YQPkgObjList::createAction( PMSelectable::UI_Status status, const QString & key, bool enabled )
{
    return createAction( statusText( status ),
			 statusIcon( status, true ),
			 statusIcon( status, false ),
			 key,
			 enabled );
}


QAction *
YQPkgObjList::createAction( const QString &	text,
			    const QPixmap &	icon,
			    const QPixmap &	insensitiveIcon,
			    const QString &	key,
			    bool		enabled )
{
    QString label = text;

    if ( ! key.isEmpty() )
	label += "\t" + key;


    QIconSet iconSet ( icon );

    if ( ! insensitiveIcon.isNull() )
    {
	iconSet.setPixmap( insensitiveIcon,
			   QIconSet::Automatic,
			   QIconSet::Disabled );
    }

    QAction * action = new QAction( label,	// text
				    iconSet,	// icon set
				    label,	// menu text
				    0,		// accel key
				    this );	// parent
    CHECK_PTR( action );
    action->setEnabled( enabled );

    return action;
}


void
YQPkgObjList::createNotInstalledContextMenu()
{
    _notInstalledContextMenu = new QPopupMenu( this );
    CHECK_PTR( _notInstalledContextMenu );

    actionSetCurrentInstall->addTo( _notInstalledContextMenu );
    actionSetCurrentDontInstall->addTo( _notInstalledContextMenu );
    actionSetCurrentTaboo->addTo( _notInstalledContextMenu );

    addAllInListSubMenu( _notInstalledContextMenu );
}


void
YQPkgObjList::createInstalledContextMenu()
{
    _installedContextMenu = new QPopupMenu( this );
    CHECK_PTR( _installedContextMenu );

    actionSetCurrentKeepInstalled->addTo( _installedContextMenu );
    actionSetCurrentDelete->addTo( _installedContextMenu );
    actionSetCurrentUpdate->addTo( _installedContextMenu );

    addAllInListSubMenu( _installedContextMenu );
}


QPopupMenu *
YQPkgObjList::addAllInListSubMenu( QPopupMenu * menu )
{
    QPopupMenu * submenu = new QPopupMenu( menu );
    CHECK_PTR( submenu );

    actionSetListInstall->addTo( submenu );
    actionSetListDontInstall->addTo( submenu );
    actionSetListKeepInstalled->addTo( submenu );
    actionSetListDelete->addTo( submenu );
    actionSetListUpdate->addTo( submenu );
    actionSetListUpdateForce->addTo( submenu );
    actionSetListTaboo->addTo( submenu );

    menu->insertItem( _( "&All in This List" ), submenu );

    return submenu;
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
YQPkgObjList::updateActions()
{
    updateActions( dynamic_cast<YQPkgObjListItem *> ( selectedItem() ) );
}


void
YQPkgObjList::updateActions( YQPkgObjListItem * item )
{
    if ( item )
    {
	PMObjectPtr pmObj = item->pmObj();

	if ( pmObj->hasInstalledObj() )
	{
	    actionSetCurrentInstall->setEnabled( false );
	    actionSetCurrentDontInstall->setEnabled( false );
	    actionSetCurrentTaboo->setEnabled( false );
	    actionSetCurrentProtected->setEnabled( true );

	    actionSetCurrentKeepInstalled->setEnabled( true );
	    actionSetCurrentDelete->setEnabled( true );
	    actionSetCurrentUpdate->setEnabled( pmObj->hasCandidateObj() );
	}
	else
	{
	    actionSetCurrentInstall->setEnabled( pmObj->hasCandidateObj() );
	    actionSetCurrentDontInstall->setEnabled( true );
	    actionSetCurrentTaboo->setEnabled( true );
	    actionSetCurrentProtected->setEnabled( false );

	    actionSetCurrentKeepInstalled->setEnabled( false );
	    actionSetCurrentDelete->setEnabled( false );
	    actionSetCurrentUpdate->setEnabled( false );
	}
    }
    else	// ! item
    {
	actionSetCurrentInstall->setEnabled( false );
	actionSetCurrentDontInstall->setEnabled( false );
	actionSetCurrentTaboo->setEnabled( false );

	actionSetCurrentKeepInstalled->setEnabled( false );
	actionSetCurrentDelete->setEnabled( false );
	actionSetCurrentUpdate->setEnabled( false );
	actionSetCurrentProtected->setEnabled( false );
    }
}


void
YQPkgObjList::keyPressEvent( QKeyEvent * event )
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

		    case '*':	// Protected

			if ( installed )
			    setCurrentStatus( PMSelectable::S_Protected );
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

    item->setText( nameCol() >= 0 ? nameCol() : 0, text );
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

    if ( candidate && installed && candidate->edition() != installed->edition() )
    {
	if ( _pmObj->getSelectable()->downgrade_condition() )
	    _installedIsNewer = true;
	else
	    _candidateIsNewer = true;
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
YQPkgObjListItem::setText( int column, const string text )
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


bool
YQPkgObjListItem::bySelection() const
{
    bool bySel = false;

    if ( _pmObj->getSelectable() && _pmObj->getSelectable()->by_appl() )
	bySel = true;

    return bySel;
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
    if ( statusCol() >= 0 )
    {
	bool enabled = editable() && _pkgObjList->editable();
	setPixmap( statusCol(), _pkgObjList->statusIcon( status(), enabled, bySelection() ) );
    }
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
	    case PMSelectable::S_Protected:
		newStatus = pmObj()->hasCandidateObj() ?
		    PMSelectable::S_KeepInstalled: PMSelectable::S_NoInst;
		break;

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
	if ( showLicenseAgreement( newStatus ) )
	{
	    showNotifyTexts( newStatus );
	    setStatus( newStatus );
	}

	_pkgObjList->sendStatusChanged();
    }
}


void
YQPkgObjListItem::showNotifyTexts( PMSelectable::UI_Status status )
{
    list<string> text;

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


bool
YQPkgObjListItem::showLicenseAgreement( PMSelectable::UI_Status status )
{
    bool confirmed = true;
    list<string> text;
    PMPackagePtr pkg = NULL;

    switch ( status )
    {
	case PMSelectable::S_Install:
	case PMSelectable::S_Update:
	    if ( _pmObj->hasCandidateObj() )
	    {
		pkg =  _pmObj->getCandidateObj();

		if ( pkg )
		{
		    text = pkg->licenseToConfirm();
		    confirmed = ! pkg->hasLicenseToConfirm ();
		}
	    }
	    break;

	default: return true;
    }

    if ( ! text.empty() )
    {
	y2debug( "Showing license agreement" );
	confirmed = confirmed
	    || YQPkgTextDialog::confirmText( _pkgObjList, _pmObj, text );

	if ( ! confirmed )
	{
	    // The user rejected the license agreement -
	    // make sure the package gets unselected.

	    switch ( status )
	    {
		case PMSelectable::S_Install:
		    setStatus( PMSelectable::S_Taboo );
		    break;

		case PMSelectable::S_Protected:
		    setStatus( PMSelectable::S_Del );
		    break;

		default: break;
	    }
	}
	else
	{
	    if (pkg)
	    {
		pkg->markLicenseConfirmed ();
	    }
	}
    }

    return confirmed;
}


QString
YQPkgObjListItem::toolTip( int col )
{
    if ( col == statusCol() )
    {
	QString tip = _pkgObjList->statusText( status() );

	switch ( status() )
	{
	    case PMSelectable::S_AutoDel:
	    case PMSelectable::S_AutoInstall:
	    case PMSelectable::S_AutoUpdate:

		if ( bySelection() )
		    // Translators: Additional hint what caused an auto-status
		    tip += "\n" + _( "(by a software selection)" );
		else
		    tip += "\n" + _( "(by dependencies)" );

		break;

	    default:
		break;
	}

	return tip;
    }

    return QString::null;
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
	else if ( col == instVersionCol() ||
		  col == versionCol() )
	{
	    // Sorting by version numbers doesn't make too much sense, so let's
	    // sort by package relation:
	    // - Installed newer than candidate ( red )
	    // - Candidate newer than installed ( blue ) - worthwhile updating
	    // - Installed
	    // - Not installed, but candidate available
	    //
	    // Within these categories, sort versions by ASCII - OK, it's
	    // pretty random, but predictable.

	    int thisPoints  = this->versionPoints();
	    int otherPoints = other->versionPoints();

	    if ( thisPoints > otherPoints ) return -1;
	    if ( thisPoints < otherPoints ) return  1;
	    return QY2ListViewItem::compare( otherListViewItem, col, ascending );
	}
    }

    // Fallback: Use parent class method
    return QY2ListViewItem::compare( otherListViewItem, col, ascending );
}


int
YQPkgObjListItem::versionPoints() const
{
    int points = 0;

    if ( installedIsNewer() )		points += 1000;
    if ( candidateIsNewer() )		points += 100;
    if ( _pmObj->hasInstalledObj() )	points += 10;
    if ( _pmObj->hasCandidateObj() )	points += 1;

    return points;
}



#include "YQPkgObjList.moc"
