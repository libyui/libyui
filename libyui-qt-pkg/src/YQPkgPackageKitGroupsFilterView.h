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

  File:		YQPkgPackageKitGroupsFilterView.h

  Authors:	Duncan Mac-Vicar Prett <duncan@suse.de>
		Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgPackageKitGroupsFilterView_h
#define YQPkgPackageKitGroupsFilterView_h

#include <map>
#include "YQZypp.h"
#include "YRpmGroupsTree.h"
#include <QTreeWidget>

using std::string;

typedef enum
{
    //
    // PackageKit values
    //
    
    PK_GROUP_ENUM_ACCESSIBILITY,
    PK_GROUP_ENUM_ACCESSORIES,
    PK_GROUP_ENUM_EDUCATION,
    PK_GROUP_ENUM_GAMES,
    PK_GROUP_ENUM_GRAPHICS,
    PK_GROUP_ENUM_INTERNET,
    PK_GROUP_ENUM_OFFICE,
    PK_GROUP_ENUM_OTHER,
    PK_GROUP_ENUM_PROGRAMMING,
    PK_GROUP_ENUM_MULTIMEDIA,
    PK_GROUP_ENUM_SYSTEM,
    PK_GROUP_ENUM_DESKTOP_GNOME,
    PK_GROUP_ENUM_DESKTOP_KDE,
    PK_GROUP_ENUM_DESKTOP_XFCE,
    PK_GROUP_ENUM_DESKTOP_OTHER,
    PK_GROUP_ENUM_PUBLISHING,
    PK_GROUP_ENUM_SERVERS,
    PK_GROUP_ENUM_FONTS,
    PK_GROUP_ENUM_ADMIN_TOOLS,
    PK_GROUP_ENUM_LEGACY,
    PK_GROUP_ENUM_LOCALIZATION,
    PK_GROUP_ENUM_VIRTUALIZATION,
    PK_GROUP_ENUM_SECURITY,
    PK_GROUP_ENUM_POWER_MANAGEMENT,
    PK_GROUP_ENUM_COMMUNICATION,
    PK_GROUP_ENUM_NETWORK,
    PK_GROUP_ENUM_MAPS,
    PK_GROUP_ENUM_REPOS,
    PK_GROUP_ENUM_UNKNOWN,

    //
    // Other values
    //
    
    YPKG_GROUP_SUGGESTED,
    YPKG_GROUP_RECOMMENDED,
    YPKG_GROUP_ORPHANED,
    YPKG_GROUP_ALL,
} YPkgGroupEnum;


class YQPkgPackageKitGroup;


/**
 * Filter view for PackageKit groups. Uses the packages' RPM group tags and
 * maps them to the corresponding PackageKit group. 
 **/
class YQPkgPackageKitGroupsFilterView : public QTreeWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgPackageKitGroupsFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgPackageKitGroupsFilterView();

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgPackageKitGroup * selection() const;

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
     *    filterStart()
     *    filterMatch() for each pkg that matches the filter
     *    filterFinished()
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
    std::map<YPkgGroupEnum, YQPkgPackageKitGroup *> _groupsMap;
    
    // map to cache converted groups
    std::map<std::string, YPkgGroupEnum> _groupsCache;
};


class YQPkgPackageKitGroup: public QTreeWidgetItem
{
public:

    YQPkgPackageKitGroup( YQPkgPackageKitGroupsFilterView * parentFilterView,
			  YPkgGroupEnum group );
    virtual ~YQPkgPackageKitGroup();

    YQPkgPackageKitGroupsFilterView * filterView() const { return _filterView; }
    YPkgGroupEnum group() const { return _group; }
    virtual bool operator< ( const QTreeWidgetItem & otherListViewItem ) const;

    
private:

    // Data members

    YQPkgPackageKitGroupsFilterView *	_filterView;
    YPkgGroupEnum 			_group;
};


#endif // ifndef YQPkgPackageKitGroupsFilterView_h
