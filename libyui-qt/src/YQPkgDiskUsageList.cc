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
	item->updateData();

	_items.insert( it->mountpoint().c_str(), item );
	++it;
    }
}


void
YQPkgDiskUsageList::updateDiskUsage()
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


QSize
YQPkgDiskUsageList::sizeHint() const
{
    return QSize( 400, 100 );
}


void
YQPkgDiskUsageList::fakeData()
{
    YQPkgDiskUsageListItem * item;
    
    item = new YQPkgDiskUsageListItem( this, YQPkgDuData( "/",     1024,  2 * FSize::GB, 1400 * FSize::MB ) ); item->updateData();
    item = new YQPkgDiskUsageListItem( this, YQPkgDuData( "/usr",  1024,  3 * FSize::GB, 1800 * FSize::MB ) ); item->updateData();
    item = new YQPkgDiskUsageListItem( this, YQPkgDuData( "/opt",  1024,  3 * FSize::GB, 2750 * FSize::MB ) ); item->updateData();
    item = new YQPkgDiskUsageListItem( this, YQPkgDuData( "/home", 1024, 30 * FSize::GB,   25 * FSize::GB ) ); item->updateData();
}





YQPkgDiskUsageListItem::YQPkgDiskUsageListItem( YQPkgDiskUsageList * 	parent,
						YQPkgDuData		duData )
	: QY2DiskUsageListItem( parent )
	, _duData( duData )
{
    y2milestone( "disk usage list entry for %s", duData.mountpoint().c_str() );
}


FSize
YQPkgDiskUsageListItem::usedSize() const
{
    return _duData.pkg_used();
}


FSize
YQPkgDiskUsageListItem::totalSize() const
{
    return _duData.total();
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
