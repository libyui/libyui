/**************************************************************************
Copyright (C) 2018 SUSE LLC
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

*/


#ifndef YQPkgServiceList_h
#define YQPkgServiceList_h

#include <string>
#include "YQZypp.h"
#include "QY2ListView.h"
#include <zypp/Product.h>


class YQPkgServiceListItem;
namespace zypp {
    class RepoManager;
}

// just a service name
typedef std::string	ZyppService;


/**
 * @short Display a list of libzypp services.
 **/
class YQPkgServiceList : public QY2ListView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgServiceList( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgServiceList();


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
     * Add a service to the list.
     **/
    void addService( ZyppService service, const zypp::RepoManager &mgr );


public:

    // Column numbers
    int nameCol()	const	{ return _nameCol;	}

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgServiceListItem * selection() const;


signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter
     * and the candidate package comes from the respective repository
     **/
    void filterMatch( ZyppSel	selectable,
		      ZyppPkg	pkg );

    /**
     * Emitted during filtering for each pkg that matches the filter
     * and the candidate package does not come from the respective repository
     **/
    void filterNearMatch( ZyppSel	selectable,
			  ZyppPkg	pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected slots:

    /**
     * Fill the list.
     **/
    void fillList();


private:

    //
    // Data members
    //

    int	_nameCol;
};



class YQPkgServiceListItem: public QY2ListViewItem
{
public:

    /**
     * Constructor
     **/
    YQPkgServiceListItem( YQPkgServiceList *parentList, ZyppService service, const zypp::RepoManager &mgr );

    /**
     * Destructor
     **/
    virtual ~YQPkgServiceListItem();

    /**
     * Returns the ZYPP service this item corresponds to (its alias)
     **/
    ZyppService zyppService() const { return _zyppService; }

    /**
     * Returns the ZYPP service name this item corresponds to
     **/
    std::string zyppServiceName() const { return _zyppServiceName; }

    /**
     * Returns the parent list
     **/
    const YQPkgServiceList * serviceList() const { return _serviceList; }

    /**
     * Returns the product on a source if it has one single product
     * or 0 if there are no or multiple products.
     **/
    static ZyppProduct singleProduct( ZyppService service );


    // Columns

    int nameCol()	const	{ return _serviceList->nameCol();	}

    virtual bool operator< ( const QTreeWidgetItem & other ) const;
protected:

    // Data members

    YQPkgServiceList *		_serviceList;
    // the zypp service
    ZyppService			_zyppService;
    std::string _zyppServiceName;
};


#endif // ifndef YQPkgServiceList_h
