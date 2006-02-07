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

  File:	      YQPkgList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#define SOURCE_RPM_DISABLED 0

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <qpixmap.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>

#include "YQZypp.h"
#include <y2pm/InstTarget.h>
#include "utf8.h"

#include "YQPkgList.h"
#include "YQUI.h"
#include "YQi18n.h"
#include "YQIconPool.h"


YQPkgList::YQPkgList( QWidget * parent )
    : YQPkgObjList( parent )
{
    _srpmStatusCol	= -42;
    int installedPkgs	= Y2PM::instTarget().numPackages();

    int numCol = 0;
    addColumn( "" );			_statusCol	= numCol++;
    // _statusCol = numCol;
    addColumn( _( "Package" 	) );	_nameCol	= numCol++;

    addColumn( _( "Summary" 	) );	_summaryCol	= numCol++;
    addColumn( _( "Size" 	) );	_sizeCol	= numCol++;

    if ( installedPkgs > 0 )
    {
	addColumn( _( "Avail. Ver." ) ); _versionCol	 = numCol++;
	addColumn( _( "Inst. Ver."  ) ); _instVersionCol = numCol++;
    }
    else
    {
	addColumn( _( "Version"	) );	_versionCol	= numCol++;
	_instVersionCol = -1;
    }

#if SOURCE_RPM_DISABLED
#warning Selecting source RPMs disabled!
#else
    addColumn( _( "Source" ) );		_srpmStatusCol	= numCol++;
#endif

    saveColumnWidths();
    setSorting( nameCol() );
    setColumnAlignment( sizeCol(), Qt::AlignRight );
    setAllColumnsShowFocus( true );

    createActions();
    createSourceRpmContextMenu();
}


YQPkgList::~YQPkgList()
{
    // NOP
}


void YQPkgList::addPkgItem( zypp::Package::constPtr zyppPkg )
{
    addPkgItem( zyppPkg, false );	
}


void YQPkgList::addPkgItemDimmed( zypp::Package::constPtr zyppPkg )
{
    addPkgItem( zyppPkg, true );
}


void
YQPkgList::addPkgItem( zypp::Package::constPtr zyppPkg, bool dimmed )
{
    if ( ! zyppPkg )
    {
	y2error( "Null zypp::Package !" );
	return;
    }

    YQPkgListItem * item = new YQPkgListItem( this, zyppPkg );
    CHECK_PTR( item );

    item->setDimmed( dimmed );
}


void
YQPkgList::pkgObjClicked( int			button,
			  QListViewItem *	listViewItem,
			  int			col,
			  const QPoint &	pos )
{
    if ( col == srpmStatusCol() )
    {
	YQPkgListItem * item = dynamic_cast<YQPkgListItem *> (listViewItem);

	if ( item )
	{
	    if ( button == Qt::LeftButton )
	    {
		if ( editable() && item->editable() )
		    item->toggleSourceRpmStatus();
		return;
	    }
	    else if ( button == Qt::RightButton )
	    {
		if ( editable() && item->editable() )
		{
		    updateActions( item );

		    if ( _sourceRpmContextMenu )
			_sourceRpmContextMenu->popup( pos );
		}

		return;
	    }
	}
    }

    YQPkgObjList::pkgObjClicked( button, listViewItem, col, pos );
}


QSize
YQPkgList::sizeHint() const
{
    return QSize( 600, 350 );
}


void
YQPkgList::createSourceRpmContextMenu()
{
    _sourceRpmContextMenu = new QPopupMenu( this );

    actionInstallSourceRpm->addTo( _sourceRpmContextMenu );
    actionDontInstallSourceRpm->addTo( _sourceRpmContextMenu );

    QPopupMenu * submenu = new QPopupMenu( _sourceRpmContextMenu );
    CHECK_PTR( submenu );
    _sourceRpmContextMenu->insertItem( _( "&All in This List" ), submenu );

    actionInstallListSourceRpms->addTo( submenu );
    actionDontInstallListSourceRpms->addTo( submenu );
}


