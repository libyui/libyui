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

#include <Y2PM.h>
#include <y2pm/InstTarget.h>
#include "utf8.h"

#include "YQPkgList.h"
#include "YUIQt.h"
#include "YQi18n.h"
#include "YQIconPool.h"


YQPkgList::YQPkgList( QWidget *parent )
    : YQPkgObjList( parent )
{
    _srpmStatusCol	= -42;
    int installedPkgs	= Y2PM::instTarget().numPackages();

    int numCol = 0;
    addColumn( ""		);	_statusCol	= numCol++;
    // _statusCol = numCol;
    addColumn( _( "Package" ) );	_nameCol	= numCol++;

    addColumn( _( "Summary"	) );	_summaryCol	= numCol++;
    addColumn( _( "Size"	) );	_sizeCol	= numCol++;

    if ( installedPkgs > 0 )
    {
	addColumn( _( "Avail. Ver." ) ); _versionCol	= numCol++;
	addColumn( _( "Inst. Ver."  ) ); _instVersionCol = numCol++;
    }
    else
    {
	addColumn( _( "Version" ) );	_versionCol	= numCol++;
	_instVersionCol = -1;
    }

#if SOURCE_RPM_DISABLED
#warning Selecting source RPMs disabled!
#else
    addColumn( _( "Source"	) );	_srpmStatusCol	= numCol++;
#endif

    saveColumnWidths();
    setSorting( nameCol() );
    setColumnAlignment( sizeCol(), Qt::AlignRight );
    setAllColumnsShowFocus( true );

    createSourceRpmContextMenu();
}


YQPkgList::~YQPkgList()
{
    // NOP
}


