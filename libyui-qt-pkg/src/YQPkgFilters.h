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

#ifndef YQPkgFilters_h
#define YQPkgFilters_h

#include "YQZypp.h"

/**
 * @short Zypp filtering helpers
 **/
class YQPkgFilters
{

public:

    /**
     * Returns the product if the filter finds a single product
     * or null product if there are no or multiple products.
      * @param  filter filtering function, the parameter is the product pool item,
      *     returning true if it matches the expectations
      * @return      Found zypp product or null
      */
    static ZyppProduct singleProductFilter( std::function<bool(const zypp::PoolItem& item)> filter);

};

#endif // ifndef YQPkgFilters_h