void
YQPkgList::setInstallCurrentSourceRpm( bool installSourceRpm,
				       bool selectNextItem )
{
    QListViewItem * listViewItem = selectedItem();

    if ( ! listViewItem )
	return;

    YQPkgListItem * item = dynamic_cast<YQPkgListItem *> (listViewItem);

    if ( item )
    {
	item->setInstallSourceRpm( installSourceRpm );

	if ( selectNextItem && item->nextSibling() )
	{
	    item->setSelected( false );			// doesn't emit signals
	    setSelected( item->nextSibling(), true );	// emits signals
	}
    }
}


void
YQPkgList::setInstallListSourceRpms( bool installSourceRpm )
{
    if ( ! _editable )
	return;

    QListViewItem * listViewItem = firstChild();

    while ( listViewItem )
    {
	YQPkgListItem * item = dynamic_cast<YQPkgListItem *> (listViewItem);

	if ( item && item->editable() )
	{
	    item->setInstallSourceRpm( installSourceRpm );
	}

	listViewItem = listViewItem->nextSibling();
    }
}



void
YQPkgList::createNotInstalledContextMenu()
{
    _notInstalledContextMenu = new QPopupMenu( this );
    CHECK_PTR( _notInstalledContextMenu );

    actionSetCurrentInstall->addTo( _notInstalledContextMenu );
    actionSetCurrentDontInstall->addTo( _notInstalledContextMenu );
    actionSetCurrentTaboo->addTo( _notInstalledContextMenu );

    addAllInListSubMenu( _notInstalledContextMenu );


    _notInstalledContextMenu->insertSeparator();
    _notInstalledContextMenu->insertItem( _( "Export This List to &Text File..." ),
					  this, SLOT( askExportList() ) );
}


void
YQPkgList::createInstalledContextMenu()
{
    _installedContextMenu = new QPopupMenu( this );
    CHECK_PTR( _installedContextMenu );

    actionSetCurrentKeepInstalled->addTo( _installedContextMenu );
    actionSetCurrentDelete->addTo( _installedContextMenu );
    actionSetCurrentUpdate->addTo( _installedContextMenu );
    actionSetCurrentProtected->addTo( _installedContextMenu );

    addAllInListSubMenu( _installedContextMenu );

    _installedContextMenu->insertSeparator();
    _installedContextMenu->insertItem( _( "Export This List to &Text File..." ),
				       this, SLOT( askExportList() ) );
}


QPopupMenu *
YQPkgList::addAllInListSubMenu( QPopupMenu * menu )
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
    actionSetListProtected->addTo( submenu );

    menu->insertItem( _( "&All in This List" ), submenu );

    return submenu;
}


void
YQPkgList::createActions()
{
    actionInstallSourceRpm		= createAction( _( "&Install Source" ),
							statusIcon( S_Install, true ),
							statusIcon( S_Install, false ) );

    actionDontInstallSourceRpm		= createAction( _( "Do &Not Install Source" ),
							statusIcon( S_NoInst, true ),
							statusIcon( S_NoInst, false ) );

    actionInstallListSourceRpms		= createAction( _( "&Install All Available Sources" ),
							statusIcon( S_Install, true ),
							statusIcon( S_Install, false ),
							QString::null,		// key
							true );			// enabled

    actionDontInstallListSourceRpms	= createAction( _( "Do &Not Install Any Sources" ),
							statusIcon( S_NoInst, true ),
							statusIcon( S_NoInst, false ),
							QString::null,		// key
							true );			// enabled

    connect( actionInstallSourceRpm,		SIGNAL( activated() ), this, SLOT( setInstallCurrentSourceRpm()	    ) );
    connect( actionDontInstallSourceRpm,	SIGNAL( activated() ), this, SLOT( setDontInstallCurrentSourceRpm() ) );

    connect( actionInstallListSourceRpms,	SIGNAL( activated() ), this, SLOT( setInstallListSourceRpms()	    ) );
    connect( actionDontInstallListSourceRpms,	SIGNAL( activated() ), this, SLOT( setDontInstallListSourceRpms()   ) );
}


void
YQPkgList::updateActions( YQPkgObjListItem * pkgObjListItem )
{
    YQPkgObjList::updateActions( pkgObjListItem );

    YQPkgListItem * item = dynamic_cast<YQPkgListItem *> (pkgObjListItem);

    if ( item )
    {
	actionInstallSourceRpm->setEnabled( item->hasSourceRpm() );
	actionDontInstallSourceRpm->setEnabled( item->hasSourceRpm() );
    }
    else
    {
	actionInstallSourceRpm->setEnabled( false );
	actionDontInstallSourceRpm->setEnabled( false );
    }
}


