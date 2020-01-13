/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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

  File:		YQPkgClassFilterView.h

  Authors:	Duncan Mac-Vicar Prett <duncan@suse.de>
		Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgClassFilterView_h
#define YQPkgClassFilterView_h

#include <map>
#include "YQZypp.h"
#include "YRpmGroupsTree.h"
#include <QTreeWidget>

using std::string;

typedef enum
{
    YPKG_GROUP_SUGGESTED,
    YPKG_GROUP_RECOMMENDED,
    YPKG_GROUP_ORPHANED,
    YPKG_GROUP_UNNEEDED,
    YPKG_GROUP_MULTIVERSION,
    YPKG_GROUP_RETRACTED,
    YPKG_GROUP_RETRACTED_INSTALLED,
    YPKG_GROUP_ALL,
} YPkgGroupEnum;


class YQPkgClassItem;


/**
 * Filter view for PackageKit groups. Uses the packages' RPM group tags and
 * maps them to the corresponding PackageKit group.
 **/
class YQPkgClassFilterView : public QTreeWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgClassFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgClassFilterView();

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgClassItem * selection() const;

    /**
     * Check if 'pkg' matches the selected group.
     * Returns true if there is a match, false otherwise.
     **/
    bool check( ZyppSel	selectable,
		ZyppPkg pkg );

    /**
     * Returns the (untranslated!) currently selected group enum
     **/
    YPkgGroupEnum selectedGroup() const { return _selectedGroup; }

public slots:

    /**
     * Filter according to the view's rules and current selection.
     * Emits those signals:
     *	  filterStart()
     *	  filterMatch() for each pkg that matches the filter
     *	  filterFinished()
     **/
    void filter();

    /**
     * Same as filter(), but only if this widget is currently visible.
     **/
    void filterIfVisible();

    /**
     * Select a list entry (if there is any).
     * Usually this will be the first list entry, but don't rely on that - this
     * might change without notice. Emits signal currentItemChanged().
     **/
    void selectSomething();

signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter.
     **/
    void filterMatch( ZyppSel	selectable,
		      ZyppPkg	pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected slots:

    void slotSelectionChanged( QTreeWidgetItem * newSelection );


protected:

    void fillGroups();

    //
    // Data members
    //

    YPkgGroupEnum _selectedGroup;
    std::map<YPkgGroupEnum, YQPkgClassItem *> _groupsMap;

    // map to cache converted groups
    std::map<std::string, YPkgGroupEnum> _groupsCache;
};


class YQPkgClassItem: public QTreeWidgetItem
{
public:

    YQPkgClassItem( YQPkgClassFilterView * parentFilterView,
		    YPkgGroupEnum group );
    virtual ~YQPkgClassItem();

    YQPkgClassFilterView * filterView() const { return _filterView; }
    YPkgGroupEnum group() const { return _group; }
    virtual bool operator< ( const QTreeWidgetItem & otherListViewItem ) const;


private:

    // Data members

    YQPkgClassFilterView * _filterView;
    YPkgGroupEnum	   _group;
};


#endif // ifndef YQPkgClassFilterView_h
