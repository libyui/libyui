/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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

  Textdomain "qt-pkg"

/-*/

#define YUILogComponent "qt-pkg"
#include "YUILog.h"
#include <QPixmap>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QKeyEvent>

#include "utf8.h"

#include "YDialog.h"
#include "YQPkgObjList.h"
#include "YQPkgTextDialog.h"
#include "YQi18n.h"
#include "YQIconPool.h"
#include "YQUI.h"

#include "zypp/ZYppFactory.h"

using std::list;
using std::string;


#define VERBOSE_EXCLUDE_RULES	0
#define EXTRA_SOLVE_COLLECTIONS	0


YQPkgObjList::YQPkgObjList( QWidget * parent )
    : QY2ListView( parent )
    , _editable( true )
    , _installedContextMenu(0)
    , _notInstalledContextMenu(0)
    , actionSetCurrentInstall(0)
    , actionSetCurrentDontInstall(0)
    , actionSetCurrentKeepInstalled(0)
    , actionSetCurrentDelete(0)
    , actionSetCurrentUpdate(0)
    , actionSetCurrentTaboo(0)
    , actionSetCurrentProtected(0)
    , actionShowCurrentSolverInfo(0)
    , actionSetListInstall(0)
    , actionSetListDontInstall(0)
    , actionSetListKeepInstalled(0)
    , actionSetListDelete(0)
    , actionSetListUpdate(0)
    , actionSetListUpdateForce(0)
    , actionSetListTaboo(0)
    , actionSetListProtected(0)
{
    // This class does not add any columns. This is the main reason why this is
    // an abstract base class: It doesn't know which columns are desired and in
    // what order.

    _iconCol 		= -42;
    _statusCol		= -42;
    _nameCol		= -42;
    _versionCol		= -42;
    _instVersionCol	= -42;
    _summaryCol		= -42;
    _sizeCol		= -42;
    _brokenIconCol	= -42;
    _satisfiedIconCol	= -42;
    _debug		= false;

    _excludedItems = new YQPkgObjList::ExcludedItems( this );

    createActions();

    connect( this,	SIGNAL( columnClicked		( int, QTreeWidgetItem *, int, const QPoint & ) ),
	     this,	SLOT  ( pkgObjClicked		( int, QTreeWidgetItem *, int, const QPoint & ) ) );

    connect( this,	SIGNAL( columnDoubleClicked	( int, QTreeWidgetItem *, int, const QPoint & ) ),
	     this,	SLOT  ( pkgObjClicked		( int, QTreeWidgetItem *, int, const QPoint & ) ) );

    connect( this,	SIGNAL( currentItemChanged	( QTreeWidgetItem *, QTreeWidgetItem * ) ),
	     this,	SLOT  ( currentItemChangedInternal( QTreeWidgetItem * ) ) );

    setIconSize( QSize( 22, 16 ) );
}


YQPkgObjList::~YQPkgObjList()
{
    if ( _excludedItems )
	delete _excludedItems;
}


void
YQPkgObjList::addPkgObjItem( ZyppSel selectable, ZyppObj zyppObj )
{
    if ( ! selectable )
    {
	yuiError() << "Null zypp::ui::Selectable!" << endl;
	return;
    }

    YQPkgObjListItem * item = new YQPkgObjListItem( this, selectable, zyppObj );
    applyExcludeRules( item );
}


void
YQPkgObjList::addPassiveItem( const QString & 	name,
			      const QString & 	summary,
			      FSize		size )
{
    QY2ListViewItem * item = new QY2ListViewItem( this, QString::null );

    if ( item )
    {
	if ( nameCol()    >= 0 && ! name.isEmpty()    )	item->setText( nameCol(),	name    );
	if ( summaryCol() >= 0 && ! summary.isEmpty() )	item->setText( summaryCol(),	summary );
	if ( sizeCol()    >= 0 && size > 0L	      )
	{
	    QString sizeStr = size.form().c_str();
	    item->setText( sizeCol(), sizeStr );
	}
    }
}


void
YQPkgObjList::pkgObjClicked( int		button,
			     QTreeWidgetItem *	listViewItem,
			     int		col,
			     const QPoint &	pos )
{
    YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (listViewItem);

    if ( item )
    {
        //y2internal("CLICKED: %s", item->zyppObj()->name().c_str());
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

                if ( ! item->selectable() )
                    return;

		QMenu * contextMenu =
		    ! item->selectable()->installedEmpty() ?
		    installedContextMenu() : notInstalledContextMenu();

		if ( contextMenu )
		    contextMenu->popup( pos );
	    }
	}
    }
}


