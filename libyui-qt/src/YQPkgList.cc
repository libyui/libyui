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

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <qpixmap.h>
#include <qheader.h>

#include <Y2PM.h>
#include <y2pm/InstTarget.h>
#include "utf8.h"

#include "YQPkgList.h"
#include "YUIQt.h"
#include "YQi18n.h"
#include "YQIconPool.h"


YQPkgList::YQPkgList( YUIQt *yuiqt, QWidget *parent )
    : QListView( parent )
    , yuiqt(yuiqt)
    , mousePressedItem( 0 )
    , mousePressedCol( -1 )
    , mousePressedButton( NoButton )
{
    int installedPkgs = Y2PM::instTarget().numPackages();

    int numCol = 0;
    addColumn( ""		);	_statusCol	= numCol++;
    // _statusCol = numCol;
    addColumn( _( "Name"	) );	_nameCol	= numCol++;

    if ( installedPkgs > 0 )
    {
	addColumn( _( "Avail. Ver." ) ); _versionCol	= numCol++;
	addColumn( _( "Inst. Ver."  ) ); _instVersionCol = numCol++;
    }
    else
    {
	addColumn( _( "Version"	) );	_versionCol	= numCol++;
	_instVersionCol = -1;
    }
    addColumn( _( "Summary"	) );	_summaryCol	= numCol++;
    addColumn( _( "Size"	) );	_sizeCol	= numCol++;
    addColumn( _( "Source"	) );	_srpmStatusCol	= numCol++;
    setAllColumnsShowFocus( true );
    setSorting( nameCol() );

    setColumnAlignment( _sizeCol, Qt::AlignRight );
    saveColumnWidths();

    connect( this, 	SIGNAL( pkgClicked		( int, YQPkg *, int ) ),
	     this, 	SLOT  ( slotPkgClicked		( int, YQPkg *, int ) ) );

    connect( this, 	SIGNAL( pkgDoubleClicked	( int, YQPkg *, int ) ),
	     this, 	SLOT   ( slotPkgClicked		( int, YQPkg *, int ) ) );

    connect( this, 	SIGNAL( selectionChanged        ( QListViewItem * ) ),
	     this, 	SLOT  ( selectionChangedInternal( QListViewItem * ) ) );

    connect( header(),	SIGNAL( sizeChange		( int, int, int ) ),
	     this,	SLOT  ( columnWidthChanged	( int, int, int ) ) );
}


YQPkgList::~YQPkgList()
{
    // NOP
}


void
YQPkgList::addPkg( PMPackagePtr pkg )
{
    new YQPkg( this, pkg );
}


void
YQPkgList::slotPkgClicked( int button, YQPkg * pkg, int col )
{
    if ( pkg )
    {
	if ( button == Qt::LeftButton )
	{
	    if ( col == srpmStatusCol() )
	    {
		pkg->toggleSourceRpmStatus();
	    }
	    else if ( col == statusCol() )
		      // || col == nameCol() )
	    {
		pkg->cycleStatus();
	    }
	}
    }
}


void
YQPkgList::selectSomething()
{
    QListViewItem * item = firstChild();

    if ( item )
    {
	setSelected( item, true );
    }
}


void
YQPkgList::updateAllItemStates()
{
    QListViewItem * item = firstChild();

    while ( item )
    {
	YQPkg * pkg = dynamic_cast<YQPkg *> ( item );
	
	if ( pkg )
	{
	    // Maybe in some future version this list will contain other types
	    // of items, too - so always use a dynamic cast and check for Null.
	    
	    pkg->setStatusIcon();
	}
	
	item = item->nextSibling();
    }
}


void
YQPkgList::contentsMousePressEvent( QMouseEvent * ev )
{
    QListViewItem * item = itemAt( contentsToViewport( ev->pos() ) );

    if ( item && item->isEnabled() )
    {
	mousePressedItem	= item;
	mousePressedCol		= header()->sectionAt( ev->pos().x() );
	mousePressedButton	= ev->button();
    }
    else	// invalidate last click data
    {
	mousePressedItem	= 0;
	mousePressedCol		= -1;
	mousePressedButton	= -1;
    }

    // Call base class method
    QListView::contentsMousePressEvent( ev );
}


