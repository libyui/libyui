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
#include "YQPkgDiskUsageWarningDialog.h"
#include "YQi18n.h"


// Warning ranges for "disk space is running out" or "disk space overflow".
// The WARN value triggers a warning popup once (!). The warning will not be
// displayed again until the value sinks below the PROXIMITY value and then
// increases again to the WARN value.
//
// See class YQPkgWarningRangeNotifier in file YQPkgDiskUsageList.h for details.

#define MIN_FREE_MB_WARN	400
#define MIN_FREE_MB_PROXIMITY	700

#define MIN_PERCENT_WARN	90
#define MIN_PERCENT_PROXIMITY	80

#define OVERFLOW_MB_WARN	0
#define OVERFLOW_MB_PROXIMITY	300



YQPkgDiskUsageList::YQPkgDiskUsageList( QWidget *parent, int thresholdPercent )
    : QY2DiskUsageList( parent, true )
{
    _debug 	= false;

    const std::set<PkgDuMaster::MountPoint>	du = Y2PM::packageManager().getDu().mountpoints();
    std::set<PkgDuMaster::MountPoint>::iterator it = du.begin();

    while ( it != du.end() )
    {
	if ( thresholdPercent == 0 ||	// Slight optimization for the most common case
	     it->pkg_u_percent() >= thresholdPercent )
	{
	    YQPkgDiskUsageListItem * item = new YQPkgDiskUsageListItem( this, *it );
	    CHECK_PTR( item );
	    item->updateData();
	    _items.insert( it->mountpoint().c_str(), item );
	}

	++it;
    }
}


void
YQPkgDiskUsageList::updateDiskUsage()
{
    YUIQt::yuiqt()->busyCursor();

    runningOutWarning.clear();
    overflowWarning.clear();

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
    postPendingWarnings();
}


void
YQPkgDiskUsageList::postPendingWarnings()
{
    if ( overflowWarning.needWarning() )
    {
	YQPkgDiskUsageWarningDialog::diskUsageWarning( _( "<b>Error:</b> Out of disk space!" ),
						       100, _( "&OK" ) );

	overflowWarning.warningPostedNotify();
	runningOutWarning.warningPostedNotify(); // Suppress this (now redundant) other warning
    }

    if ( runningOutWarning.needWarning() )
    {
	YQPkgDiskUsageWarningDialog::diskUsageWarning( _( "<b>Warning:</b> Disk space is running out!" ) ,
						       MIN_PERCENT_WARN, _( "&OK" ) );
	runningOutWarning.warningPostedNotify();
    }

    if ( overflowWarning.leavingProximity() )
	overflowWarning.clearHistory();

    if ( runningOutWarning.leavingProximity() )
	runningOutWarning.clearHistory();
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


void
YQPkgDiskUsageList::keyPressEvent( QKeyEvent *event )
{

    if ( event )
    {
	unsigned special_combo = ( Qt::ControlButton | Qt::ShiftButton | Qt::AltButton );

	if ( ( event->state() & special_combo ) == special_combo )
	{
	    if ( event->key() == Qt::Key_Q )
	    {
		_debug = ! _debug;
		y2milestone( "Debug mode %s", _debug ? "on" : "off" );
	    }

	}

	if ( _debug && currentItem() )
	{
	    YQPkgDiskUsageListItem * item = dynamic_cast<YQPkgDiskUsageListItem *> ( currentItem() );

	    if ( item )
	    {
		{
		    int percent = item->usedPercent();

		    switch ( event->ascii() )
		    {
			case '0':	percent	= 0;	break;
			case '1':	percent	= 100;	break;
			case '2':	percent	= 20;	break;
			case '3':	percent	= 30;	break;
			case '4':	percent	= 40;	break;
			case '5':	percent	= 50;	break;
			case '6':	percent	= 60;	break;
			case '7':	percent	= 70;	break;
			case '8':	percent	= 80;	break;
			case '9':	percent	= 90;	break;
			case 'o':	percent	= 120;	break;
			case '+':	percent += 2; 	break;
			case '-':	percent--;	break;

			case 'w':
			    // Only for testing, thus intentionally using no translations
			    YQPkgDiskUsageWarningDialog::diskUsageWarning( "<b>Warning:</b> Disk space is running out!",
									   90, "&OK" );
			    break;

			case 'f':
			    YQPkgDiskUsageWarningDialog::diskUsageWarning( "<b>Error:</b> Out of disk space!",
									   100, "&Continue anyway", "&Cancel" );
			    break;
		    }

		    if ( percent < 0   ) percent = 0;
		    YQPkgDuData du( item->duData() );

		    if ( percent != item->usedPercent() )
		    {
			du._used = du.total() * percent / 100;

			runningOutWarning.clear();
			overflowWarning.clear();

			item->updateDuData( du );
			postPendingWarnings();
		    }
		}
	    }
	}
    }

    QY2DiskUsageList::keyPressEvent( event );
}






YQPkgDiskUsageListItem::YQPkgDiskUsageListItem( YQPkgDiskUsageList * 	parent,
						YQPkgDuData		duData )
	: QY2DiskUsageListItem( parent )
	, _duData( duData )
	, _pkgDiskUsageList( parent )
{
    y2debug( "disk usage list entry for %s", duData.mountpoint().c_str() );
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
    checkRemainingDiskSpace();
}


void
YQPkgDiskUsageListItem::checkRemainingDiskSpace()
{
    int	percent = usedPercent();
    int	free	= freeSize() / FSize::MB;

    if ( percent > MIN_PERCENT_WARN )
    {
	// Modern hard disks can be huge, so a warning based on percentage only
	// can be misleading - check the absolute value, too.

	if ( free < MIN_FREE_MB_PROXIMITY )
	    _pkgDiskUsageList->runningOutWarning.enterProximity();

	if ( free < MIN_FREE_MB_WARN )
	    _pkgDiskUsageList->runningOutWarning.enterRange();
    }

    if ( free < MIN_FREE_MB_PROXIMITY )
    {
	if ( percent > MIN_PERCENT_PROXIMITY )
	    _pkgDiskUsageList->runningOutWarning.enterProximity();
    }

    if ( free < OVERFLOW_MB_WARN )
	_pkgDiskUsageList->overflowWarning.enterRange();

    if ( free < OVERFLOW_MB_PROXIMITY )
	_pkgDiskUsageList->overflowWarning.enterProximity();
}






YQPkgWarningRangeNotifier::YQPkgWarningRangeNotifier()
{
    clearHistory();
}


void
YQPkgWarningRangeNotifier::clear()
{
    _inRange 		= false;
    _hasBeenClose	= _isClose;
    _isClose 		= false;
}


void
YQPkgWarningRangeNotifier::clearHistory()
{
    clear();
    _hasBeenClose  = false;
    _warningPosted = false;
}


void
YQPkgWarningRangeNotifier::enterRange()
{
    _inRange = true;
    enterProximity();
}


void
YQPkgWarningRangeNotifier::enterProximity()
{
    _isClose      = true;
    _hasBeenClose = true;
}


void
YQPkgWarningRangeNotifier::warningPostedNotify()
{
    _warningPosted = true;
}


bool
YQPkgWarningRangeNotifier::inRange() const
{
    return _inRange;
}


bool
YQPkgWarningRangeNotifier::leavingProximity() const
{
    return ! _isClose && ! _hasBeenClose;
}


bool
YQPkgWarningRangeNotifier::needWarning() const
{
    return _inRange && ! _warningPosted;
}




#include "YQPkgDiskUsageList.moc.cc"