void
YQPkgObjList::currentItemChangedInternal( QTreeWidgetItem * listViewItem )
{
    YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (listViewItem);

    emit currentItemChanged( item ? item->selectable() : ZyppSel() );
}


void
YQPkgObjList::clear()
{
    emit currentItemChanged( ZyppSel() );

    _excludedItems->clear();
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

	    case S_AutoDel:		icon = YQIconPool::pkgAutoDel();	break;
	    case S_AutoInstall:		icon = YQIconPool::pkgAutoInstall();	break;
	    case S_AutoUpdate:		icon = YQIconPool::pkgAutoUpdate();	break;

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

	    case S_AutoDel:		icon = YQIconPool::disabledPkgAutoDel();        break;
	    case S_AutoInstall:		icon = YQIconPool::disabledPkgAutoInstall();	break;
	    case S_AutoUpdate:		icon = YQIconPool::disabledPkgAutoUpdate();	break;

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
    QTreeWidgetItem * listViewItem = currentItem();

    if ( ! listViewItem )
	return;

    YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (listViewItem);

    if ( item && item->editable() && _editable )
    {
	if ( newStatus != item->status() )
	{
	    item->setStatus( newStatus );

	    if ( item->showLicenseAgreement() )
	    {
		item->showNotifyTexts( newStatus );
	    }
	    else // License not confirmed?
	    {
		// Status is now S_Taboo or S_Del - update status icon
		item->setStatusIcon();
	    }

	    emit statusChanged();
	}
    }

    if ( doSelectNextItem )
	selectNextItem();
}


void
YQPkgObjList::showSolverInfo()
{
    QTreeWidgetItem * listViewItem = currentItem();

    if ( ! listViewItem )
	return;

    YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (listViewItem);

    if ( item )
    {
	_plugin.createZyppSolverDialog(item->zyppObj()->poolItem());
    }
}


void
YQPkgObjList::setAllItemStatus( ZyppStatus newStatus, bool force )
{
    if ( ! _editable )
	return;

    YQUI::ui()->busyCursor();
    QTreeWidgetItemIterator it( this );

    while ( *it )
    {
	YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (*it);

	if ( item && item->editable() && newStatus != item->status() )
	{
	    if ( newStatus == S_Update )
	    {
		if ( ( item->candidateIsNewer() && item->status() != S_Protected ) || force )
		    item->setStatus( newStatus,
				     false );	// sendSignals
	    }
	    else
	    {
		item->setStatus( newStatus,
				 false );	// sendSignals
	    }
	}

	++it;
    }

    emit updateItemStates();
    emit updatePackages();

    YQUI::ui()->normalCursor();
    emit statusChanged();
}