void
YQPkgList::askExportList() const
{
    QString filename = YQUI::ui()->askForSaveFileName( "pkglist.txt",	// startsWith
							   "*.txt",		// filter
							   _( "Export Package List" ) );
    if ( ! filename.isEmpty() )
	exportList( filename, true );
}


void
YQPkgList::exportList( const QString filename, bool interactive ) const
{
    // Open file

    FILE * file = fopen( (const char *) filename, "w" );

    if ( ! file )
    {
	y2error( "Can't open file %s", (const char *) filename );

	if ( interactive )
	{
	    // Post error popup.

	    QMessageBox::warning( 0,						// parent
				  _( "Error" ),					// caption
				  _( "Cannot open file %1" ).arg( filename ),
				  QMessageBox::Ok | QMessageBox::Default,	// button0
				  QMessageBox::NoButton,			// button1
				  QMessageBox::NoButton );			// button2
	}
	return;
    }


    // Write all items

    const QListViewItem * item = firstChild();

    while ( item )
    {
	const YQPkgListItem * pkg = dynamic_cast<const YQPkgListItem *> (item);

	if ( pkg )
	{
	    QString status = "[" + statusText( pkg->status() ) + "]";
	    fprintf( file, "%-16s %-30s | %-16s | %10s\n",
		     (const char *) status,
		     (const char *) pkg->text( nameCol()    ),
		     (const char *) pkg->text( versionCol() ),
		     (const char *) pkg->text( sizeCol()    )
		     );
	}

	item = item->nextSibling();
    }


    // Clean up

    if ( file )
	fclose( file );
}






YQPkgListItem::YQPkgListItem( YQPkgList * pkgList, zypp::Package::constPtr zyppPkg )
    : YQPkgObjListItem( pkgList, zyppPkg )
    , _pkgList( pkgList )
    , _zyppPkg( zyppPkg )
    , _dimmed( false )
{
    CHECK_PTR( zyppPkg );
    CHECK_PTR( pkgList );

    setSourceRpmIcon();
}


YQPkgListItem::~YQPkgListItem()
{
    // NOP
}


void
YQPkgListItem::updateData()
{
    YQPkgObjListItem::updateData();
    setSourceRpmIcon();
}


bool
YQPkgListItem::hasSourceRpm() const
{
    if ( ! _selectable )
	return false;

    return _selectable->providesSources();
}


bool
YQPkgListItem::installSourceRpm() const
{
    if ( ! _selectable )
	return false;

    if ( ! _selectable->providesSources() )
	return false;

    return _selectable->source_install();
}


void
YQPkgListItem::setSourceRpmIcon()
{
    if ( srpmStatusCol() < 0 )
	return;

    QPixmap icon;

    if ( hasSourceRpm() )
    {

	if ( editable() && _pkgObjList->editable() )
	{
	    icon = installSourceRpm() ?
		YQIconPool::pkgInstall() :
		YQIconPool::pkgNoInst();
	}
	else
	{
	    icon = installSourceRpm() ?
		YQIconPool::disabledPkgInstall() :
		YQIconPool::disabledPkgNoInst();
	}
    }

    setPixmap( srpmStatusCol(), icon );
}


void
YQPkgListItem::setInstallSourceRpm( bool installSourceRpm )
{
    if ( hasSourceRpm() )
    {
	if ( _selectable )
	    _selectable->set_source_install( installSourceRpm );
    }

    setSourceRpmIcon();
}


void
YQPkgListItem::toggleSourceRpmStatus()
{
    setInstallSourceRpm( ! installSourceRpm() );
}


