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

  File:	      YQPkgDiskUsageList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <Y2PM.h>
#include <y2pm/PMPackageManager.h>
#include "utf8.h"

#include "YUIQt.h"
#include "YQPkgDiskUsageList.h"
#include "YQi18n.h"



YQPkgDiskUsageList::YQPkgDiskUsageList( QWidget *parent )
    : QY2DiskUsageList( parent, true )
{
    const std::set<PkgDuMaster::MountPoint>	du = Y2PM::packageManager().getDu().mountpoints();
    std::set<PkgDuMaster::MountPoint>::iterator it = du.begin();

    while ( it != du.end() )
    {
	YQPkgDiskUsageListItem * item = new YQPkgDiskUsageListItem( this, *it );
	CHECK_PTR( item );

	_items.insert( it->mountpoint().c_str(), item );
	++it;
    }
}


void
YQPkgDiskUsageList::updateDuData()
{
    YUIQt::yuiqt()->busyCursor();

    const std::set<PkgDuMaster::MountPoint> 	du = Y2PM::packageManager().updateDu().mountpoints();
    std::set<PkgDuMaster::MountPoint>::iterator it = du.begin();

    while ( it != du.end() )
    {
	YQPkgDiskUsageListItem * item = _items[ it->mountpoint().c_str() ];

	if ( item )
	    item->updateDuData( *it );
	else
	    y2error( "No entry for mount point %s", it->mountpoint().c_str() );

	++it;
    }

    YUIQt::yuiqt()->normalCursor();
}






YQPkgDiskUsageListItem::YQPkgDiskUsageListItem( YQPkgDiskUsageList * 	parent,
						YQPkgDuData		duData )
	: QY2DiskUsageListItem( parent )
	, _duData( duData )
{
}


QString
YQPkgDiskUsageListItem::name() const
{
    return fromUTF8( _duData.mountpoint().c_str() );
}


void
YQPkgDiskUsageListItem::updateDuData( const YQPkgDuData & fromData )
{
    _duData.assignData( fromData );
    updateData();
}



#include "YQPkgDiskUsageList.moc.cc"
