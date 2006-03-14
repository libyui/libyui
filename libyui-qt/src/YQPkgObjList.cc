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

#include <zypp/ResStatus.h>

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
YQPkgObjList::addPkgObjItem( ZyppSel selectable, ZyppObj zyppObj )
{
    if ( ! selectable )
    {
	y2error( "Null zypp::ui::Selectable!" );
	return;
    }

    new YQPkgObjListItem( this, selectable, zyppObj );
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
		    item->selectable()->hasInstalledObj() ?
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

    emit selectionChanged( item ? item->selectable() : ZyppSel() );
}


void
YQPkgObjList::clear()
{
    emit selectionChanged( ZyppSel() );
    QY2ListView::clear();
}


QPixmap
YQPkgObjList::statusIcon( ZyppStatus status, bool enabled, bool bySelection )
{
    QPixmap icon = YQIconPool::pkgNoInst();

    if ( enabled )
    {
	switch ( status )
	{
	    case S_Del:			icon = YQIconPool::pkgDel();		break;
	    case S_Install:		icon = YQIconPool::pkgInstall();	break;
	    case S_KeepInstalled:	icon = YQIconPool::pkgKeepInstalled();	break;
	    case S_NoInst:		icon = YQIconPool::pkgNoInst();		break;
	    case S_Protected:		icon = YQIconPool::pkgProtected();	break;
	    case S_Taboo:		icon = YQIconPool::pkgTaboo();		break;
	    case S_Update:		icon = YQIconPool::pkgUpdate();		break;

	    case S_AutoDel:		icon = bySelection ?
		 			    YQIconPool::pkgSelAutoDel() :
		 			    YQIconPool::pkgAutoDel();		break;

	    case S_AutoInstall:		icon = bySelection ?
		 			    YQIconPool::pkgSelAutoInstall() :
		 			    YQIconPool::pkgAutoInstall();	break;

	    case S_AutoUpdate:		icon = bySelection ?
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
	    case S_Del:			icon = YQIconPool::disabledPkgDel();		break;
	    case S_Install:		icon = YQIconPool::disabledPkgInstall();	break;
	    case S_KeepInstalled:	icon = YQIconPool::disabledPkgKeepInstalled();	break;
	    case S_NoInst:		icon = YQIconPool::disabledPkgNoInst();		break;
	    case S_Protected:		icon = YQIconPool::disabledPkgProtected();	break;
	    case S_Taboo:		icon = YQIconPool::disabledPkgTaboo();		break;
	    case S_Update:		icon = YQIconPool::disabledPkgUpdate();		break;

	    case S_AutoDel:		icon = bySelection ?
		 			    YQIconPool::disabledPkgSelAutoDel() :
		 			    YQIconPool::disabledPkgAutoDel();		break;

	    case S_AutoInstall:		icon = bySelection ?
		 			    YQIconPool::disabledPkgSelAutoInstall() :
		 			    YQIconPool::disabledPkgAutoInstall();	break;

	    case S_AutoUpdate:		icon = bySelection ?
					    YQIconPool::disabledPkgSelAutoUpdate() :
					    YQIconPool::disabledPkgAutoUpdate();	break;

		// Intentionally omitting 'default' branch so the compiler can
		// catch unhandled enum states
	}
    }

    return icon;
}


QString
YQPkgObjList::statusText( ZyppStatus status ) const
{
    switch ( status )
    {
	case S_AutoDel:		return _( "Autodelete" 			);
	case S_AutoInstall:	return _( "Autoinstall" 		);
	case S_AutoUpdate:	return _( "Autoupdate" 			);
	case S_Del:		return _( "Delete" 			);
	case S_Install:		return _( "Install" 			);
	case S_KeepInstalled:	return _( "Keep" 			);
	case S_NoInst:		return _( "Do Not Install" 		);
	case S_Protected:	return _( "Protected -- Do Not Modify"	);
	case S_Taboo:		return _( "Taboo -- Never Install" 	);
	case S_Update:		return _( "Update" 			);
    }

    return QString::null;
}


