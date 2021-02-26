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

  Textdomain "qt-pkg"

/-*/



#include <algorithm>
using std::max;

#define YUILogComponent "qt-pkg"
#include "YUILog.h"
#include <QPixmap>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QFile>
#include <QTreeView>
#include <QDebug>
#include <QHeaderView>

#include "utf8.h"

#include "YQPkgList.h"
#include "YQUI.h"
#include "YQi18n.h"
#include "YQIconPool.h"
#include "YQApplication.h"

#define SINGLE_VERSION_COL	1


YQPkgList::YQPkgList( QWidget * parent )
    : YQPkgObjList( parent )
{
    _srpmStatusCol	= -42;

    int numCol = 0;
    QStringList headers;
    QString 	versionHeaderText;
    QString 	instVersionHeaderText;


    headers <<  "";			_statusCol	= numCol++;
    headers <<  _( "Package" 	);	_nameCol	= numCol++;
    headers <<  _( "Summary" 	);	_summaryCol	= numCol++;

    if ( haveInstalledPkgs() )
    {
#if SINGLE_VERSION_COL
        versionHeaderText = _("Installed (Available)");
        headers << versionHeaderText;	_instVersionCol = numCol++;
	_versionCol = _instVersionCol;
#else
	versionHeaderText = _( "Avail. Ver." );
	headers << versionHeaderText;	_versionCol	= numCol++;

	instVersionHeaderText = _( "Inst. Ver."  );
	headers << instVersionHeaderText;  _instVersionCol = numCol++;
#endif
    }
    else
    {
	versionHeaderText = _( "Version" );
	headers << versionHeaderText;	_versionCol	= numCol++;
    }

    // headers <<  _( "Source" );	_srpmStatusCol	= numCol++;
    headers <<  _( "Size" 	);	_sizeCol	= numCol++;

    setHeaderLabels( headers );

    setSortingEnabled( true );
    header()->setSortIndicatorShown( true );
    header()->setClickable( true );

    sortByColumn( nameCol(), Qt::AscendingOrder );
    setAllColumnsShowFocus( true );
    setIconSize( QSize( 22, 16 ) );

    header()->setResizeMode( QHeaderView::Interactive );
    header()->setResizeMode( statusCol(), QHeaderView::ResizeToContents	);

    QFontMetrics fms( header()->font() );

    setColumnWidth( sizeCol(),		fms.width( " 9999.9 K  "	) );
    setColumnWidth( versionCol(),	fms.width( versionHeaderText + "   " ) );
    setColumnWidth( nameCol(),		fms.width( "desktop-data-openSUSE-extraroom" ) );
    setColumnWidth( summaryCol(),	fms.width( "A really really long text, but not too long" ) );

    if ( instVersionCol() != versionCol() )
    {
	setColumnWidth( versionCol(),     max( fms.width( versionHeaderText + "  " ),
					       fms.width( " 20071220pre"           ) ) );
	setColumnWidth( instVersionCol(), max( fms.width( instVersionHeaderText + "  " ),
					       fms.width( " 20071220pre"	       ) ) );
    }

#if 0
    header()->setResizeMode( nameCol(),		QHeaderView::Stretch	);
    header()->setResizeMode( summaryCol(),	QHeaderView::Stretch	);
    header()->setResizeMode( versionCol(),	QHeaderView::Stretch	);
#endif

    header()->setResizeMode( sizeCol(),		QHeaderView::Fixed );

    saveColumnWidths();
    createActions();
    createSourceRpmContextMenu();
}


YQPkgList::~YQPkgList()
{
    // NOP
}


void YQPkgList::addPkgItem( ZyppSel selectable,
			    ZyppPkg zyppPkg )
{
    addPkgItem( selectable, zyppPkg, false );
}


void YQPkgList::addPkgItemDimmed( ZyppSel selectable,
				  ZyppPkg zyppPkg )
{
    addPkgItem( selectable, zyppPkg, true );
}