void
YQPkgObjList::selectNextItem()
{
    QTreeWidgetItemIterator it(this);
    QTreeWidgetItem * item;

    while ( (item = *it) != NULL )
    {
        ++it;
	//item->setSelected( false );			// Doesn't emit signals
	scrollToItem( *it );	// Scroll if necessary
	setCurrentItem( *it );	// Emits signals

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

    actionShowCurrentSolverInfo		= createAction( _( "Show solver information" ));

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
    connect( actionShowCurrentSolverInfo,    SIGNAL( activated() ), this, SLOT( showCurrentSolverInfo()	  ) );
    connect( actionSetListInstall,	     SIGNAL( activated() ), this, SLOT( setListInstall()	  ) );
    connect( actionSetListDontInstall,	     SIGNAL( activated() ), this, SLOT( setListDontInstall()	  ) );
    connect( actionSetListKeepInstalled,     SIGNAL( activated() ), this, SLOT( setListKeepInstalled()	  ) );
    connect( actionSetListDelete,	     SIGNAL( activated() ), this, SLOT( setListDelete()		  ) );
    connect( actionSetListUpdate,	     SIGNAL( activated() ), this, SLOT( setListUpdate()		  ) );
    connect( actionSetListUpdateForce,	     SIGNAL( activated() ), this, SLOT( setListUpdateForce()	  ) );
    connect( actionSetListTaboo,	     SIGNAL( activated() ), this, SLOT( setListTaboo()		  ) );
    connect( actionSetListProtected,	     SIGNAL( activated() ), this, SLOT( setListProtected()	  ) );

    // if the solver info plugin did not success to load (which is, the package
    // of the plugin is not installed or was not included in the media
    //
    // it will show up a popup when called, however, if we are in installation
    // (that is, target is not / or there is no target at all,
    // the user will have no chance of installing
    // the plugin package, therefore we disable the action.
    //
    zypp::Target_Ptr target = zypp::getZYpp()->getTarget();
    if ( ! target || ( target->root() != "/" ) )
    {
        // there is no target or the target is mounted out of root
        // which means install or update
        // if the plugin did not load
        if ( ! _plugin.success() )
        {
            // grey out the option
            yuiMilestone() << "Disabling solver info plugin: not available and no target or target is not /" << endl;
            actionShowCurrentSolverInfo->setVisible(false);
        }
        else
        {
            yuiMilestone() << "target not available or target or target is not /. Solver info plugin available anyway." << endl;
        }
    }
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


    QIcon iconSet ( icon );

    if ( ! insensitiveIcon.isNull() )
    {
	iconSet.addPixmap( insensitiveIcon,
			   QIcon::Disabled );
    }

    QAction * action = new QAction( label,	// text
				    this );	// parent
    Q_CHECK_PTR( action );
    action->setEnabled( enabled );
    action->setIcon( iconSet );

    return action;
}


void
YQPkgObjList::createNotInstalledContextMenu()
{
    _notInstalledContextMenu = new QMenu( this );
    Q_CHECK_PTR( _notInstalledContextMenu );

    _notInstalledContextMenu->addAction( actionSetCurrentInstall	);
    _notInstalledContextMenu->addAction( actionSetCurrentDontInstall	);
    _notInstalledContextMenu->addAction( actionSetCurrentTaboo		);
    _notInstalledContextMenu->addAction( actionShowCurrentSolverInfo	);

    addAllInListSubMenu( _notInstalledContextMenu );
}


void
YQPkgObjList::createInstalledContextMenu()
{
    _installedContextMenu = new QMenu( this );
    Q_CHECK_PTR( _installedContextMenu );

    _installedContextMenu->addAction( actionSetCurrentKeepInstalled	);
    _installedContextMenu->addAction( actionSetCurrentDelete		);
    _installedContextMenu->addAction( actionSetCurrentUpdate		);
    _installedContextMenu->addAction( actionShowCurrentSolverInfo	);

    addAllInListSubMenu( _installedContextMenu );
}


QMenu *
YQPkgObjList::addAllInListSubMenu( QMenu * menu )
{
    QMenu * submenu = new QMenu( menu );
    Q_CHECK_PTR( submenu );

    submenu->addAction( actionSetListInstall		);
    submenu->addAction( actionSetListDontInstall 	);
    submenu->addAction( actionSetListKeepInstalled 	);
    submenu->addAction( actionSetListDelete 		);
    submenu->addAction( actionSetListUpdate 		);
    submenu->addAction( actionSetListUpdateForce 	);
    submenu->addAction( actionSetListTaboo 		);

    QAction *action = menu->addMenu( submenu );
    action->setText( _( "&All in This List" ) );

    return submenu;
}


QMenu *
YQPkgObjList::notInstalledContextMenu()
{
    if ( ! _notInstalledContextMenu )
	createNotInstalledContextMenu();

    return _notInstalledContextMenu;
}


QMenu *
YQPkgObjList::installedContextMenu()
{
    if ( ! _installedContextMenu )
	createInstalledContextMenu();

    return _installedContextMenu;
}


void
YQPkgObjList::updateActions( YQPkgObjListItem * item )
{
    if ( !item)
      item = dynamic_cast<YQPkgObjListItem *> ( currentItem() );

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
	actionShowCurrentSolverInfo->setEnabled( true );
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

	actionShowCurrentSolverInfo->setEnabled( false );

    }
}


