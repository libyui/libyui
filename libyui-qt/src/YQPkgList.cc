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
#include <qpainter.h>

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
    
    saveColumnWidths();
    setSorting( nameCol() );
    setColumnAlignment( sizeCol(), Qt::AlignRight );
    setAllColumnsShowFocus( true );
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
YQPkgList::pkgObjClicked( int button, QListViewItem * listViewItem, int col )
{
    if ( button == Qt::LeftButton )
    {
	if ( col == srpmStatusCol() )
	{
	    YQPkgListItem * item = dynamic_cast<YQPkgListItem *> (listViewItem);
	
	    if ( item )
	    {
		item->toggleSourceRpmStatus();
		return;
	    }
	}
    }

    YQPkgObjList::pkgObjClicked( button, listViewItem, col );
}


QSize
YQPkgList::sizeHint() const
{
    return QSize( 600, 350 );
}






YQPkgListItem::YQPkgListItem( YQPkgList * pkgList, PMPackagePtr pmPkg )
    : YQPkgObjListItem( pkgList, pmPkg )
    , _pkgList( pkgList )
    , _pmPkg( pmPkg )
{
    CHECK_PTR( pmPkg );
    CHECK_PTR( pkgList );
    
    _haveSourceRpm	 = true;	// FIXME - get this from the package!

    setStatusIcon();
    setInstallSourceRpm( false ); // No other chance - RPM won't tell if a SRPM is installed
    _candidateIsNewer = false;
    _installedIsNewer = false;

    PMObjectPtr candidate = _pmPkg->getCandidateObj();
    PMObjectPtr installed = _pmPkg->getInstalledObj();

    if ( candidate && installed )
    {
	_candidateIsNewer = candidate->edition() > installed->edition();
	_installedIsNewer = candidate->edition() < installed->edition();
	// Cache this information, it's expensive to obtain!
    }
}



YQPkgListItem::~YQPkgListItem()
{
    // NOP
}


void
YQPkgListItem::setInstallSourceRpm( bool installSourceRpm )
{
    _installSourceRpm = installSourceRpm;

    if ( _haveSourceRpm )
    {
	setPixmap( srpmStatusCol(),
		   _installSourceRpm ? YQIconPool::pkgInstall() : YQIconPool::pkgNoInst() );
    }
}


void
YQPkgListItem::toggleSourceRpmStatus()
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
YQPkgListItem::compare( QListViewItem *		otherListViewItem,
			int			col,
			bool			ascending ) const
{
    if ( col == srpmStatusCol() )
    {
	YQPkgListItem * other = dynamic_cast<YQPkgListItem *> (otherListViewItem);

	if ( other )
	{
	    if (   this->installSourceRpm() && ! other->installSourceRpm() ) return -1;
	    if ( ! this->installSourceRpm() &&   other->installSourceRpm() ) return 1;
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
    if ( _installedIsNewer )
    {
	QColorGroup cg = colorGroup;


	if ( column == instVersionCol() )
	    cg.setColor( QColorGroup::Base, QColor( 0xFF, 0x30, 0x30 ) );	// Background
	else
	    cg.setColor( QColorGroup::Text, QColor( 0xFF, 0, 0 ) );		// Foreground

	QListViewItem::paintCell( painter, cg, column, width, alignment );
    }
    else if ( _candidateIsNewer )
    {
	QColorGroup cg = colorGroup;

	cg.setColor( QColorGroup::Text, QColor( 0, 0, 0xC0 ) );		// Foreground
	
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
