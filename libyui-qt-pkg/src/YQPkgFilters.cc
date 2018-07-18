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

Textdomain "qt-pkg"

/-*/

#include <algorithm>

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include <zypp/ResPool.h>
#include <zypp/PoolItem.h>

#include "YQPkgFilters.h"


ZyppProduct
YQPkgFilters::singleProductFilter(std::function<bool(const zypp::PoolItem& item)> filter)
{
    ZyppProduct product;

    auto it = zypp::ResPool::instance().byKindBegin( zypp::ResKind::product );
    auto end = zypp::ResPool::instance().byKindEnd( zypp::ResKind::product );

    // Find the first product
    auto product_it = std::find_if(it, end, [&](const zypp::PoolItem& item) {
        return filter(item);
    });

    if (product_it == end)
    {
        yuiMilestone() << "No product found " << std::endl;
        return product;
    }

    product = zypp::asKind<zypp::Product>( product_it->resolvable() );
    yuiMilestone() << "Found product " << product->name() << std::endl;

    // Check if there is another product
    product_it = std::find_if(++product_it, end, [&](const zypp::PoolItem& item) {
        return filter(item);
    });

    if (product_it == end)
        return product;

    product = zypp::asKind<zypp::Product>( product_it->resolvable() );
    yuiMilestone() << "Found another product " << product->name() << std::endl;

    // nullptr
    return ZyppProduct();
}
