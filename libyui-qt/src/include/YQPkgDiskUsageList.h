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

  File:	      YQPkgDiskUsageList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgDiskUsageList_h
#define YQPkgDiskUsageList_h

#include <QY2DiskUsageList.h>
#include <y2pm/PkgDu.h>
#include <qasciidict.h>

class YQPkgDiskUsageListItem;
typedef PkgDuMaster::MountPoint YQPkgDuData;


/**
 * @short List of disk usage of all attached partitions.
 **/
class YQPkgDiskUsageList : public QY2DiskUsageList
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * 'thresholdPercent' can be used to include only partitions with at least
     * this many percent used disk space in the list. This is useful for
     * warning dialogs (only?).
     **/
    YQPkgDiskUsageList( QWidget *parent, int thresholdPercent = 0 );

    /**
     * Destructor.
     **/
    virtual ~YQPkgDiskUsageList() {}

    /**
     * For debugging: Add some fake data.
     **/
    void fakeData();

    /**
     * Suggest reasonable default size.
     *
     * Reimplemented from QListView.
     **/
    virtual QSize sizeHint() const;

    /**
     * Returns the percentage threshold at which to warn about running out of
     * disk space.
     **/
    int warningPercentThreshold() const { return _warningPercentThreshold; }

    /**
     * Returns the free size threshold at which to warn about running out of
     * disk space. This is meant to prevent warnings about file systems being
     * 90% full when there are still Gigabytes of free space left.
     **/
    FSize freeSizeThreshold() const { return _freeSizeThreshold; }

    /**
     * Notify the list that a warning about disk space is necessary.
     * This does _not_ immediately post a warning dialog.
     **/
    void warningNotify() { _warn = true; }

    /**
     * Notify the list about file system overflow.
     * This does _not_ immediately post a warning dialog.
     **/
    void overflowNotify() { _overflow = true; }

    /**
     * Return 'true' if one or more file system are overflowing.
     **/
    bool overflow() const { return _overflow; }


public slots:

    /**
     * Update all statistical data in the list.
     **/
    void updateDiskUsage();


protected:

    /**
     * Event handler for keyboard input - for debugging and testing.
     * Changes the current item's percentage on the fly.
     *
     * Reimplemented from QListView / QWidget.
     */
    virtual void keyPressEvent( QKeyEvent * ev );


    // Data members

    QAsciiDict<YQPkgDiskUsageListItem>  _items;
    bool				_debug;
    bool				_warn;
    bool				_overflow;
    int					_warningPercentThreshold;
    FSize				_freeSizeThreshold;
};



class YQPkgDiskUsageListItem: public QY2DiskUsageListItem
{
public:

    /**
     * Constructor. Creates a YQPkgDiskUsageList item that corresponds to the
     * specified file system.
     **/
    YQPkgDiskUsageListItem( YQPkgDiskUsageList * 	parent,
			    YQPkgDuData			duData);

    /**
     * Destructor.
     **/
    virtual ~YQPkgDiskUsageListItem() {}

    /**
     * Returns the corresponding disk usage data.
     **/
    YQPkgDuData duData() const { return _duData; }

    /**
     * Update the disk usage data.
     **/
    void updateDuData( const YQPkgDuData & fromData );

    /**
     * The currently used size of this partition.
     *
     * Reimplemented from QY2DiskUsageListItem.
     **/
    virtual FSize usedSize() const;

    /**
     * The total size of this partition.
     *
     * Reimplemented from QY2DiskUsageListItem.
     **/
    virtual FSize totalSize() const;

    /**
     * The name to display for this partition (the mount point).
     *
     * Reimplemented from QY2DiskUsageListItem.
     **/
    virtual QString name() const;

    /**
     * The device name of this partition.
     *
     * Reimplemented from QY2DiskUsageListItem.
     **/
    virtual QString deviceName() const { return ""; }


protected:

    // Data members

    YQPkgDuData 		_duData;
    YQPkgDiskUsageList *	_pkgDiskUsageList;
};


#if 0
// EXPERIMENTAL
// EXPERIMENTAL
// EXPERIMENTAL
class YQPkgWarningRange
{
public:
    /**
     * Constructor.
     **/
    YQPkgWarningRange( int min, int max );

    bool check( int val );

    void warningPosted();
    

protected:

    int		_min;
    int 	_max;
    bool 	_inRange;
    bool 	_warningPosted;
};
// EXPERIMENTAL
// EXPERIMENTAL
#endif



#endif // ifndef YQPkgDiskUsageList_h
