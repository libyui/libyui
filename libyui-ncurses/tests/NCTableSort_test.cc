/*
  Copyright (C) 2021 SUSE LLC

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

#define BOOST_TEST_MODULE NCTableSort_tests
#include <boost/test/unit_test.hpp>

#include "NCTableSort.h"

// To test a protected method, derive a child that makes it public.
// But its _class_ is protected too, so expose the class first...
class NCTableSortDefaultExposed : public NCTableSortDefault
{
public:
    using NCTableSortDefault::Compare;
};

// ... and now expose the method
class CompareExposed : public NCTableSortDefaultExposed::Compare {
public:
    CompareExposed(int sort_column, bool reverse)
	: Compare(sort_column, reverse)
        {}
    using Compare::toNumber;
};

BOOST_AUTO_TEST_CASE(toNumber)
{
    static const int sort_column = 0;
    static const bool reverse = false;
    CompareExposed cmp(sort_column, reverse);
    bool ok;
    
    BOOST_CHECK_EQUAL(cmp.toNumber(L"1", &ok), 1LL);
    BOOST_CHECK_EQUAL(ok, true);

    BOOST_CHECK_EQUAL(cmp.toNumber(L"1half", &ok), 1LL);
    BOOST_CHECK_EQUAL(ok, false);

    BOOST_CHECK_EQUAL(cmp.toNumber(L"half", &ok), 0LL);
    BOOST_CHECK_EQUAL(ok, false);
}