void
YQPkgObjList::setCurrentStatus( ZyppStatus newStatus, bool doSelectNextItem )
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
YQPkgObjList::setAllItemStatus( ZyppStatus newStatus, bool force )
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
	    if ( newStatus == S_Update )
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
    actionSetCurrentInstall		= createAction( S_Install,		"[+]"		);
    actionSetCurrentDontInstall		= createAction( S_NoInst,		"[-]"		);
    actionSetCurrentKeepInstalled	= createAction( S_KeepInstalled,	"[<], [-]"	);
    actionSetCurrentDelete		= createAction( S_Del,			"[-]"		);
    actionSetCurrentUpdate		= createAction( S_Update,		"[>], [+]"	);
    actionSetCurrentTaboo		= createAction( S_Taboo,		"[!]"		);
    actionSetCurrentProtected		= createAction( S_Protected, 		"[*]" 		);

    actionSetListInstall		= createAction( S_Install,		"", true );
    actionSetListDontInstall		= createAction( S_NoInst,		"", true );
    actionSetListKeepInstalled		= createAction( S_KeepInstalled,	"", true );
    actionSetListDelete			= createAction( S_Del,			"", true );
    actionSetListProtected		= createAction( S_Protected, 		"", true );

    actionSetListUpdate			= createAction( _( "Update if newer version available" ),
							statusIcon( S_Update, true ),
							statusIcon( S_Update, false ),
							"",
							true );

    actionSetListUpdateForce		= createAction( _( "Update unconditionally" ),
							statusIcon( S_Update, true ),
							statusIcon( S_Update, false ),
							"",
							true );

    actionSetListTaboo			= createAction( S_Taboo,		"", true );

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
YQPkgObjList::createAction( ZyppStatus status, const QString & key, bool enabled )
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
	ZyppSel selectable = item->selectable();

	if ( selectable->hasInstalledObj() )
	{
	    actionSetCurrentInstall->setEnabled( false );
	    actionSetCurrentDontInstall->setEnabled( false );
	    actionSetCurrentTaboo->setEnabled( false );
	    actionSetCurrentProtected->setEnabled( true );

	    actionSetCurrentKeepInstalled->setEnabled( true );
	    actionSetCurrentDelete->setEnabled( true );
	    actionSetCurrentUpdate->setEnabled( selectable->hasCandidateObj() );
	}
	else
	{
	    actionSetCurrentInstall->setEnabled( selectable->hasCandidateObj() );
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
		bool installed = item->selectable()->hasInstalledObj();
		ZyppStatus status = item->status();

		switch( event->ascii() )
		{
		    case Qt::Key_Space:		// Cycle
			item->cycleStatus();
			event->accept();
			return;

		    case '+':	// Grab everything - install or update

			if ( installed )
			{
			    ZyppStatus newStatus = S_KeepInstalled;

			    if ( item->candidateIsNewer() )
				newStatus = S_Update;

			    setCurrentStatus( newStatus );
			}
			else
			    setCurrentStatus( S_Install );
			selectNextItem();
			event->accept();
			return;

		    case '-':	// Get rid of everything - don't install or delete
			setCurrentStatus( installed ? S_Del : S_NoInst );
			selectNextItem();
			event->accept();
			return;

		    case '!':	// Taboo

			if ( ! installed )
			    setCurrentStatus( S_Taboo );
			selectNextItem();
			event->accept();
			return;

		    case '*':	// Protected

			if ( installed )
			    setCurrentStatus( S_Protected );
			selectNextItem();
			event->accept();
			return;

		    case '>':	// Update what is worth to be updated

			if ( installed && item->candidateIsNewer() )
			    setCurrentStatus( S_Update );
			selectNextItem();
			event->accept();
			return;

		    case '<':	// Revert update

			if ( status == S_Update ||
			     status == S_AutoUpdate )
			{
			    setCurrentStatus( S_KeepInstalled );
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




YQPkgObjListItem::YQPkgObjListItem( YQPkgObjList * pkgObjList,
				    ZyppSel selectable,
				    ZyppObj zyppObj )
    : QY2ListViewItem( pkgObjList )
    , _pkgObjList( pkgObjList )
    , _selectable( selectable )
    , _zyppObj( zyppObj )
    , _editable( true )
{
    init();
}


YQPkgObjListItem::YQPkgObjListItem( YQPkgObjList *	pkgObjList,
				    QY2ListViewItem *	parent,
				    ZyppSel 		selectable,
				    ZyppObj 		zyppObj )
    : QY2ListViewItem( parent )
    , _pkgObjList( pkgObjList )
    , _selectable( selectable )
    , _zyppObj( zyppObj )
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
    if ( _zyppObj == 0 && _selectable )
	_zyppObj = _selectable->theObj();


    _candidateIsNewer = false;
    _installedIsNewer = false;

    const ZyppObj candidate = selectable()->candidateObj();
    const ZyppObj installed = selectable()->installedObj();

    if ( candidate && installed )
    {
	if ( candidate->edition() < installed->edition() )	
	    _installedIsNewer = true;
	else if ( installed->edition() < candidate->edition() )	
	    _candidateIsNewer = true;
    }

    if ( nameCol()    >= 0 )	setText( nameCol(),	zyppObj()->name()	);
    if ( summaryCol() >= 0 )	setText( summaryCol(),	zyppObj()->summary()	);
    if ( sizeCol()    >= 0 )	setText( sizeCol(),	zyppObj()->size().asString() + "  " );

    if ( instVersionCol() >= 0 )
    {
	if ( selectable()->hasInstalledObj() )
	     setText( instVersionCol(), installed->edition() );

	if ( zyppObj() != selectable()->installedObj() &&
	     zyppObj() != selectable()->candidateObj()   )
	{
	    setText( versionCol(), zyppObj()->edition() );
	}
	else if ( selectable()->hasCandidateObj() )
	{
	    setText( versionCol(), candidate->edition() );
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
YQPkgObjListItem::setText( int column, const zypp::Edition & edition )
{
    setText( column, edition.asString() );
}


ZyppStatus
YQPkgObjListItem::status() const
{
    if ( ! selectable() )
    {
	y2error( "No selectable" );
	return S_NoInst;
    }

    return selectable()->status();
}


bool
YQPkgObjListItem::bySelection() const
{
    zypp::ResStatus::TransactByValue modifiedBy = selectable()->modifiedBy();

    return ( modifiedBy == zypp::ResStatus::APPL_LOW ||
	     modifiedBy == zypp::ResStatus::APPL_HIGH  );
}


void
YQPkgObjListItem::setStatus( ZyppStatus newStatus )
{
    selectable()->set_status( newStatus );
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

    ZyppStatus oldStatus = status();
    ZyppStatus newStatus = oldStatus;

    if ( selectable()->hasInstalledObj() )
    {
	switch ( oldStatus )
	{
	    case S_Protected:
		newStatus = selectable()->hasCandidateObj() ?
		    S_KeepInstalled: S_NoInst;
		break;

	    case S_KeepInstalled:
		newStatus = selectable()->hasCandidateObj() ?
		    S_Update : S_Del;
		break;

	    case S_Update:
		newStatus = S_Del;
		break;

	    case S_Del:
		newStatus = S_KeepInstalled;
		break;

	    default:
		newStatus = S_KeepInstalled;
		break;
	}
    }
    else	// Pkg not installed
    {
	switch ( oldStatus )
	{
	    case S_NoInst:
		if ( selectable()->hasCandidateObj() )
		{
		    newStatus = S_Install;
		}
		else
		{
		    y2warning( "No candidate for %s", selectable()->theObj()->name().c_str() );
		    newStatus = S_NoInst;
		}
		break;

	    case S_AutoInstall:
		newStatus =  S_Taboo;
		break;

	    default:
		newStatus = S_NoInst;
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
YQPkgObjListItem::showNotifyTexts( ZyppStatus status )
{
    string text;

    switch ( status )
    {
	case S_Install:
	    if ( selectable()->hasCandidateObj() )
		text = selectable()->candidateObj()->insnotify();
	    break;

	case S_NoInst:
	case S_Del:
	case S_Taboo:
	    if ( selectable()->hasCandidateObj() )
		text = selectable()->candidateObj()->delnotify();
	    break;

	default: break;
    }

    if ( ! text.empty() )
    {
	y2debug( "Showing notify text" );
	YQPkgTextDialog::showText( _pkgObjList, selectable(), text );
    }
}


bool
YQPkgObjListItem::showLicenseAgreement( ZyppStatus status )
{
    bool confirmed = true;
    string licenseText;
    ZyppPkg pkg = 0;

    switch ( status )
    {
	case S_Install:
	case S_Update:
	    
	    if ( selectable()->hasLicenceConfirmed() )
		return true;
	    
	    if ( selectable()->hasCandidateObj() )
	    {
		pkg = tryCastToZyppPkg( selectable()->candidateObj() );

		if ( pkg )
		    licenseText = pkg->licenseToConfirm();
	    }
	    break;

	default: return true;
    }

    if ( ! licenseText.empty() )
    {
	y2debug( "Showing license agreement" );
	confirmed = YQPkgTextDialog::confirmText( _pkgObjList, selectable(), licenseText );

	if ( confirmed )
	{
	    selectable()->setLicenceConfirmed( true );
	}
	else
	{
	    // The user rejected the license agreement -
	    // make sure the package gets unselected.

	    switch ( status )
	    {
		case S_Install:
		    setStatus( S_Taboo );
		    break;

		case S_Protected:
		    setStatus( S_Del );
		    break;

		default: break;
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
	    case S_AutoDel:
	    case S_AutoInstall:
	    case S_AutoUpdate:

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

	    if ( this->zyppObj()->size() < other->zyppObj()->size() ) return -1;
	    if ( this->zyppObj()->size() > other->zyppObj()->size() ) return 1;
	    return 0;
	}
	else if ( col == statusCol() )
	{
	    // Sorting by status depends on the numeric value of the
	    // ZyppStatus enum, thus it is important to insert new
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

    if ( installedIsNewer() )			points += 1000;
    if ( candidateIsNewer() )			points += 100;
    if ( selectable()->hasInstalledObj() )	points += 10;
    if ( selectable()->hasCandidateObj() )	points += 1;

    return points;
}



#include "YQPkgObjList.moc"
