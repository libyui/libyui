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
     **/
    YQPkgDiskUsageList( QWidget *parent );

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

    
public slots:
    /**
     * Update all statistical data in the list.
     **/
    void updateDiskUsage();


protected:

    // Data members

    QAsciiDict<YQPkgDiskUsageListItem> _items;
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

    YQPkgDuData _duData;
};



#endif // ifndef YQPkgDiskUsageList_h
