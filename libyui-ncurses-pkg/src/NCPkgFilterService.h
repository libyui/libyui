/*
  Copyright (c) 2002-2011 Novell, Inc.
  Copyright (c) 2018-2021 SUSE LLC

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
*/


#ifndef NCPkgFilterService_h
#define NCPkgFilterService_h

#include <iosfwd>
#include <string>
#include <vector>
#include <algorithm>

#include <zypp/RepoManager.h>

#include <yui/ncurses/NCPadWidget.h>
#include <yui/ncurses/NCPopup.h>
#include <yui/ncurses/NCPushButton.h>
#include <yui/ncurses/NCTable.h>
#include <yui/ncurses/NCTablePad.h>

#include "NCZypp.h"


class NCTable;
class NCPushButton;
class NCPackageSelector;

// represent a service by its name
typedef std::string ZyppService;

class NCPkgServiceTag : public YTableCell
{

private:

    ZyppService service;

public:
    /**
      * A helper class to hold a reference to zypp::Service
      * for each service table line
      * (actually it's a dummy column of the table)
      * @param service zypp::Service reference
      */

    NCPkgServiceTag( ZyppService service );

    ~NCPkgServiceTag() {};

    /*
     * Get service reference from the line tag
     * @return ZyppService
     */

    ZyppService getService() const		{ return service; }

};

class NCPkgServiceTable : public NCTable
{
private:

    NCPkgServiceTable & operator=( const NCPkgServiceTable & );
    NCPkgServiceTable            ( const NCPkgServiceTable & );

    NCPackageSelector *packager;
    zypp::shared_ptr<zypp::RepoManager> repo_manager;

public:

    /**
      * A helper class to hold service data in a neat table
      * widget
      * @param parent A parent widget
      * @param tableHeader table header
      * @param pkg the master object
      */

    NCPkgServiceTable  ( YWidget *parent, YTableHeader *tableHeader, NCPackageSelector *pkg);

    virtual ~NCPkgServiceTable() {};

    /**
      * @return bool any service is present at all
      */
    static bool any_service();

    /**
      * Add one line (with tag) to the services table
      * @param ZyppService Reference to zypp::Service
      * @param cols String std::vector with service data (name + URL)
      */
    virtual void addLine( ZyppService r, const std::vector<std::string> & cols );

    /*
     * Fill header of services table (name + URL)
     */
    void fillHeader();

    /**
      * Get tag of service table line on current index,
      * (contains service reference)
      * @param index Index of selected table line
      * @return NCPkgServiceTag* Tag of selected line
      */
    NCPkgServiceTag * getTag ( int index );

    /**
     * Get service reference from selected line's tag
     * @param index Index of selected table line
     * @return ZyppService Associated zypp::Service reference
     */
    ZyppService getService( int index );

    /**
     * @return HTML contents for the description pane
     */
    std::string getDescription (ZyppService r);

    virtual NCursesEvent wHandleInput ( wint_t ch );

   /**
      * Add items to the service list (assoc.
      * product name, if any, and URL)
      * @return bool (always true;-) )
      */
    bool fillServiceList();

    /**
     * Make the Package List show the packages
     * for the currently selected service
     */
    void showServicePackages();

};
#endif