void
YQPkgObjList::keyPressEvent( QKeyEvent * event )
{
    if ( event )
    {
	int special_combo = ( Qt::ControlModifier | Qt::ShiftModifier | Qt::AltModifier );

	if ( ( event->modifiers() & special_combo ) == special_combo )
	{
	    if ( event->key() == Qt::Key_Q )
	    {
		_debug= ! _debug;
		yuiMilestone() << "Debug mode: " << _debug << endl;
	    }

	}
	QTreeWidgetItem * selectedListViewItem = currentItem();

	if ( selectedListViewItem )
	{
	    YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (selectedListViewItem);

	    if ( item )
	    {
                bool installed;

                if ( item->selectable() )
                    installed = item->selectable()->hasInstalledObj();
                else
                    installed = false;

		ZyppStatus status = item->status();

		switch( event->key() )
		{
		    case Qt::Key_Space:		// Cycle
			item->cycleStatus();
			event->accept();
			return;

		    case Qt::Key_Plus:	// Grab everything - install or update

			if ( installed )
			{
			    ZyppStatus newStatus = S_KeepInstalled;

			    if ( item->candidateIsNewer() )
				newStatus = S_Update;

			    setCurrentStatus( newStatus );
			}
			else
			    setCurrentStatus( S_Install );
			event->accept();
			return;

		    case Qt::Key_Minus:	// Get rid of everything - don't install or delete
			setCurrentStatus( installed ? S_Del : S_NoInst );
			event->accept();
			return;

		    case Qt::Key_Exclam:	// Taboo

			if ( ! installed )
			    setCurrentStatus( S_Taboo );
			event->accept();
			return;

		    case Qt::Key_Asterisk:	// Protected

			if ( installed )
			    setCurrentStatus( S_Protected );
			event->accept();
			return;

		    case Qt::Key_Greater:	// Update what is worth to be updated

			if ( installed && item->candidateIsNewer() )
			    setCurrentStatus( S_Update );
			event->accept();
			return;

		    case Qt::Key_Less:	// Revert update

			if ( status == S_Update ||
			     status == S_AutoUpdate )
			{
			    setCurrentStatus( S_KeepInstalled );
			}
			event->accept();
			return;

		    case Qt::Key_B:	// Toggle debugIsBroken flag

			if ( _debug )
			{
			    item->toggleDebugIsBroken();
			    item->setStatusIcon();
			}
			event->accept();
			break;

		    case Qt::Key_S:	// Toggle debugIsSatisfied flag

			if ( _debug )
			{
			    item->toggleDebugIsSatisfied();
			    item->setStatusIcon();
			}
			event->accept();
			break;
		}
	    }
	}
    }
    QY2ListView::keyPressEvent( event );
}


void
YQPkgObjList::message( const QString & text )
{
    QY2ListViewItem * item = new QY2ListViewItem( this );
    Q_CHECK_PTR( item );

    item->setText( nameCol() >= 0 ? nameCol() : 0, text );
    item->setBackgroundColor( 0, QColor( 0xE0, 0xE0, 0xF8 ) );
}


void
YQPkgObjList::addExcludeRule( YQPkgObjList::ExcludeRule * rule )
{
    _excludeRules.push_back( rule );
}


void
YQPkgObjList::applyExcludeRules()
{
    // yuiDebug() << "Applying exclude rules" << endl;
    QTreeWidgetItemIterator listView_it( this );

    while ( *listView_it )
    {
	QTreeWidgetItem * current_item = *listView_it;

	// Advance iterator now so it remains valid even if there are changes
	// to the QListView, e.g., if the current item is excluded and thus
	// removed from the QListView
	++listView_it;

	applyExcludeRules( current_item );
    }

    ExcludedItems::iterator excluded_it = _excludedItems->begin();

    while ( excluded_it != _excludedItems->end() )
    {
	QTreeWidgetItem * current_item = (*excluded_it).first;

	// Advance iterator now so it remains valid even if there are changes
	// to the excluded items, e.g., if the current item is un-excluded and thus
	// removed from the excluded items
	++excluded_it;

	applyExcludeRules( current_item );
    }

    logExcludeStatistics();
}


void
YQPkgObjList::logExcludeStatistics()
{
    if ( _excludedItems->size() > 0 )
    {
	yuiMilestone() << _excludedItems->size() << " packages excluded" << endl;

	for ( ExcludeRuleList::iterator rule_it = _excludeRules.begin();
	      rule_it != _excludeRules.end();
	      ++rule_it )
	{
	    ExcludeRule * rule = *rule_it;

	    if ( rule->isEnabled() )
	    {
		yuiMilestone() << "Active exclude rule: \""
			       << rule->regexp().pattern() << "\""
			       << endl;
	    }
	}
    }
}