void
YQPkgList::addPkgItem( PMPackagePtr pmPkg )
{
    if ( ! pmPkg )
    {
	y2error( "Null PMPackage !" );
	return;
    }

    new YQPkgListItem( this, pmPkg );
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
		    actionInstallSourceRpm->setEnabled( item->hasSourceRpm() );
		    actionDontInstallSourceRpm->setEnabled( item->hasSourceRpm() );

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
    actionInstallSourceRpm		= createAction( _( "&Install Source" ),
							statusIcon( PMSelectable::S_Install, true ),
							statusIcon( PMSelectable::S_Install, false ) );

    actionDontInstallSourceRpm		= createAction( _( "Do&n't Install Source" ),
							statusIcon( PMSelectable::S_NoInst, true ),
							statusIcon( PMSelectable::S_NoInst, false ) );

    actionInstallListSourceRpms		= createAction( _( "&Install All Available Sources" ),
							statusIcon( PMSelectable::S_Install, true ),
							statusIcon( PMSelectable::S_Install, false ),
							QString::null,		// key
							true );			// enabled

    actionDontInstallListSourceRpms	= createAction( _( "Do&n't Install Any Sources" ),
							statusIcon( PMSelectable::S_NoInst, true ),
							statusIcon( PMSelectable::S_NoInst, false ),
							QString::null,		// key
							true );			// enabled

    connect( actionInstallSourceRpm,		SIGNAL( activated() ), this, SLOT( setInstallCurrentSourceRpm()	    ) );
    connect( actionDontInstallSourceRpm,	SIGNAL( activated() ), this, SLOT( setDontInstallCurrentSourceRpm() ) );

    connect( actionInstallListSourceRpms,	SIGNAL( activated() ), this, SLOT( setInstallListSourceRpms()	    ) );
    connect( actionDontInstallListSourceRpms,	SIGNAL( activated() ), this, SLOT( setDontInstallListSourceRpms()   ) );


    _sourceRpmContextMenu = new QPopupMenu( this );

    actionInstallSourceRpm->addTo( _sourceRpmContextMenu );
    actionDontInstallSourceRpm->addTo( _sourceRpmContextMenu );

    QPopupMenu * submenu = new QPopupMenu( _sourceRpmContextMenu );
    CHECK_PTR( submenu );
    _sourceRpmContextMenu->insertItem( _( "&All in this list" ), submenu );

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
YQPkgList::askExportList() const
{
    QString filename = YUIQt::yuiqt()->askForSaveFileName( "pkglist.txt",	// startsWith
							   "*.txt",		// filter
							   _( "Export package list" ) );
    if ( ! filename.isEmpty() )
	exportList( filename, true );
}


void
YQPkgList::exportList(  const QString filename, bool interactive ) const
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
				  _( "Can't open file %1" ).arg( filename ),
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
	const YQPkgListItem * pkg = dynamic_cast<const YQPkgListItem *> ( item );

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






YQPkgListItem::YQPkgListItem( YQPkgList * pkgList, PMPackagePtr pmPkg )
    : YQPkgObjListItem( pkgList, pmPkg )
    , _pkgList( pkgList )
    , _pmPkg( pmPkg )
{
    CHECK_PTR( pmPkg );
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
    PMSelectablePtr sel = _pmPkg->getSelectable();

    if ( ! sel )
	return false;

    return sel->providesSources();
}


bool
YQPkgListItem::installSourceRpm() const
{
    PMSelectablePtr sel = _pmPkg->getSelectable();

    if ( ! sel )
	return false;

    if ( ! sel->providesSources() )
	return false;

    return sel->source_install();
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
	PMSelectablePtr sel = _pmPkg->getSelectable();

	if ( sel )
	    sel->set_source_install( installSourceRpm );
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
    QString name = _pmObj->name().asString().c_str();

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
		_( "Install sources" ) :
		_( "Don't install sources" );
	}
	else
	{
	    text += _( "No sources available" );
	}
    }
    else
    {
	text = name + "\n\n";

	QString installed;
	QString candidate;

	if ( _pmObj->hasInstalledObj() )
	{
	    installed  = _pmObj->getInstalledObj()->edition().asString().c_str();
	    installed += "-";
	    installed +=  _pmObj->getInstalledObj()->arch().asString().c_str();
	    installed  = _( "Installed version: %1" ).arg( installed );
	}

	if (  _pmObj->hasCandidateObj() )
	{
	    candidate  = _pmObj->getCandidateObj()->edition().asString().c_str();
	    candidate += "-";
	    candidate +=  _pmObj->getCandidateObj()->arch().asString().c_str();
	}

	if ( _pmObj->hasInstalledObj() )
	{
	    text += installed + "\n";

	    if ( _pmObj->hasCandidateObj() )
	    {
		// Translators: This is the relation between two versions of one package
		// if both versions are the same, e.g., both "1.2.3-42", "1.2.3-42"
		QString relation = _( "same" );

		if ( _candidateIsNewer )	relation = _( "newer" );
		if ( _installedIsNewer )	relation = _( "older" );

		// Translators: %1 is the version, %2 is one of "newer", "older", "same"
		text += _( "Available version: %1 (%2)" ).arg( candidate ).arg( relation );
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
    if ( installedIsNewer() )
    {
	QColorGroup cg = colorGroup;


	if ( column == instVersionCol() )
	    cg.setColor( QColorGroup::Base, QColor( 0xFF, 0x30, 0x30 ) );	// Background
	else
	    cg.setColor( QColorGroup::Text, QColor( 0xFF, 0, 0 ) );		// Foreground

	QListViewItem::paintCell( painter, cg, column, width, alignment );
    }
    else if ( candidateIsNewer() )
    {
	QColorGroup cg = colorGroup;

	cg.setColor( QColorGroup::Text, QColor( 0, 0, 0xC0 ) );			// Foreground

	if ( column == versionCol() )
	    cg.setColor( QColorGroup::Base, QColor( 0xF0, 0xF0, 0xF0 ) );	// Background

	QListViewItem::paintCell( painter, cg, column, width, alignment );
    }
    else
    {
	QListViewItem::paintCell( painter, colorGroup, column, width, alignment );
    }
}



#include "YQPkgList.moc.cc"
