/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| Copyright (c) 2018 SUSE LLC
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/


/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPkgPopupDiskspace.cc

   Author:     Gabriele Strattner <gs@suse.de>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "YMenuButton.h"
#include "YDialog.h"
#include "YTypes.h"

#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCPkgStrings.h"
#include "NCLabel.h"
#include "NCPushButton.h"
#include "NCTable.h"

#include "NCZypp.h"

#include "NCPkgPopupDiskspace.h"

#include "NCi18n.h"

// zypp::str::form()
#include <zypp/base/String.h>

// set values as set in YQPkgDiskUsageList.cc
#define MIN_FREE_MB_WARN	400
#define MIN_FREE_MB_PROXIMITY	700

#define MIN_PERCENT_WARN	90
#define MIN_PERCENT_PROXIMITY	80

#define OVERFLOW_MB_WARN	0
#define OVERFLOW_MB_PROXIMITY	300

using std::endl;

/*
  Textdomain "ncurses-pkg"
*/

 ///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::NCPkgDiskspace
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgDiskspace::NCPkgDiskspace( bool testMode )
      : testmode( testMode )
      , popupWin( 0 )
{

    if ( testMode )
    {
	yuiMilestone() << "TESTMODE Diskspace" << endl;
	zypp::getZYpp()->setPartitions(zypp::DiskUsageCounter::detectMountPoints ());
	testDiskUsage = zypp::getZYpp()->diskUsage();
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::~NCPkgDiskspace
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgDiskspace::~NCPkgDiskspace()
{
}

namespace {
    std::string formatSize(double size, int width = 0)
    {
        // FSize::bestUnit does not work for huge numbers so only use it for small ones
        FSize::Unit unit = (size >= FSize::TB) ? FSize::T : FSize(size).bestUnit();
        int prec = unit == FSize::B ? 0 : 2;

        return zypp::str::form( "%*.*f %s", width, prec, size / FSize::factor(unit), FSize::unit(unit));
    }

    /**
     * Compute percent usage
     * @param  used  used size (any unit, but the same as the "total")
     * @param  total total size (any unit, but the same as the "used")
     * @return       percent, might be more than 100 if the size of the selected
     *      packages is bigger than the available free size
     */
    int usedPercentInt(long long used, long long total)
    {
        int percent = 0;

        if ( total != 0 )
           // temporarily use double to avoid overflow
    	   percent = ( 100.0 * used ) / total;

        return percent;
    }

    // see usedPercentInt()
    std::string usedPercentStr(long long used, long long total)
    {
        int percent = usedPercentInt(used, total);
        return zypp::str::form( "%2d%%", percent );
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::fillPartitionTable
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgDiskspace::fillPartitionTable()
{
    NCTable * partitions = popupWin->Partitions();
    partitions->deleteAllItems();		// clear table

    zypp::ZYpp::Ptr z = zypp::getZYpp();
    ZyppDuSet du = z->diskUsage ();

    if (du.empty())
    {
        // retry after detecting from the target
        z->setPartitions(zypp::DiskUsageCounter::detectMountPoints ());
        du = z->diskUsage();
    }

    for (const ZyppPartitionDu &item: du)
    {
	    if (item.readonly)
	       continue;

        double pkg_used = double(item.pkg_size) * FSize::KB;
        double pkg_available = double(item.total_size - item.pkg_size) * FSize::KB;
        double total = double(item.total_size) * FSize::KB;

	    YTableItem * newItem = new YTableItem( item.dir,
				  formatSize(pkg_used, 8),
				  formatSize(pkg_available, 8),
				  formatSize(total, 8),
				  usedPercentStr( item.pkg_size, item.total_size ) );

        partitions->addItem( newItem );
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::checkDiskSpace
//	METHOD TYPE : std::string
//
//	DESCRIPTION : called to check disk space before installation
//		      (after OK button is pressed)
//
std::string NCPkgDiskspace::checkDiskSpace()
{
    std::string text;

    zypp::ZYpp::Ptr z = zypp::getZYpp();
    ZyppDuSet du = z->diskUsage ();

    if (du.empty())
    {
        // retry after detecting from the target
        z->setPartitions(zypp::DiskUsageCounter::detectMountPoints ());
        du = z->diskUsage();
    }

    for (const ZyppPartitionDu &item: du)
    {
	if (item.readonly)
	    continue;

    // available size (in KiB!)
	long long pkg_available = item.total_size - item.pkg_size;
	if ( pkg_available < 0 )
	{
	    text += "\"";
	    text += item.dir;
	    text += "\"";
	    text += " ";
	    text += NCPkgStrings::MoreText();
	    text += " ";
        // make positive, use double to avoid overflow
	    text += formatSize(-1.0 * double(pkg_available) * FSize::KB);
	    text += " ";
	    text += NCPkgStrings::MoreSpaceText();
	    text += "<br>";
	}
    }
    return text;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::checkRemainingDiskSpace
//	METHOD TYPE : void
//
//	DESCRIPTION : check whether remaining disk space enters
// 		      warning or error range
//
void NCPkgDiskspace::checkRemainingDiskSpace( const ZyppPartitionDu & partition )
{
    if ( partition.readonly )
	return;

    int percent = usedPercentInt(partition.pkg_size, partition.total_size);

    // free in MiB - libzyp sizes are already in KiB, divide by 1024 to get MiB
    long long free = (partition.total_size - partition.pkg_size) / 1024;

    yuiMilestone() <<  "Partition: " << partition.dir << "  Total (MiB): "
        << partition.total_size / 1024   << "  Used (MiB): " << partition.pkg_size / 1024
        << "  Used percent: " << percent  << "%  Free (MiB): " << free << endl;

    if ( percent > MIN_PERCENT_WARN )
    {
	// Modern hard disks can be huge, so a warning based on percentage only
	// can be misleading - check the absolute value, too.
	if ( free < MIN_FREE_MB_PROXIMITY )
	{
        yuiWarning() << "free < MIN_FREE_MB_PROXIMITY (" << MIN_FREE_MB_PROXIMITY << ")" << endl;
	    runningOutWarning.enterProximity();
	}
	if ( free < MIN_FREE_MB_WARN )
	{
        yuiWarning() << "free < MIN_FREE_MB_WARN (" << MIN_FREE_MB_WARN << ")" << endl;
	    runningOutWarning.enterRange();
	}
    }

    if ( free < MIN_FREE_MB_PROXIMITY )
    {
	if ( percent > MIN_PERCENT_PROXIMITY )
	    runningOutWarning.enterProximity();
    }

    if ( free < OVERFLOW_MB_WARN )
	overflowWarning.enterRange();

    if ( free < OVERFLOW_MB_PROXIMITY )
	overflowWarning.enterProximity();

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::setDiskSpace
//	METHOD TYPE : void
//
//	DESCRIPTION : For testing only; called from NCPkgTable if the PackageSelector
//		      running in testMode
//	TESTDESCRIPTION: Call `PackageSelector with `opt(`testMode) (ycp example).
//		      	 With focus on the package list press '+' or '-' to
//		      	 increase/decrease used diskspace (see y2log).
//			 Use the 'Actions' menu to select/delete a package.
//
void NCPkgDiskspace::setDiskSpace( wint_t ch )
{
    // set diskspace values in ZyppDuSet testDiskSpace
    for ( const ZyppPartitionDu &partitionDu: testDiskUsage )
    {
    int percent = usedPercentInt(partitionDu.pkg_size, partitionDu.total_size);

	if ( ch == '+' )
	    percent += 3;
	else if ( ch == '-' )
	    percent -= 3;

	if ( percent < 0   )
	    percent = 0;

    partitionDu.pkg_size = partitionDu.total_size / 100 * percent;

    // libzyp sizes are already in KiB, divide by 1024 to get MiB
	yuiMilestone() << "Used size (MiB): " << partitionDu.pkg_size / 1024 << endl;
	yuiMilestone() << "Total size (MiB): " << partitionDu.total_size / 1024 << endl;
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::checkDiskSpaceRange
//	METHOD TYPE : void
//
//	DESCRIPTION : calls checkRemaingDiskspace for every partition
//
void NCPkgDiskspace::checkDiskSpaceRange( )
{
    // see YQPkgDiskUsageList::updateDiskUsage()
    runningOutWarning.clear();
    overflowWarning.clear();
    ZyppDuSet diskUsage;

    if ( testmode )
	diskUsage = testDiskUsage;
    else
	diskUsage = zypp::getZYpp()->diskUsage();

    for ( ZyppDuSetIterator it = diskUsage.begin();
	  it != diskUsage.end();
	  ++it )
    {
	//Exclude readonly dirs from the check (#384368)
	if( it->readonly )
	    continue;
	checkRemainingDiskSpace( *it );
    }

     // see YQPkgDiskUsageList::postPendingWarnings()
    if ( overflowWarning.needWarning() )
    {
	showInfoPopup( _( "Error: Out of disk space!" ) );

	overflowWarning.warningPostedNotify();
	runningOutWarning.warningPostedNotify(); // Suppress this ( now redundant ) other warning
    }

    if ( runningOutWarning.needWarning() )
    {
	showInfoPopup( _( "Warning: Disk space is running out!" ) );

	runningOutWarning.warningPostedNotify();
    }

    if ( overflowWarning.leavingProximity() )
	overflowWarning.clearHistory();

    if ( runningOutWarning.leavingProximity() )
	runningOutWarning.clearHistory();

    if ( testmode )
    {
	yuiMilestone() << "Running out Warning:" << endl;
	runningOutWarning.logSettings();

	yuiMilestone() << "Overflow Warning:" << endl;
	overflowWarning.logSettings();
    }
}

// FIXME: do not use this, contains overflow bug, use usedPercentStr() instead!
std::string NCPkgDiskspace::usedPercent( FSize used, FSize total )
{
    int percent = 0;
    char percentStr[10];

    if ( total != 0 )
	percent = ( 100 * used ) / total;

    sprintf( percentStr, "%d%%", percent );

    return percentStr;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgDiskspace::showInfoPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgDiskspace::showInfoPopup( std::string headline )
{

    popupWin = new NCPkgPopupDiskspace (wpos( (NCurses::lines() - 15)/2, NCurses::cols()/6  ), headline );
    // update values in partition table
    fillPartitionTable();
    popupWin->doit();
    YDialog::deleteTopmostDialog();
}

zypp::ByteCount NCPkgDiskspace::calculateDiff()
{
    zypp::ZYpp::Ptr z = zypp::getZYpp();
    ZyppDuSet du = z->diskUsage ();

    if (du.empty())
    {
        // retry after detecting from the target
        z->setPartitions(zypp::DiskUsageCounter::detectMountPoints ());
        du = z->diskUsage();
    }

    zypp::ByteCount diff = 0;
    for (const ZyppPartitionDu &item: du)
    {
        // the diff should be normally very small, the "long long" limit should be never reached (TM)
        diff += (item.pkg_size - item.used_size) * 1024;
    }

    return diff;
}

//////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDiskspace::NCPkgPopupDiskspace
//	METHOD TYPE : Constructor
//
NCPkgPopupDiskspace::NCPkgPopupDiskspace( const wpos at, std::string headline )
    : NCPopup( at, false )
    , partitions( 0 )
    , okButton( 0 )
    , head( 0 )
{
    createLayout( headline );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDiskspace::~NCPkgPopupDiskspace
//	METHOD TYPE : Destructor
//
NCPkgPopupDiskspace::~NCPkgPopupDiskspace()
{
}

//////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDiskspace::createLyout
//	METHOD TYPE : void
//
//	DESCRIPTION : create layout (partition table)
//
void NCPkgPopupDiskspace::createLayout( std::string headline )
{
    // the vertical split is the (only) child of the dialog
    NCLayoutBox * split = new NCLayoutBox( this, YD_VERT );

    head = new NCLabel( split, "", true, false );	// isHeading = true
    head->setLabel( headline );

    YTableHeader * tableHeader = new YTableHeader();
    tableHeader->addColumn( NCPkgStrings::Partition(), YAlignBegin );
    tableHeader->addColumn( NCPkgStrings::UsedSpace(), YAlignBegin );
    tableHeader->addColumn( NCPkgStrings::FreeSpace(), YAlignBegin );
    tableHeader->addColumn( NCPkgStrings::TotalSpace(), YAlignBegin );
    tableHeader->addColumn( "% ", YAlignBegin );

    // add the partition table
    partitions = new NCTable( split, tableHeader );

    // add the ok button
    okButton = new NCPushButton( split, NCPkgStrings::OKLabel() );
    okButton->setFunctionKey( 10 );
    okButton->setKeyboardFocus();
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDiskspace::preferredWidth
//	METHOD TYPE : int
//
int NCPkgPopupDiskspace::preferredWidth()
{
    return NCurses::cols()*2/3;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDiskspace::preferredHeight
//	METHOD TYPE : int
//
int NCPkgPopupDiskspace::preferredHeight()
{
    if ( NCurses::lines() > 15 )
	return 15;
    else
	return NCurses::lines()-4;
}

void NCPkgPopupDiskspace::doit()
{
    postevent = NCursesEvent();
    do {
	// show the popup
	popupDialog( );
    } while ( postAgain() );

    popdownDialog();

}
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPopupDiskspace::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    if ( ch == KEY_RETURN )
	return NCursesEvent::button;

    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDiskspace::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgPopupDiskspace::postAgain()
{
    if ( ! postevent.widget )
	return false;

    if ( postevent == NCursesEvent::button || postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }
    return true;
}




NCPkgWarningRangeNotifier::NCPkgWarningRangeNotifier()
{
    clearHistory();
}


void
NCPkgWarningRangeNotifier::clear()
{
    _inRange 		= false;
    _hasBeenClose	= _isClose;
    _isClose 		= false;
}


void
NCPkgWarningRangeNotifier::clearHistory()
{
    clear();
    _hasBeenClose  = false;
    _warningPosted = false;
}


void
NCPkgWarningRangeNotifier::enterRange()
{
    _inRange = true;
    enterProximity();
}


void
NCPkgWarningRangeNotifier::enterProximity()
{
    _isClose      = true;
    _hasBeenClose = true;
}


void
NCPkgWarningRangeNotifier::warningPostedNotify()
{
    _warningPosted = true;
}


bool
NCPkgWarningRangeNotifier::inRange() const
{
    return _inRange;
}


bool
NCPkgWarningRangeNotifier::leavingProximity() const
{
    return ! _isClose && ! _hasBeenClose;
}


bool
NCPkgWarningRangeNotifier::needWarning() const
{
    return _inRange && ! _warningPosted;
}

void
NCPkgWarningRangeNotifier::logSettings() const
{
    yuiMilestone() << "in range: " << (_inRange?"true":"false") << endl;
    yuiMilestone() << "is close: " << (_isClose?"true":"false") << endl;
    yuiMilestone() << "has been close: " << (_hasBeenClose?"true":"false") << endl;
    yuiMilestone() << "warning posted: " << (_warningPosted?"true":"false") << endl;
}