void
YQPkgObjList::applyExcludeRules( QTreeWidgetItem * listViewItem )
{
    YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *>( listViewItem );

    if ( item )
    {
	bool exclude = false;
	ExcludeRule * matchingRule = 0;

	for ( ExcludeRuleList::iterator rule_it = _excludeRules.begin();
	      rule_it != _excludeRules.end() && ! exclude;
	      ++rule_it )
	{
	    ExcludeRule * rule = *rule_it;

	    if ( rule->match( item ) )
	    {
		exclude = true;
		matchingRule = rule;
	    }
	}

	if ( exclude != item->isExcluded() )	// change exclude status?
	{
	    this->exclude( item, exclude );

#if VERBOSE_EXCLUDE_RULES
	    if ( exclude )
	    {
		yuiDebug() << "Rule \"" << matchingRule->regexp().pattern()
			   << "\" matches: Excluding " << item->zyppObj()->name()
			   << endl;
	    }
	    else
	    {
		yuiDebug() << "Un-excluding " << item->zyppObj()->name() << endl;
	    }
#endif
	}
    }
}


void
YQPkgObjList::exclude( YQPkgObjListItem * item, bool exclude )
{
    if ( exclude == item->isExcluded() )
	return;

    item->setExcluded( exclude );

    QTreeWidgetItem * parentItem = item->parent();

    if ( parentItem )
        parentItem->setHidden(exclude);
    else
        item->setHidden(exclude);

    _excludedItems->add( item, parentItem );
}




YQPkgObjListItem::YQPkgObjListItem( YQPkgObjList * pkgObjList,
				    ZyppSel selectable,
				    ZyppObj zyppObj )
    : QY2ListViewItem( pkgObjList )
    , _pkgObjList( pkgObjList )
    , _selectable( selectable )
    , _zyppObj( zyppObj )
    , _editable( true )
    , _excluded( false )
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
    , _excluded( false )
{
    init();
}


