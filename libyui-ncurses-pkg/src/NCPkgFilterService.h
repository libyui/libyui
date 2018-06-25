/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| Copyright (c) [2018] SUSE LLC
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
| along with this program; if not, contact SUSE.
|
| To contact SUSE about this file by physical or electronic mail,
| you may find current contact information at www.suse.com
|
|***************************************************************************/

#ifndef NCPkgFilterService_h
#define NCPkgFilterService_h

#include <iosfwd>

#include <vector>
#include <string>
#include <algorithm>

#include "NCPadWidget.h"
#include "NCPopup.h"
#include "NCPushButton.h"
#include "NCTable.h"
#include "NCTablePad.h"
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

    NCPkgServiceTag ( ZyppService service);

    ~NCPkgServiceTag() {  };

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
public:

    /**
      * A helper class to hold service data in a neat table
      * widget
      * @param parent A parent widget
      * @param opt Widget options
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

    std::string showDescription (ZyppService r);

    virtual NCursesEvent wHandleInput ( wint_t ch );

    /**
      * Find single zypp::Product for this service
      * (null product if multiple products found)
      * @param service zypp::Service
      * @return ZyppProduct
      */

    ZyppProduct findProductForService (ZyppService service);

   /**
      * Add items to the service list (assoc.
      * product name, if any, and URL)
      * @return bool (always true ;-) )
      */
    bool fillServiceList( );

    bool showServicePackages( );

};
#endif
