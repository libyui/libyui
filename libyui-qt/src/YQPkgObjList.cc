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

#include "YQZypp.h"

#ifdef MISSING
#include <y2pm/InstTarget.h>
#endif

#include <zypp/Package.h>
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
YQPkgObjList::addPkgObjItem( zypp::ResObject::Ptr zyppObj )
{
    if ( ! zyppObj )
    {
	y2error( "Null zypp::ResObject!" );
	return;
    }

    new YQPkgObjListItem( this, zyppObj );
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
		    item->zyppObj()->hasInstalledObj() ?
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

    emit selectionChanged( item ? item->zyppObj() : zypp::ResObject::Ptr() );
}


void
YQPkgObjList::clear()
{
    emit selectionChanged( zypp::ResObject::Ptr() );
    QY2ListView::clear();
}


QPixmap
YQPkgObjList::statusIcon( zypp::ui::Status status, bool enabled, bool bySelection )
{
    QPixmap icon = YQIconPool::pkgNoInst();

    if ( enabled )
    {
	switch ( status )
	{
	    case Status::S_Del:			icon = YQIconPool::pkgDel();		break;
	    case Status::S_Install:		icon = YQIconPool::pkgInstall();	break;
	    case Status::S_KeepInstalled:	icon = YQIconPool::pkgKeepInstalled();	break;
	    case Status::S_NoInst:		icon = YQIconPool::pkgNoInst();		break;
	    case Status::S_Protected:		icon = YQIconPool::pkgProtected();	break;
	    case Status::S_Taboo:		icon = YQIconPool::pkgTaboo();		break;
	    case Status::S_Update:		icon = YQIconPool::pkgUpdate();		break;

	    case Status::S_AutoDel:		icon = bySelection ?
						    YQIconPool::pkgSelAutoDel() :
						    YQIconPool::pkgAutoDel();		break;

	    case Status::S_AutoInstall:		icon = bySelection ?
						    YQIconPool::pkgSelAutoInstall() :
						    YQIconPool::pkgAutoInstall();	break;

	    case Status::S_AutoUpdate:		icon = bySelection ?
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
	    case Status::S_Del:			icon = YQIconPool::disabledPkgDel();		break;
	    case Status::S_Install:		icon = YQIconPool::disabledPkgInstall();	break;
	    case Status::S_KeepInstalled:	icon = YQIconPool::disabledPkgKeepInstalled();	break;
	    case Status::S_NoInst:		icon = YQIconPool::disabledPkgNoInst();		break;
	    case Status::S_Protected:		icon = YQIconPool::disabledPkgProtected();	break;
	    case Status::S_Taboo:		icon = YQIconPool::disabledPkgTaboo();		break;
	    case Status::S_Update:		icon = YQIconPool::disabledPkgUpdate();		break;

	    case Status::S_AutoDel:		icon = bySelection ?
						    YQIconPool::disabledPkgSelAutoDel() :
						    YQIconPool::disabledPkgAutoDel();		break;

	    case Status::S_AutoInstall:		icon = bySelection ?
						    YQIconPool::disabledPkgSelAutoInstall() :
						    YQIconPool::disabledPkgAutoInstall();	break;

	    case Status::S_AutoUpdate:		icon = bySelection ?
						    YQIconPool::disabledPkgSelAutoUpdate() :
						    YQIconPool::disabledPkgAutoUpdate();	break;

		// Intentionally omitting 'default' branch so the compiler can
		// catch unhandled enum states
	}
    }

    return icon;
}


QString
YQPkgObjList::statusText( zypp::ui::Status status ) const
{
    switch ( status )
    {
	case Status::S_AutoDel:		return _( "Autodelete" );
	case Status::S_AutoInstall:	return _( "Autoinstall" );
	case Status::S_AutoUpdate:	return _( "Autoupdate" );
	case Status::S_Del:		return _( "Delete" );
	case Status::S_Install:		return _( "Install" );
	case Status::S_KeepInstalled:	return _( "Keep" );
	case Status::S_NoInst:		return _( "Do Not Install" );
	case Status::S_Protected:	return _( "Protected -- Do Not Modify" );
	case Status::S_Taboo:		return _( "Taboo -- Never Install" );
	case Status::S_Update:		return _( "Update" );
    }

    return QString::null;
}