void
YQPkgList::contentsMouseReleaseEvent( QMouseEvent * ev )
{
    QListViewItem * item = itemAt( contentsToViewport( ev->pos() ) );

    if ( item && item->isEnabled() && item == mousePressedItem )
    {
	int col = header()->sectionAt( ev->pos().x() );

	if ( item == mousePressedItem	&&
	     col  == mousePressedCol	&&
	     ev->button() == mousePressedButton )
	{
	    emit( pkgClicked( ev->button(), (YQPkg *) item, col ) );
	}

    }

    // invalidate last click data

    mousePressedItem	= 0;
    mousePressedCol	= -1;
    mousePressedButton	= NoButton;

    // Call base class method
    QListView::contentsMouseReleaseEvent( ev );
}


void
YQPkgList::contentsMouseDoubleClickEvent( QMouseEvent * ev )
{
    QListViewItem * item = itemAt( contentsToViewport( ev->pos() ) );

    if ( item && item->isEnabled() )
    {
	int col = header()->sectionAt( ev->pos().x() );
	emit( pkgDoubleClicked( ev->button(), (YQPkg *) item, col ) );
    }

    // invalidate last click data

    mousePressedItem	= 0;
    mousePressedCol	= -1;
    mousePressedButton	= NoButton;

    // Call base class method
    QListView::contentsMouseDoubleClickEvent( ev );
}


void
YQPkgList::selectionChangedInternal( QListViewItem * new_sel )
{
    YQPkg * sel = dynamic_cast<YQPkg *> (new_sel);

    emit selectionChanged( sel ? sel->pkg() : PMPackagePtr() );
}


void
YQPkgList::saveColumnWidths()
{
    _savedColumnWidth.clear();
    _savedColumnWidth.reserve( columns() );

    for ( int i = 0; i < columns(); i++ )
    {
	_savedColumnWidth.push_back( columnWidth( i ) );
    }
}


void
YQPkgList::restoreColumnWidths()
{
    if ( _savedColumnWidth.size() != (unsigned) columns() )
    {
	y2error( "Number of saved column widths (%d) "
		 "doesn't match current number of columns (%d)!",
		 _savedColumnWidth.size(), columns() );
	return;
    }

    for ( int i = 0; i < columns(); i++ )
    {
	setColumnWidth( i, _savedColumnWidth[ i ] );
    }
}


void
YQPkgList::columnWidthChanged( int, int, int )
{
    saveColumnWidths();
}


void
YQPkgList::clear()
{
    emit selectionChanged( PMPackagePtr() );
    QListView::clear();
    restoreColumnWidths();
}


QSize
YQPkgList::sizeHint() const
{
    return QSize( 600, 350 );
}






YQPkg::YQPkg( YQPkgList * pkgList, PMPackagePtr pkg )
    : QListViewItem( pkgList )
    , _pkgList( pkgList )
    , _pkg( pkg )
{
    _isInstalled = pkg->hasInstalledObj();

    setText( nameCol(),		pkg->name()		  );
    setText( summaryCol(),	pkg->summary()		  );
    setText( sizeCol(),		pkg->size().form() + "  " );

    if ( instVersionCol() >= 0 )
    {
	if ( pkg->hasInstalledObj() )
	     setText( instVersionCol(), pkg->getInstalledObj()->edition() );

	if ( pkg->hasCandidateObj() )
	    setText( versionCol(), pkg->getCandidateObj()->edition() );
    }
    else
    {
	setText( versionCol(),	pkg->edition() );
    }

    _haveSourceRpm	 = true;	// FIXME - get this from the package!

    setStatusIcon();
    setInstallSourceRpm( false ); // No other chance - RPM won't tell if a SRPM is installed
}



YQPkg::~YQPkg()
{

}


void
YQPkg::setText( int column, const std::string text )
{
    QListViewItem::setText( column, fromUTF8( text.c_str() ) );
}


void
YQPkg::setText( int column, const PkgEdition & edition )
{
    QListViewItem::setText( column,
			    fromUTF8( edition.version() )
			    + "-" +
			    fromUTF8( edition.release() )
			    );
}


PMSelectable::UI_Status
YQPkg::status() const
{
    if ( ! _pkg )
    {
	y2error( "NULL package" );
	return isInstalled() ? PMSelectable::S_KeepInstalled : PMSelectable::S_NoInst;
    }

    return _pkg->getSelectable()->status();
}