YQPkgObjListItem::YQPkgObjListItem( YQPkgObjList *	pkgObjList )
    : QY2ListViewItem( pkgObjList )
    , _pkgObjList( pkgObjList )
    , _selectable( 0 )
    , _zyppObj( 0 )
    , _editable( true )
    , _excluded( false )
{
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

    _debugIsBroken	= false;
    _debugIsSatisfied	= false;
    _candidateIsNewer	= false;
    _installedIsNewer 	= false;

    const ZyppObj candidate = selectable()->candidateObj();
    const ZyppObj installed = selectable()->installedObj();

    if ( candidate && installed )
    {
        if ( candidate->edition() < installed->edition() )
            _installedIsNewer = true;
        else if ( installed->edition() < candidate->edition() )
            _candidateIsNewer = true;
    }

    if ( installed && ! candidate )
	_installedIsNewer = true;

    if ( nameCol()    >= 0 )	setText( nameCol(),	zyppObj()->name()	);
    if ( summaryCol() >= 0 )	setText( summaryCol(),	zyppObj()->summary()	);

    if ( sizeCol()    >= 0 )
    {
        zypp::ByteCount size = zyppObj()->installSize();

        if ( size > 0L )
            setText( sizeCol(),	size.asString() );
    }

    if ( versionCol() == instVersionCol() ) // Display both versions in the same column: 1.2.3 (1.2.4)
    {
	if ( versionCol() >= 0 )
	{
	    setBackgroundColor( versionCol(), _pkgObjList->palette().color( QPalette::AlternateBase ) );

	    if ( installed )
	    {
		if ( zyppObj() != installed  &&
		     zyppObj() != candidate )
		{
		    setText( versionCol(), QString().sprintf( "%s", zyppObj()->edition().c_str() ) );
		}
		else
		{
		    if ( candidate && installed->edition() != candidate->edition() )
		    {
			setText( versionCol(),
				 QString().sprintf( "%s (%s)",
						    installed->edition().c_str(),
						    candidate->edition().c_str() ) );
		    }
		    else // no candidate or both versions are the same anyway
		    {
			setText( versionCol(),
				 QString().sprintf( "%s", installed->edition().c_str() ) );
		    }
		}
	    }
	    else
	    {
		if ( candidate )
		    setText( versionCol(), QString().sprintf( "(%s)", candidate->edition().c_str() ) );
		else
		    setText( versionCol(), zyppObj()->edition() );
	    }

	    if ( _installedIsNewer )
		setTextColor( versionCol(), Qt::red);
	    else if ( _candidateIsNewer )
		setTextColor( versionCol(), Qt::blue);
	}
    }
    else // separate columns for installed and available versions
    {
	if ( instVersionCol() >= 0 )
	{
	    if ( installed )
	    {
		setText( instVersionCol(), installed->edition() );

		if ( _installedIsNewer )
		    setTextColor( instVersionCol(), Qt::red);
		else if ( _candidateIsNewer )
		    setTextColor( instVersionCol(), Qt::blue);
	    }
	}

	if ( versionCol() >= 0 )
	{
	    if ( zyppObj() != installed &&
		 zyppObj() != candidate )
	    {
		setText( versionCol(), zyppObj()->edition() );
	    }
	    else if ( candidate )
	    {
		setText( versionCol(), candidate->edition() );

		if ( _installedIsNewer )
		    setTextColor( versionCol(), Qt::red);
		else if ( _candidateIsNewer )
		    setTextColor( versionCol(), Qt::blue);
	    }
	}
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
    QTreeWidgetItem::setText( column, fromUTF8( text.c_str() ) );
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
	yuiError() << "No selectable" << endl;
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
YQPkgObjListItem::setStatus( ZyppStatus newStatus, bool sendSignals )
{
    ZyppStatus oldStatus = selectable()->status();
    selectable()->setStatus( newStatus );

    if ( oldStatus != selectable()->status() )
    {
	applyChanges();

	if ( sendSignals )
	{
	    _pkgObjList->updateItemStates();
	    _pkgObjList->sendUpdatePackages();
	}
    }

    setStatusIcon();
}


void
YQPkgObjListItem::solveResolvableCollections()
{
    zypp::getZYpp()->resolver()->resolvePool();
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
        setIcon( statusCol(), _pkgObjList->statusIcon( status(), enabled, bySelection() ) );
    }


    if ( brokenIconCol() >= 0 )
    {
	// Reset this icon now - it might be the same column as satisfiedIconCol()
        setIcon( brokenIconCol(), QPixmap() );
    }

    if ( satisfiedIconCol() >= 0 )
    {
	// Set special icon for zyppObjs that are not marked as installed,
	// but satisfied anyway (e.g. for patches or patterns where the user
	// selected all required packages manually)

        setIcon( satisfiedIconCol(), isSatisfied() ? YQIconPool::pkgSatisfied() : QPixmap() );
    }

    if ( brokenIconCol() >= 0 )
    {
	// Set special icon for zyppObjs that are installed, but broken
	// (dependencies no longer satisfied, e.g. for patches or patterns)

	if ( isBroken() )
	{
            setIcon( brokenIconCol(), YQIconPool::warningSign() );

	    yuiWarning() << "Broken object: " << _selectable->theObj()->name()
			 << " - " << _selectable->theObj()->summary()
			 << endl;
	}
    }
}


bool
YQPkgObjListItem::isSatisfied() const
{
    if ( _debugIsSatisfied )
	return true;

    if ( _selectable->hasInstalledObj() )
	return false;

    return _selectable->candidateObj().isSatisfied();
}


bool YQPkgObjListItem::isBroken() const
{
    if ( _debugIsBroken )
	return true;

    if ( ! _selectable->hasInstalledObj() )
	return false;		// can't be broken if not installed

    switch ( status() )
    {
	case S_KeepInstalled:
	case S_Protected:

	    return _selectable->installedObj().isBroken();

	case S_Update:		// will be fixed by updating
	case S_AutoUpdate:
	case S_Del:		// will no longer be relevant after deleting
	case S_AutoDel:

	    return false;

	case S_NoInst:		// should not happen - no installed obj
	case S_Install:
	case S_AutoInstall:
	case S_Taboo:

	    yuiError() << "Expected uninstalled zyppObj" << endl;
	    return false;
    }

    yuiError() << "Should never get here" << endl;
    return false;
}


void
YQPkgObjListItem::cycleStatus()
{
    if ( ! _editable || ! _pkgObjList->editable() )
	return;

    ZyppStatus oldStatus = status();
    ZyppStatus newStatus = oldStatus;

    switch ( oldStatus )
    {
	case S_Install:
	    newStatus = S_NoInst;
	    break;

	case S_Protected:
	    newStatus = selectable()->hasCandidateObj() ?
		S_KeepInstalled: S_NoInst;
	    break;

	case S_Taboo:
	    newStatus = selectable()->hasInstalledObj() ?
		S_KeepInstalled : S_NoInst;
	    break;

	case S_KeepInstalled:
	    newStatus = selectable()->hasCandidateObj() ?
		S_Update : S_Del;
	    break;

	case S_Update:
	    newStatus = S_Del;
	    break;

	case S_AutoUpdate:
	    newStatus = S_KeepInstalled;
	    break;

	case S_Del:
	case S_AutoDel:
	    newStatus = S_KeepInstalled;
	    break;

	case S_NoInst:
	    if ( selectable()->hasCandidateObj() )
	    {
		newStatus = S_Install;
	    }
	    else
	    {
		yuiWarning() << "No candidate for " << selectable()->theObj()->name() << endl;
		newStatus = S_NoInst;
	    }
	    break;

	case S_AutoInstall:
	    // this used to be taboo before, but now ZYpp supports
	    // saving weak locks (unselected packages)
	    newStatus =  S_NoInst;
	    break;
    }

    if ( oldStatus != newStatus )
    {
	setStatus( newStatus );

	if ( showLicenseAgreement() )
	{
	    showNotifyTexts( newStatus );
	}
	else // License not confirmed?
	{
	    // Status is now S_Taboo or S_Del - update status icon
	    setStatusIcon();
	}

	_pkgObjList->sendStatusChanged();
    }
}


void
YQPkgObjListItem::showNotifyTexts( ZyppStatus status )
{
    // just return if no selectable
    if ( ! selectable() )
        return;

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
	yuiDebug() << "Showing notify text" << endl;
	YQPkgTextDialog::showText( _pkgObjList, selectable(), text );
    }
}