void
YQPkgList::addPkgItem( ZyppSel	selectable,
		       ZyppPkg 	zyppPkg,
		       bool 	dimmed )
{
    scheduleDelayedItemsLayout();
    
    if ( ! selectable )
    {
	yuiError() << "NULL zypp::ui::Selectable!" << endl;
	return;
    }

    YQPkgListItem * item = new YQPkgListItem( this, selectable, zyppPkg );
    Q_CHECK_PTR( item );

    item->setDimmed( dimmed );
    applyExcludeRules( item );
}


bool
YQPkgList::haveInstalledPkgs()
{
    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	if ( (*it)->installedObj() )
	    return true;
    }

    return false;
}


void
YQPkgList::pkgObjClicked( int			button,
			  QTreeWidgetItem *	listViewItem,
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
    _sourceRpmContextMenu = new QMenu( this );

    _sourceRpmContextMenu->addAction(actionInstallSourceRpm);
    _sourceRpmContextMenu->addAction(actionDontInstallSourceRpm);

    QMenu * submenu = new QMenu( _sourceRpmContextMenu );
    Q_CHECK_PTR( submenu );
    QAction *action = _sourceRpmContextMenu->addMenu( submenu );
    action->setText(_( "&All in This List" ));

    submenu->addAction(actionInstallListSourceRpms);
    submenu->addAction(actionDontInstallListSourceRpms);
}


void
YQPkgList::setInstallCurrentSourceRpm( bool installSourceRpm,
				       bool selectNextItem )
{
#if FIXME
    QTreeWidgetItem * listViewItem = selectedItem();

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
#endif
}


void
YQPkgList::setInstallListSourceRpms( bool installSourceRpm )
{
    if ( ! _editable )
	return;

#if FIXME
    QTreeWidgetItem * listViewItem = firstChild();

    while ( listViewItem )
    {
	YQPkgListItem * item = dynamic_cast<YQPkgListItem *> (listViewItem);

	if ( item && item->editable() )
	{
	    item->setInstallSourceRpm( installSourceRpm );
	}

	listViewItem = listViewItem->nextSibling();
    }
#endif
}



void
YQPkgList::createNotInstalledContextMenu()
{
    _notInstalledContextMenu = new QMenu( this );
    Q_CHECK_PTR( _notInstalledContextMenu );

    _notInstalledContextMenu->addAction(actionSetCurrentInstall);
    _notInstalledContextMenu->addAction(actionSetCurrentDontInstall);
    _notInstalledContextMenu->addAction(actionSetCurrentTaboo);
    _notInstalledContextMenu->addAction(actionShowCurrentSolverInfo);        

    addAllInListSubMenu( _notInstalledContextMenu );

    _notInstalledContextMenu->addSeparator();
    _notInstalledContextMenu->addAction( _( "Export This List to &Text File..." ),
					  this, SLOT( askExportList() ) );
}


void
YQPkgList::createInstalledContextMenu()
{
    _installedContextMenu = new QMenu( this );
    Q_CHECK_PTR( _installedContextMenu );

    _installedContextMenu->addAction(actionSetCurrentKeepInstalled);
    _installedContextMenu->addAction(actionSetCurrentDelete);
    _installedContextMenu->addAction(actionSetCurrentUpdate);
    _installedContextMenu->addAction(actionSetCurrentProtected);
    _installedContextMenu->addAction(actionShowCurrentSolverInfo);            

    addAllInListSubMenu( _installedContextMenu );

    _installedContextMenu->addSeparator();
    _installedContextMenu->addAction( _( "Export This List to &Text File..." ),
				       this, SLOT( askExportList() ) );
}


QMenu *
YQPkgList::addAllInListSubMenu( QMenu * menu )
{
    QMenu * submenu = new QMenu( menu );
    Q_CHECK_PTR( submenu );

    submenu->addAction(actionSetListInstall);
    submenu->addAction(actionSetListDontInstall);
    submenu->addAction(actionSetListKeepInstalled);
    submenu->addAction(actionSetListDelete);
    submenu->addAction(actionSetListDelete);
    submenu->addAction(actionSetListUpdate);
    submenu->addAction(actionSetListUpdateForce);
    submenu->addAction(actionSetListTaboo);
    submenu->addAction(actionSetListProtected);

    QAction *action = menu->addMenu( submenu );
    action->setText(_( "&All in This List" ));

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
    QString filename = YQApplication::askForSaveFileName( "pkglist.txt",	// startsWith
							  "*.txt",		// filter
							  _( "Export Package List" ) );
    if ( ! filename.isEmpty() )
	exportList( filename, true );
}