void
YQPkg::setStatus( PMSelectable::UI_Status newStatus )
{
    _pkg->getSelectable()->set_status( newStatus );
    setStatusIcon();
}


void
YQPkg::setStatusIcon()
{
    QPixmap icon = YQIconPool::pkgNoInst();

    switch ( status() )
    {
        case PMSelectable::S_Taboo:		icon = YQIconPool::pkgTaboo();		break;
        case PMSelectable::S_Del:		icon = YQIconPool::pkgDel();		break;
        case PMSelectable::S_Update:		icon = YQIconPool::pkgUpdate();		break;
        case PMSelectable::S_Install:		icon = YQIconPool::pkgInstall();	break;
        case PMSelectable::S_AutoDel:		icon = YQIconPool::pkgAutoDel();	break;
        case PMSelectable::S_AutoInstall:	icon = YQIconPool::pkgAuto();		break;
        case PMSelectable::S_AutoUpdate:	icon = YQIconPool::pkgAuto();		break;
        case PMSelectable::S_KeepInstalled:	icon = YQIconPool::pkgKeepInstalled();	break;
        case PMSelectable::S_NoInst:		icon = YQIconPool::pkgNoInst();		break;

	    // Intentionally omitting 'default' branch so the compiler can
	    // catch unhandled enum states
    }

    setPixmap( statusCol(), icon );
}


void
YQPkg::setInstallSourceRpm( bool installSourceRpm )
{
    _installSourceRpm = installSourceRpm;

    if ( _haveSourceRpm )
    {
	setPixmap( srpmStatusCol(),
		   _installSourceRpm ? YQIconPool::pkgInstall() : YQIconPool::pkgNoInst() );
    }
}


void
YQPkg::cycleStatus()
{
    PMSelectable::UI_Status oldStatus = status();
    PMSelectable::UI_Status newStatus = oldStatus;

    if ( isInstalled() )
    {
	switch ( oldStatus )
	{
	    case PMSelectable::S_KeepInstalled:	newStatus = _pkg->hasCandidateObj() ?
						    PMSelectable::S_Update : PMSelectable::S_Del;
						break;
	    case PMSelectable::S_Update:	newStatus = PMSelectable::S_Del;		break;
	    case PMSelectable::S_Del:		newStatus = PMSelectable::S_KeepInstalled;	break;
	    default:				newStatus = PMSelectable::S_KeepInstalled;	break;
	}
    }
    else	// pkg not installed
    {
	switch ( oldStatus )
	{
	    case PMSelectable::S_NoInst:	newStatus = PMSelectable::S_Install;	break;
	    default:				newStatus = PMSelectable::S_NoInst;	break;

		// Intentionally NOT cycling through YQPkgTaboo:
		// This status is not common enough for that.
	}
    }

    setStatus( newStatus );
}


void
YQPkg::toggleSourceRpmStatus()
{
    if ( _haveSourceRpm )
	setInstallSourceRpm( ! _installSourceRpm );
}



/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
YQPkg::compare( QListViewItem *		otherListViewItem,
		int			col,
		bool			ascending ) const
{
    YQPkg * other = (YQPkg *) otherListViewItem;

    if ( ! other )
	return 1;

    if ( col == sizeCol() )
    {
	// Numeric sort by size

	if ( this->constPkg()->size() < other->constPkg()->size() ) return -1;
	if ( this->constPkg()->size() > other->constPkg()->size() ) return 1;
	return 0;
    }
    else if ( col == versionCol() )
    {
	// Sorting different packages by version number doesn't make any sense.
	// Even sorting instances of the same package by version is critical:
	// The version numbers may or may not be numerically ascending. There
	// may be CVS snapshots that use entirely different naming schemes -
	// how should we compare "3.01-p4" against "20020814" ?
	// Let's simply forget it. No sort by version.

	return 0;	// FIXME
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
    else if ( col == srpmStatusCol() )
    {
	if (   this->installSourceRpm() && ! other->installSourceRpm() ) return -1;
	if ( ! this->installSourceRpm() &&   other->installSourceRpm() ) return 1;
	return 0;
    }
    else
    {
	// All other columns use string sorting (by locale!) which is what the
	// default method does anyway - let it handle it

	return QListViewItem::compare( other, col, ascending );
    }
}



#include "YQPkgList.moc.cc"
