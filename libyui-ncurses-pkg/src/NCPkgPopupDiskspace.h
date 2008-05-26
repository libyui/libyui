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

   File:       NCPkgPopupDiskspace.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPkgPopupDiskspace_h
#define NCPkgPopupDiskspace_h

#include <iosfwd>

#include <vector>
#include <string>

#include <FSize.h>
#include "NCPopup.h"
#include "NCLabel.h"

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

class NCPkgPopupDiskspace : public NCPopup {

private:
    NCTable * partitions;
    NCPushButton * okButton;
    NCLabel * head;

protected:

    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
  
public:
    NCPkgPopupDiskspace( const wpos at, string headline );

    virtual ~NCPkgPopupDiskspace();

    void createLayout( string headline );
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
class NCPkgDiskspace {

    NCPkgDiskspace & operator=( const NCPkgDiskspace & );
    NCPkgDiskspace            ( const NCPkgDiskspace & );

private:

    bool testmode;
    NCPkgPopupDiskspace *popupWin;
    ZyppDuSet testDiskUsage;

    string usedPercent( FSize used, FSize total );

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

    string checkDiskSpace();

    void setDiskSpace( wint_t key );	// used for testing
    
    void checkDiskSpaceRange( );
    
    void showInfoPopup( string headline );
 
    void checkRemainingDiskSpace( const ZyppPartitionDu & partition );
};


///////////////////////////////////////////////////////////////////


#endif // NCPkgPopupDiskspace_h