QString
YQPkgListItem::toolTip( int col )
{
    QString text;
    QString name = _zyppObj->name().asString().c_str();

    if ( col == statusCol() )
    {
	text = YQPkgObjListItem::toolTip( col );
    }
    else if ( col == srpmStatusCol() )
    {
	text = name + "\n\n";

	if ( hasSourceRpm() )
	{
	    text += installSourceRpm() ?
		_( "Install Sources" ) :
		_( "Do Not Install Sources" );
	}
	else
	{
	    text += _( "No Sources Available" );
	}
    }
    else
    {
	text = name + "\n\n";

	QString installed;
	QString candidate;

	if ( _zyppObj->hasInstalledObj() )
	{
	    installed  = _zyppObj->getInstalledObj()->edition().asString().c_str();
	    installed += "-";
	    installed +=  _zyppObj->getInstalledObj()->arch().asString().c_str();
	    installed  = _( "Installed Version: %1" ).arg( installed );
	}

	if (  _zyppObj->hasCandidateObj() )
	{
	    candidate  = _zyppObj->getCandidateObj()->edition().asString().c_str();
	    candidate += "-";
	    candidate +=  _zyppObj->getCandidateObj()->arch().asString().c_str();
	}

	if ( _zyppObj->hasInstalledObj() )
	{
	    text += installed + "\n";

	    if ( _zyppObj->hasCandidateObj() )
	    {
		// Translators: This is the relation between two versions of one package
		// if both versions are the same, e.g., both "1.2.3-42", "1.2.3-42"
		QString relation = _( "same" );

		if ( _candidateIsNewer )	relation = _( "newer" );
		if ( _installedIsNewer )	relation = _( "older" );

		// Translators: %1 is the version, %2 is one of "newer", "older", "same"
		text += _( "Available Version: %1 (%2)" ).arg( candidate ).arg( relation );
	    }
	    else
	    {
		text += _( "Not available for installation" );
	    }
	}
	else // not installed
	{
	    text += candidate;
	}
    }

    return text;
}



/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
YQPkgListItem::compare( QListViewItem *		otherListViewItem,
			int			col,
			bool			ascending ) const
{
    if ( col == srpmStatusCol() )
    {
	YQPkgListItem * other = dynamic_cast<YQPkgListItem *> (otherListViewItem);

	if ( other )
	{
	    int thisPoints  = ( this->hasSourceRpm()  ? 1 : 0 ) + ( this->installSourceRpm()  ? 1 : 0 );
	    int otherPoints = ( other->hasSourceRpm() ? 1 : 0 ) + ( other->installSourceRpm() ? 1 : 0 );

	    // Intentionally inverting order: Pkgs with source RPMs are more interesting than without.
	    if ( thisPoints > otherPoints ) return -1;
	    if ( thisPoints < otherPoints ) return  1;
	    return 0;
	}
    }

    // Fallback: Use parent class method
    return YQPkgObjListItem::compare( otherListViewItem, col, ascending );
}


void
YQPkgListItem::paintCell( QPainter *		painter,
			  const QColorGroup &	colorGroup,
			  int			column,
			  int			width,
			  int			alignment )
{
    if ( isDimmed() && ! YQUI::ui()->usingVisionImpairedPalette() )
    {
	QColorGroup cg = colorGroup;
	cg.setColor( QColorGroup::Text, QColor( 0xA0, 0xA0, 0xA0 ) );
	
	QListViewItem::paintCell( painter, cg, column, width, alignment );
    }
    else
    {
	if ( installedIsNewer() )
	{
	    QColorGroup cg = colorGroup;

	    if ( ! YQUI::ui()->usingVisionImpairedPalette() )
	    {
		if ( column == instVersionCol() )
		    cg.setColor( QColorGroup::Base, QColor( 0xFF, 0x30, 0x30 ) );	// Background
		else
		    cg.setColor( QColorGroup::Text, QColor( 0xFF, 0, 0 ) );		// Foreground
	    }

	    QListViewItem::paintCell( painter, cg, column, width, alignment );
	}
	else if ( candidateIsNewer() )
	{
	    QColorGroup cg = colorGroup;

	    if ( ! YQUI::ui()->usingVisionImpairedPalette() )
	    {
		cg.setColor( QColorGroup::Text, QColor( 0, 0, 0xC0 ) );		// Foreground

		if ( column == versionCol() )
		    cg.setColor( QColorGroup::Base, QColor( 0xF0, 0xF0, 0xF0 ) );	// Background
	    }

	    QListViewItem::paintCell( painter, cg, column, width, alignment );
	}
	else
	{
	    QListViewItem::paintCell( painter, colorGroup, column, width, alignment );
	}
    }
}



#include "YQPkgList.moc"