void
YQPkgList::exportList( const QString filename, bool interactive ) const
{
    // Open file

    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    if ( file.error() != QFile::NoError )
    {
	yuiError() << "Can't open file " << filename << endl;

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


    //
    // Write header
    //

    // Format the header line with QString::sprintf() because plain stdio
    // fprintf() is not UTF-8 aware - it will count multi-byte characters
    // wrong, so the formatting will be broken.

    QString header;
    header.sprintf( "# %-18s %-30s | %-40s | %-25s | %10s\n\n",
		    (const char *) _( "Status"      ).toUtf8(),
		    (const char *) _( "Package"     ).toUtf8(),
		    (const char *) _( "Summary"     ).toUtf8(),
		    (const char *) _( "Installed (Available)" ).toUtf8(),
		    (const char *) _( "Size"        ).toUtf8()
		    );
    file.write(header.toUtf8());


    // Write all items

    QTreeWidgetItemIterator it((QTreeWidget*) this);
    
    while (*it)
    {
        const QTreeWidgetItem* item(*it);
        const YQPkgListItem *  pkg = dynamic_cast<const YQPkgListItem *> (item);

        if ( pkg )
        {
            QString version = pkg->text( versionCol() );
            if ( version.isEmpty() ) version = "---";

            QString summary = pkg->text( summaryCol() );
            if ( summary.isEmpty() ) summary = "---";
            if ( summary.size() > 40 )
            {
                summary.truncate(40-3);
                summary += "...";
            }

            QString status = "[" + statusText( pkg->status() ) + "]";
            QString format;
	    format.sprintf("%-20s %-30s | %-40s | %-25s | %10s\n",
		     (const char*) status.toUtf8(),
		     (const char*) pkg->text( nameCol() ).toUtf8(),
		     (const char*) summary.toUtf8(),
		     (const char*) version.toUtf8(),
		     (const char*) pkg->text( sizeCol() ).toUtf8() 
		     );
            file.write(format.toUtf8());
	}
        ++it;
    }

    // Clean up

    if ( file.isOpen() )
        file.close();
}


int
YQPkgList::globalSetPkgStatus( ZyppStatus newStatus, bool force, bool countOnly )
{
    YQUI::ui()->busyCursor();
    int changedCount = 0;

    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	ZyppSel	   selectable = *it;
	ZyppStatus oldStatus  = selectable->status();

	if ( newStatus != oldStatus )
	{
	    bool doChange = false;

	    switch ( newStatus )
	    {
		case S_KeepInstalled:
		case S_Del:
		case S_AutoDel:
		case S_Protected:
		    doChange = !selectable->installedEmpty();
		    break;

		case S_Update:
		case S_AutoUpdate:

		    if ( force )
		    {
			doChange = !selectable->installedEmpty();
		    }
		    else // don't force - update only if useful (if candidate is newer)
		    {
			const ZyppObj candidate = selectable->candidateObj();
			const ZyppObj installed = selectable->installedObj();

			if ( candidate && installed )
			{
			    doChange = ( installed->edition() < candidate->edition() );
			}
		    }
		    break;

		case S_Install:
		case S_AutoInstall:
		case S_NoInst:
		case S_Taboo:
		    doChange = selectable->installedEmpty();
		    break;
	    }

	    if ( doChange )
	    {
		if ( ! countOnly )
		    selectable->setStatus( newStatus );

		changedCount++;
		// yuiMilestone() << "Updating " << selectable->name() << endl;
	    }
	}
    }

    if ( changedCount > 0 && ! countOnly )
    {
	emit updateItemStates();
	emit updatePackages();
	emit statusChanged();
    }

    YQUI::ui()->normalCursor();

    return changedCount;
}






