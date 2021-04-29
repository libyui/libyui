/*
  Copyright (c) [2002-2011] Novell, Inc.
  Copyright (c) 2021 SUSE LLC

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA


  File:       NCPkgPopupDiskspace.h
  Author:     Gabriele Strattner <gs@suse.de>

*/


#ifndef NCPkgPopupDiskspace_h
#define NCPkgPopupDiskspace_h

#include <iosfwd>
#include <vector>
#include <string>

#include <yui/FSize.h>
#include <yui/ncurses/NCPopup.h>
#include <yui/ncurses/NCLabel.h>

#include "NCZypp.h"


typedef zypp::DiskUsageCounter::MountPoint 		ZyppPartitionDu;
typedef zypp::DiskUsageCounter::MountPointSet 		ZyppDuSet;
typedef zypp::DiskUsageCounter::MountPointSet::iterator ZyppDuSetIterator;

class NCTable;
class NCPushButton;

///////////////////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgWarningRangeNotifier
//
//	Class YQPkgWarningRangeNotifier from YQPkgDiskUsageList.h
//

/**
 * Helper class to manage warnings that are to be issued when a value enters a
 * predefined range, but repeated only when that value leaves a (wider)
 * "proximity" range and then re-enters the (narrower) "inner" range.
 *
 * Example: Disk space usage:
 *
 *	70%
 *	75%
 *	80% 	[proximity range start]
 *	85%
 *	90%	[inner range start]
 *	95%
 *	100%
 *
 * A warning is to be posted when there is only 90% disk space left. After the
 * warning is displayed, there shall be no more warning until disk usage decreases
 * below 80% (the proximity range) and then later increases again to 90%.
 *
 * The net effect of all that is to avoid posting the warning over and over
 * again while the value changes back and forth around the boundary of the
 * ( inner ) warning range.
 **/
class NCPkgWarningRangeNotifier
{
public:

    /**
     * Constructor.
     **/
    NCPkgWarningRangeNotifier();

    /**
     * Notification that the inner range is entered.
     * The caller has to decide the criteria for that.
     **/
    void enterRange();

    /**
     * Notification that the proximity range is entered, i.e. that the value is
     * getting near the inner range.
     * 'enterRange()' automatically includes this, too.
     **/
    void enterProximity();

    /**
     * Notification that a warning has been posted.
     **/
    void warningPostedNotify();

    /**
     * Check if the value is in range, i.e. if anybody from the outside has
     * called 'enterRange()' since the last call to 'clear()'.
     **/
    bool inRange() const;

    /**
     * Check if a warning should be posted, i.e. if the value is currently in
     * range ( see 'inRange() ) and there has been no notification yet that a
     * warning has already been posted.
     **/
    bool needWarning() const;

    /**
     * Check if the value is leaving the proximity range.
     **/
    bool leavingProximity() const;

    /*
     * Log settings to y2log
     */
    void logSettings() const;

    /**
     * Clear the current values, i.e. prepare for a new round of checks
     **/
    void clear();

    /**
     * Clear everything, including all history values such as if a warning has
     * been posted.
     **/
    void clearHistory();


protected:

    bool 	_inRange;
    bool	_isClose;
    bool	_hasBeenClose;
    bool 	_warningPosted;
};

class NCPkgPopupDiskspace : public NCPopup
{
private:
    NCTable * partitions;
    NCPushButton * okButton;
    NCLabel * head;

protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );

public:
    NCPkgPopupDiskspace( const wpos at, std::string headline );

    virtual ~NCPkgPopupDiskspace();

    void createLayout( std::string headline );
    void doit();

    NCTable *Partitions() { return partitions; }

    virtual int preferredWidth();
    virtual int preferredHeight();

};

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPopupDiskspace
//
//	DESCRIPTION :
//
class NCPkgDiskspace
{
    NCPkgDiskspace & operator=( const NCPkgDiskspace & );
    NCPkgDiskspace            ( const NCPkgDiskspace & );

private:

    bool testmode;
    NCPkgPopupDiskspace *popupWin;
    ZyppDuSet testDiskUsage;

    std::string usedPercent( const FSize &used, const FSize &total );

    /**
     * Warning range notifier about running out of disk space warning.
     **/
    NCPkgWarningRangeNotifier runningOutWarning;

    /**
     * Warning range notifier about disk space overflow warning.
     **/
    NCPkgWarningRangeNotifier overflowWarning;


public:

    NCPkgDiskspace(  bool testSpaceMode );

    virtual ~NCPkgDiskspace();


    void fillPartitionTable();

    std::string checkDiskSpace();

    void setDiskSpace( wint_t key );	// used for testing

    void checkDiskSpaceRange();

    void showInfoPopup( std::string headline );

    void checkRemainingDiskSpace( const ZyppPartitionDu & partition );

    FSize calculateDiff();
};


///////////////////////////////////////////////////////////////////


#endif // NCPkgPopupDiskspace_h
