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

// arbitrary precision integer
#include <boost/multiprecision/cpp_int.hpp>

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

namespace
{
    /**
     * Local helper method, obtain the current disk usage. Initializes the libzypp
     * disk usage with the current values from the system if needed.
     * @return Libzypp disk usage
     */
    ZyppDuSet get_du()
    {
        ZyppDuSet diskUsage = zypp::getZYpp()->diskUsage();

        if ( diskUsage.empty() )
        {
            zypp::getZYpp()->setPartitions( zypp::DiskUsageCounter::detectMountPoints() );
            diskUsage = zypp::getZYpp()->diskUsage();
        }

        return diskUsage;
    }

    /**
     * Compute used percent for the used and the total size.
     * @param  used  the used size
     * @param  total the total size
     * @return       used percent (returns zero when 'total' is zero to avoid
     *               division by zero)
     */
    int usedPercentInt(const FSize &used, const FSize &total)
    {
        int percent = 0;

        if ( total != 0 )
            percent = int(( 100 * used ) / total);

        return percent;
    }

    /**
     * Compute the dialog width, try to make all fields visible. Make it
     * dependant on the longest mount point path.
     * @return width
     */
    int dialogWidth()
    {
        int width = 0;
        for (const ZyppPartitionDu &du: get_du())
        {
            if ( int(du.dir.length()) > width )
                width = du.dir.length();
        }
        yuiDebug() << "The longest mount point path: " << width << " characters" << endl;

        // add the width of the other columns + small extra space
        // (e.g. buffer for longer translations)
        width += 50;

        // cannot be wider than the screen, keep some minimal space around the popup
        if (width > NCurses::cols() - 6)
            width = NCurses::cols() - 6;

        yuiDebug() << "Dialog width: " << width << endl;

        return width;
    }

    /**
     * Compute the dialog X position on the screen.
     * @return X position
     */
    int dialogXpos()
    {
        return (NCurses::cols() - dialogWidth()) / 2;
    }
}

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
	zypp::getZYpp()->setPartitions(zypp::DiskUsageCounter::detectMountPoints());
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

    ZyppDuSet du = get_du();
    for (const ZyppPartitionDu &item: du)
    {
	if (item.readonly)
	    continue;

	FSize pkg_used (item.pkg_size, FSize::Unit::K);
	FSize pkg_available ((item.total_size - item.pkg_size), FSize::Unit::K);
	FSize total (item.total_size, FSize::Unit::K);

	YTableItem *newItem = new YTableItem( item.dir,
				  pkg_used.form(8),
				  pkg_available.form(8),
				  total.form(8),
				  usedPercent( pkg_used, total ) );

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
    ZyppDuSet diskUsage = get_du();

    std::string text = "";
    for (const ZyppPartitionDu &du: diskUsage)
    {
    	if (du.readonly)
    	    continue;

    	FSize pkg_available(du.total_size - du.pkg_size, FSize::Unit::K);
    	if ( pkg_available < 0 )
    	{
            // make it positive
            pkg_available *= -1;
    	    text += "\"";
    	    text += du.dir;
    	    text += "\"";
    	    text += " ";
    	    text += NCPkgStrings::MoreText();
    	    text += " ";
    	    text += pkg_available.asString();
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

    FSize usedSize ( partition.pkg_size, FSize::Unit::K );
    FSize totalSize ( partition.total_size, FSize::Unit::K );

    int percent = usedPercentInt(usedSize, totalSize);

    // free size in MiB
    boost::multiprecision::cpp_int free = ( totalSize - usedSize ).in_unit(FSize::Unit::M);

    yuiMilestone() <<  "Partition: " << partition.dir << "  Used percent: "
	  << percent << "  Free: " << free << endl;

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
	FSize usedSize ( partitionDu.pkg_size, FSize::Unit::K );
	FSize totalSize ( partitionDu.total_size, FSize::Unit::K );
    int percent = usedPercentInt(usedSize, totalSize);

	if ( ch == '+' )
	    percent += 3;
	else if ( ch == '-' )
	    percent -= 3;

	if ( percent < 0   )
	    percent = 0;

	partitionDu.pkg_size = partitionDu.total_size / 100 * percent;

	FSize newSize ( partitionDu.pkg_size, FSize::Unit::K );

	yuiMilestone() << "Used size (MiB): " << newSize.in_unit(FSize::Unit::M) << endl;
	yuiMilestone() << "Total size (MiB): " << totalSize.in_unit(FSize::Unit::M) << endl;
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
void NCPkgDiskspace::checkDiskSpaceRange()
{
    // see YQPkgDiskUsageList::updateDiskUsage()
    runningOutWarning.clear();
    overflowWarning.clear();
    ZyppDuSet diskUsage;

    if ( testmode )
	diskUsage = testDiskUsage;
    else
	diskUsage = zypp::getZYpp()->diskUsage();

    for (const ZyppPartitionDu &du: diskUsage)
    {
	// Exclude readonly dirs from the check (#384368)
	if ( du.readonly )
	    continue;
	checkRemainingDiskSpace( du );
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

std::string NCPkgDiskspace::usedPercent( const FSize &used, const FSize &total )
{
    int percent = usedPercentInt(used, total);
    return zypp::str::form( "%2d%%", percent );
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

    popupWin = new NCPkgPopupDiskspace (wpos( (NCurses::lines() - 15)/2, dialogXpos() ), headline );
    // update values in partition table
    fillPartitionTable();
    popupWin->doit();
    YDialog::deleteTopmostDialog();
}

FSize NCPkgDiskspace::calculateDiff()
{
    ZyppDuSet diskUsage = get_du();

    FSize diff = 0;
    for (const ZyppPartitionDu &du: diskUsage)
    {
	    diff += FSize(du.pkg_size - du.used_size, FSize::Unit::K);
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
    return dialogWidth();
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
    do
    {
	// show the popup
	popupDialog();
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