void
YQPkgObjList::setCurrentStatus( zypp::ui::Status	newStatus,
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
YQPkgObjList::setAllItemStatus( zypp::ui::Status newStatus, bool force )
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
	    if ( newStatus == Status::S_Update )
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
    actionSetCurrentInstall		= createAction( Status::S_Install,		"[+]"		);
    actionSetCurrentDontInstall		= createAction( Status::S_NoInst,		"[-]"		);
    actionSetCurrentKeepInstalled	= createAction( Status::S_KeepInstalled,	"[<], [-]"	);
    actionSetCurrentDelete		= createAction( Status::S_Del,			"[-]"		);
    actionSetCurrentUpdate		= createAction( Status::S_Update,		"[>], [+]"	);
    actionSetCurrentTaboo		= createAction( Status::S_Taboo,		"[!]"		);
    actionSetCurrentProtected		= createAction( Status::S_Protected, 		"[*]" 		);

    actionSetListInstall		= createAction( Status::S_Install,		"", true );
    actionSetListDontInstall		= createAction( Status::S_NoInst,		"", true );
    actionSetListKeepInstalled		= createAction( Status::S_KeepInstalled,	"", true );
    actionSetListDelete			= createAction( Status::S_Del,			"", true );
    actionSetListProtected		= createAction( Status::S_Protected, 		"", true );

    actionSetListUpdate			= createAction( _( "Update if newer version available" ),
							statusIcon( Status::S_Update, true ),
							statusIcon( Status::S_Update, false ),
							"",
							true );

    actionSetListUpdateForce		= createAction( _( "Update unconditionally" ),
							statusIcon( Status::S_Update, true ),
							statusIcon( Status::S_Update, false ),
							"",
							true );

    actionSetListTaboo			= createAction( Status::S_Taboo,		"", true );

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
YQPkgObjList::createAction( zypp::ui::Status status, const QString & key, bool enabled )
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
	zypp::ResObject::Ptr zyppObj = item->zyppObj();

	if ( zyppObj->hasInstalledObj() )
	{
	    actionSetCurrentInstall->setEnabled( false );
	    actionSetCurrentDontInstall->setEnabled( false );
	    actionSetCurrentTaboo->setEnabled( false );
	    actionSetCurrentProtected->setEnabled( true );

	    actionSetCurrentKeepInstalled->setEnabled( true );
	    actionSetCurrentDelete->setEnabled( true );
	    actionSetCurrentUpdate->setEnabled( zyppObj->hasCandidateObj() );
	}
	else
	{
	    actionSetCurrentInstall->setEnabled( zyppObj->hasCandidateObj() );
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
		bool installed = item->zyppObj()->hasInstalledObj();
		zypp::ui::Status status = item->status();

		switch( event->ascii() )
		{
		    case Qt::Key_Space:		// Cycle
			item->cycleStatus();
			event->accept();
			return;

		    case '+':	// Grab everything - install or update

			if ( installed )
			{
			    zypp::ui::Status newStatus = Status::S_KeepInstalled;

			    if ( item->candidateIsNewer() )
				newStatus = Status::S_Update;

			    setCurrentStatus( newStatus );
			}
			else
			    setCurrentStatus( Status::S_Install );
			selectNextItem();
			event->accept();
			return;

		    case '-':	// Get rid of everything - don't install or delete
			setCurrentStatus( installed ? Status::S_Del : Status::S_NoInst );
			selectNextItem();
			event->accept();
			return;

		    case '!':	// Taboo

			if ( ! installed )
			    setCurrentStatus( Status::S_Taboo );
			selectNextItem();
			event->accept();
			return;

		    case '*':	// Protected

			if ( installed )
			    setCurrentStatus( Status::S_Protected );
			selectNextItem();
			event->accept();
			return;

		    case '>':	// Update what is worth to be updated

			if ( installed && item->candidateIsNewer() )
			    setCurrentStatus( Status::S_Update );
			selectNextItem();
			event->accept();
			return;

		    case '<':	// Revert update

			if ( status == Status::S_Update ||
			     status == Status::S_AutoUpdate )
			{
			    setCurrentStatus( Status::S_KeepInstalled );
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




YQPkgObjListItem::YQPkgObjListItem( YQPkgObjList * pkgObjList, zypp::ResObject::Ptr pm_obj )
    : QY2ListViewItem( pkgObjList )
    , _pkgObjList( pkgObjList )
    , _zyppObj( pm_obj )
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

    zypp::ResObject::Ptr candidate = _zyppObj->getCandidateObj();
    zypp::ResObject::Ptr installed = _zyppObj->getInstalledObj();

    if ( candidate && installed && candidate->edition() != installed->edition() )
    {
	if ( _zyppObj->getSelectable()->downgrade_condition() )
	    _installedIsNewer = true;
	else
	    _candidateIsNewer = true;
    }


    if ( nameCol()    >= 0 )	setText( nameCol(),	zyppObj()->name()		);
    if ( summaryCol() >= 0 )	setText( summaryCol(),	zyppObj()->summary()	);
    if ( sizeCol()    >= 0 )	setText( sizeCol(),	zyppObj()->size().form() + "  " );

    if ( instVersionCol() >= 0 )
    {
	if ( zyppObj()->hasInstalledObj() )
	     setText( instVersionCol(), zyppObj()->getInstalledObj()->edition() );

	if ( ! zyppObj()->isInstalledObj() &&
	     ! zyppObj()->isCandidateObj()   )
	{
	    setText( versionCol(), zyppObj()->edition() );
	}
	else if ( zyppObj()->hasCandidateObj() )
	{
	    setText( versionCol(), zyppObj()->getCandidateObj()->edition() );
	}
    }
    else
    {
	setText( versionCol(),	zyppObj()->edition() );
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


zypp::ui::Status
YQPkgObjListItem::status() const
{
    Selectable::Ptr selectable = constZyppObj()->getSelectable();

    if ( ! selectable )
	return Status::S_NoInst;

    return selectable->status();
}


bool
YQPkgObjListItem::bySelection() const
{
    bool bySel = false;

    if ( _zyppObj->getSelectable() && _zyppObj->getSelectable()->by_appl() )
	bySel = true;

    return bySel;
}


void
YQPkgObjListItem::setStatus( zypp::ui::Status newStatus )
{
    zyppObj()->getSelectable()->set_status( newStatus );
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

    zypp::ui::Status oldStatus = status();
    zypp::ui::Status newStatus = oldStatus;
    bool showInsNotify = false;
    bool showDelNotify = false;

    if ( zyppObj()->hasInstalledObj() )
    {
	switch ( oldStatus )
	{
	    case Status::S_Protected:
		newStatus = zyppObj()->hasCandidateObj() ?
		    Status::S_KeepInstalled: Status::S_NoInst;
		break;

	    case Status::S_KeepInstalled:
		newStatus = zyppObj()->hasCandidateObj() ?
		    Status::S_Update : Status::S_Del;
		break;

	    case Status::S_Update:
		newStatus = Status::S_Del;
		showDelNotify = true;
		break;

	    case Status::S_Del:
		newStatus = Status::S_KeepInstalled;
		break;

	    default:
		newStatus = Status::S_KeepInstalled;
		break;
	}
    }
    else	// Pkg not installed
    {
	switch ( oldStatus )
	{
	    case Status::S_NoInst:
		if ( zyppObj()->hasCandidateObj() )
		{
		    newStatus = Status::S_Install;
		    showInsNotify = true;
		}
		else
		{
		    newStatus = Status::S_NoInst;
		}
		break;

	    case Status::S_AutoInstall:
		newStatus =  Status::S_Taboo;
		break;

	    default:
		newStatus = Status::S_NoInst;
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
YQPkgObjListItem::showNotifyTexts( zypp::ui::Status status )
{
    list<string> text;

    switch ( status )
    {
	case Status::S_Install:
	    if ( _zyppObj->hasCandidateObj() )
		text = _zyppObj->getCandidateObj()->insnotify();
	    break;

	case Status::S_NoInst:
	case Status::S_Del:
	case Status::S_Taboo:
	    if ( _zyppObj->hasCandidateObj() )
		text = _zyppObj->getCandidateObj()->delnotify();
	    break;

	default: break;
    }

    if ( ! text.empty() )
    {
	y2debug( "Showing notify text" );
	YQPkgTextDialog::showText( _pkgObjList, _zyppObj, text );
    }
}


bool
YQPkgObjListItem::showLicenseAgreement( zypp::ui::Status status )
{
    bool confirmed = true;
    list<string> text;
    zypp::Package::Ptr pkg = NULL;

    switch ( status )
    {
	case Status::S_Install:
	case Status::S_Update:
	    if ( _zyppObj->hasCandidateObj() )
	    {
		pkg =  _zyppObj->getCandidateObj();

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
	    || YQPkgTextDialog::confirmText( _pkgObjList, _zyppObj, text );

	if ( ! confirmed )
	{
	    // The user rejected the license agreement -
	    // make sure the package gets unselected.

	    switch ( status )
	    {
		case Status::S_Install:
		    setStatus( Status::S_Taboo );
		    break;

		case Status::S_Protected:
		    setStatus( Status::S_Del );
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
	    case Status::S_AutoDel:
	    case Status::S_AutoInstall:
	    case Status::S_AutoUpdate:

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

	    if ( this->constZyppObj()->size() < other->constZyppObj()->size() ) return -1;
	    if ( this->constZyppObj()->size() > other->constZyppObj()->size() ) return 1;
	    return 0;
	}
	else if ( col == statusCol() )
	{
	    // Sorting by status depends on the numeric value of the
	    // zypp::ui::Status enum, thus it is important to insert new
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
	    // - Installed newer than candidate (red)
	    // - Candidate newer than installed (blue) - worthwhile updating
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
    if ( _zyppObj->hasInstalledObj() )	points += 10;
    if ( _zyppObj->hasCandidateObj() )	points += 1;

    return points;
}



#include "YQPkgObjList.moc"
