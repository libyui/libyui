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

#include <Y2PM.h>
#include <y2pm/InstTarget.h>
#include "utf8.h"

#include "YQPkgObjList.h"
#include "YQi18n.h"
#include "YQIconPool.h"


YQPkgObjList::YQPkgObjList( QWidget *parent )
    : QY2ListView( parent )
{
    // This class does not add any columns. This is the main reason why this is
    // an abstract base class: It doesn't know which columns are desired and in
    // what order.
    
    _statusCol		= -42;
    _nameCol		= -42;
    _versionCol		= -42;
    _instVersionCol 	= -42;
    _summaryCol		= -42;
    _sizeCol		= -42;

    connect( this, 	SIGNAL( columnClicked		( int, QListViewItem *, int ) ),
	     this, 	SLOT  ( pkgObjClicked		( int, QListViewItem *, int ) ) );

    connect( this, 	SIGNAL( columnDoubleClicked	( int, QListViewItem *, int ) ),
	     this, 	SLOT  ( pkgObjClicked		( int, QListViewItem *, int ) ) );

    connect( this, 	SIGNAL( selectionChanged        ( QListViewItem * ) ),
	     this, 	SLOT  ( selectionChangedInternal( QListViewItem * ) ) );
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
YQPkgObjList::pkgObjClicked( int button, QListViewItem * listViewItem, int col )
{
    YQPkgObjListItem * item = dynamic_cast<YQPkgObjListItem *> (listViewItem);
	
    if ( item )
    {
	if ( button == Qt::LeftButton )
	{
	    if ( col == statusCol() )
		// || col == nameCol() )
	    {
		item->cycleStatus();
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






YQPkgObjListItem::YQPkgObjListItem( YQPkgObjList * pkgObjList, PMObjectPtr pm_obj )
    : QY2ListViewItem( pkgObjList )
    , _pkgObjList( pkgObjList )
    , _pmObj( pm_obj )
{
    if ( nameCol()    >= 0 ) 	setText( nameCol(),	pmObj()->name()		);
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
    return constPMObj()->getSelectable()->status();
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

    if ( ! other )
	return QY2ListViewItem::compare( otherListViewItem, col, ascending );

    if ( col == sizeCol() )
    {
	// Numeric sort by size

	if ( this->constPMObj()->size() < other->constPMObj()->size() ) return -1;
	if ( this->constPMObj()->size() > other->constPMObj()->size() ) return 1;
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

    // Fallback: Use parent class method
    return QY2ListViewItem::compare( otherListViewItem, col, ascending );
}



#include "YQPkgObjList.moc.cc"