YQPkgListItem::YQPkgListItem( YQPkgList * 		pkgList,
			      ZyppSel	selectable,
			      ZyppPkg 	zyppPkg )
    : YQPkgObjListItem( pkgList, selectable, zyppPkg )
    , _pkgList( pkgList )
    , _zyppPkg( zyppPkg )
    , _dimmed( false )
{
    if ( ! _zyppPkg )
	_zyppPkg = tryCastToZyppPkg( selectable->theObj() );

    setSourceRpmIcon();
    
    setTextAlignment( sizeCol(), Qt::AlignRight );
    setSizeHint( sizeCol(), QSize( QFontMetrics( pkgList->font() ).width( text( sizeCol() ) ), 10 ) );
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
    if ( ! selectable() )
	return false;

#ifdef FIXME
    return selectable()->providesSources();
#else
    return false;
#endif
}


bool
YQPkgListItem::installSourceRpm() const
{
    if ( ! selectable() )
	return false;

#ifdef FIXME
    if ( ! selectable()->providesSources() )
	return false;

    return selectable()->source_install();
#else
    return false;
#endif
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
    setData( srpmStatusCol(), Qt::DecorationRole, icon );
}


void
YQPkgListItem::setInstallSourceRpm( bool installSourceRpm )
{
    if ( hasSourceRpm() )
    {
#ifdef FIXME
	if ( selectable() )
	    selectable()->set_source_install( installSourceRpm );
#endif
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
    QString name = _zyppObj->name().c_str();

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

	if ( !selectable()->installedEmpty() )
	{
	    installed  = selectable()->installedObj()->edition().asString().c_str();
	    installed += "-";
	    installed +=  selectable()->installedObj()->arch().asString().c_str();
	    installed  = _( "Installed Version: %1" ).arg( installed );
	}

	if (  selectable()->hasCandidateObj() )
	{
	    candidate  = selectable()->candidateObj()->edition().asString().c_str();
	    candidate += "-";
	    candidate +=  selectable()->candidateObj()->arch().asString().c_str();
	}

	if ( !selectable()->installedEmpty() )
	{
	    text += installed + "\n";

	    if ( selectable()->hasCandidateObj() )
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



bool YQPkgListItem::operator< ( const QTreeWidgetItem & otherListViewItem ) const
{
    const YQPkgListItem * other = dynamic_cast<const YQPkgListItem *> (&otherListViewItem);

    int col = treeWidget()->sortColumn();
    if ( col == srpmStatusCol() )
    {
	if ( other )
	{
	    int thisPoints  = ( this->hasSourceRpm()  ? 1 : 0 ) + ( this->installSourceRpm()  ? 1 : 0 );
	    int otherPoints = ( other->hasSourceRpm() ? 1 : 0 ) + ( other->installSourceRpm() ? 1 : 0 );

	    // Intentionally inverting order: Pkgs with source RPMs are more interesting than without.
	    return ( thisPoints < otherPoints );
	}
    }

    // Fallback: Use parent class method
    return YQPkgObjListItem::operator<( otherListViewItem );
}

#if 0
void
YQPkgListItem::paintCell( QPainter *		painter,
			  const QColorGroup &	colorGroup,
			  int			column,
			  int			width,
			  int			alignment )
{
#if FIXME
    if ( isDimmed() && ! YQUI::ui()->usingVisionImpairedPalette() )
    {
	QColorGroup cg = colorGroup;
	cg.setColor( QColorGroup::Text, QColor( 0xA0, 0xA0, 0xA0 ) );

	QTreeWidgetItem::paintCell( painter, cg, column, width, alignment );
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

	    QTreeWidgetItem::paintCell( painter, cg, column, width, alignment );
	}
	else if ( candidateIsNewer() )
	{
	    QColorGroup cg = colorGroup;

	    if ( ! YQUI::ui()->usingVisionImpairedPalette() )
	    {
		cg.setColor( QColorGroup::Text, QColor( 0, 0, 0xC0 ) );			// Foreground

		if ( column == versionCol() )
		    cg.setColor( QColorGroup::Base, QColor( 0xF0, 0xF0, 0xF0 ) );	// Background
	    }

	    QTreeWidgetItem::paintCell( painter, cg, column, width, alignment );
	}
	else
	{
	    QTreeWidgetItem::paintCell( painter, colorGroup, column, width, alignment );
	}
    }
#endif
}
#endif


#include "YQPkgList.moc"