bool
YQPkgObjListItem::showLicenseAgreement()
{
    return showLicenseAgreement( selectable() );
}


bool
YQPkgObjListItem::showLicenseAgreement( ZyppSel sel )
{
    // if we have a subclass with no selectable
    // confirming the license should be just always
    // true
    if ( ! sel )
        return true;

    string licenseText;

    switch ( sel->status() )
    {
	case S_Install:
	case S_AutoInstall:
	case S_Update:
	case S_AutoUpdate:

	    if ( sel->hasLicenceConfirmed() )
		return true;

	    if ( sel->candidateObj() )
		licenseText = sel->candidateObj()->licenseToConfirm();
	    break;

	default: return true;
    }

    if ( licenseText.empty() )
	return true;

    yuiDebug() << "Showing license agreement for " << sel->name() << endl;

    bool confirmed = YQPkgTextDialog::confirmText( (QWidget *) YDialog::currentDialog()->widgetRep(),
						   sel, licenseText );

    if ( confirmed )
    {
	yuiMilestone() << "User confirmed license agreement for " << sel->name() << endl;
	sel->setLicenceConfirmed( true );
    }
    else
    {
	// The user rejected the license agreement -
	// make sure the package gets unselected.

	switch ( sel->status() )
	{
	    case S_Install:
	    case S_AutoInstall:

		yuiWarning() << "User rejected license agreement for " << sel->name()
			     << " - setting to TABOO"
			     << endl;

		sel->setStatus( S_Taboo );
		break;


	    case S_Update:
	    case S_AutoUpdate:

		yuiWarning() << "User rejected license agreement for " << sel->name()
			     << "  - setting to PROTECTED"
			     << endl;

		sel->setStatus( S_Protected );
		// S_Keep wouldn't be good enough: The next solver run might
		// set it to S_AutoUpdate again
		break;

	    default: break;
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

    if ( col == brokenIconCol() )
    {
	if ( isBroken() )
	    // Translators: tool tip for patches / patterns that are installed,
	    // but whose dependencies are broken (no longer satisfied)
	    return _( "Dependencies broken" );
    }

    // don't use "else if" here, it might be the same colum as another one!

    if ( col == satisfiedIconCol() )
    {
	if ( isSatisfied() )
	    // Translators: tool tip for patches / patterns that are not installed,
	    // but whose dependencies are satisfied
	    return _( "All dependencies satisfied" );
    }

    return QString::null;
}



bool YQPkgObjListItem::operator<( const QTreeWidgetItem & otherListViewItem ) const
{
    const YQPkgObjListItem * other = dynamic_cast<const YQPkgObjListItem *> (&otherListViewItem);
    int col = treeWidget()->sortColumn();

    if ( other )
    {
        if ( col == nameCol() )
	{
	    return ( strcmp( this->zyppObj()->name().c_str(), other->zyppObj()->name().c_str() ) < 0 );
	}
	if ( col == summaryCol() )
	{
	    // locale aware sort
            return ( strcoll( this->zyppObj()->summary().c_str(), other->zyppObj()->summary().c_str() ) < 0 );
	}
	if ( col == sizeCol() )
	{
	    // Numeric sort by size

	    return ( this->zyppObj()->installSize() < other->zyppObj()->installSize() );
	}
	else if ( col == statusCol() )
	{
	    // Sorting by status depends on the numeric value of the
	    // ZyppStatus enum, thus it is important to insert new
	    // package states there where they make most sense. We want to show
	    // dangerous or noteworthy states first - e.g., "taboo" which should
	    // seldeom occur, but when it does, it is important.

	    bool b = ( this->status() < other->status() );
            if ( !b && this->status() == other->status() )
                b = this->zyppObj()->name() < other->zyppObj()->name();
            return b;
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

	    if (thisPoints == otherPoints )
		return ( QString (this->zyppObj()->edition().c_str() ) < 
			 QString (other->zyppObj()->edition().c_str() ) );
	    else 
		return ( thisPoints < otherPoints );
	}
    }

    // Fallback: Use parent class method
    return QY2ListViewItem::operator<( otherListViewItem );
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


void
YQPkgObjListItem::setExcluded( bool excl )
{
    _excluded = excl;
}

YQPkgObjList::ExcludeRule::ExcludeRule( YQPkgObjList *	parent,
					const QRegExp &	regexp,
					int		column )
    : _parent( parent )
    , _regexp( regexp )
    , _column( column )
    , _enabled( true )
{
    _parent->addExcludeRule( this );
}


void
YQPkgObjList::ExcludeRule::enable( bool enable )
{
    _enabled = enable;

#if VERBOSE_EXCLUDE_RULES
    yuiDebug() << ( enable ? "Enabling" : "Disabling" )
	       << " exclude rule " << _regexp.pattern()
	       << endl;
#endif
}


void
YQPkgObjList::ExcludeRule::setRegexp( const QRegExp & regexp )
{
    _regexp = regexp;
}


void
YQPkgObjList::ExcludeRule::setColumn( int column )
{
    _column = column;
}


bool
YQPkgObjList::ExcludeRule::match( QTreeWidgetItem * item )
{
    if ( ! _enabled )
	return false;

    QString text = item->text( _column );

    if ( text.isEmpty() )
	return false;

    return _regexp.exactMatch( text );
}






YQPkgObjList::ExcludedItems::ExcludedItems( YQPkgObjList * parent )
    : _pkgObjList( parent )
{
}


YQPkgObjList::ExcludedItems::~ExcludedItems()
{
    clear();
}


void YQPkgObjList::ExcludedItems::add( QTreeWidgetItem * item, QTreeWidgetItem * oldParent )
{
    _excludeMap.insert( ItemPair( item, oldParent ) );
}


void YQPkgObjList::ExcludedItems::remove( QTreeWidgetItem * item )
{
    ItemMap::iterator it = _excludeMap.find( item );

    if ( it != _excludeMap.end() )
    {
	_excludeMap.erase( it );
    }
}


void YQPkgObjList::ExcludedItems::clear()
{
    for ( ItemMap::iterator it = _excludeMap.begin();
	  it != _excludeMap.end();
	  ++it )
    {
	delete it->first;
    }

    _excludeMap.clear();
}


bool YQPkgObjList::ExcludedItems::contains( QTreeWidgetItem * item )
{
    return ( _excludeMap.find( item ) != _excludeMap.end() );
}


QTreeWidgetItem * YQPkgObjList::ExcludedItems::oldParentItem( QTreeWidgetItem * item )
{
    ItemMap::iterator it = _excludeMap.find( item );

    if ( it == _excludeMap.end() )
	return 0;

    return it->second;
}



#include "YQPkgObjList.moc"
