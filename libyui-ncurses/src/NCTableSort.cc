/*
  Copyright (C) 2020 SUSE LLC
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


#define  YUILogComponent "ncurses"
#include <yui/YUILog.h>
#include <yui/YTableItem.h>

#include "NCTableSort.h"
#include "NCTable.h"


/**
 * Support classes for sorting by column in a table for use in an NCTablePad
 **/

void
NCTableSortDefault::sort( YItemIterator begin, YItemIterator end )
{
    // yuiMilestone() << "Sorting by col #" << sortCol()
    //                << " reverse: " << std::boolalpha << reverse() << endl;

    std::stable_sort( begin, end,
                      Compare( sortCol(), reverse() ) );
}


bool
NCTableSortDefault::Compare::operator() ( YItem * item1,
					  YItem * item2 ) const
{
    std::wstring w1 = smartSortKey( item1 );
    std::wstring w2 = smartSortKey( item2 );

    bool ok1, ok2;
    long long number1 = toNumber( w1, &ok1 );
    long long number2 = toNumber( w2, &ok2 );

    if ( ok1 && ok2 )
    {
	// Both are numbers
	return !_reverse ? number1 < number2 : number1 > number2;
    }
    else if ( ok1 && !ok2 )
    {
	// int < string
	return true;
    }
    else if ( !ok1 && ok2 )
    {
	// string > int
	return false;
    }
    else
    {
	// compare strings using collating information
	int result = std::wcscoll( w1.c_str(), w2.c_str() );

	return !_reverse ? result < 0 : result > 0;
    }
}


long long
NCTableSortDefault::Compare::toNumber( const std::wstring & str, bool * ok ) const
{
    try
    {
	*ok = true;
	return std::stoll( str );
    }
    catch (...)
    {
	*ok = false;
	return 0;
    }
}


std::wstring
NCTableSortDefault::Compare::smartSortKey( YItem * item ) const
{
    std::wstring empty;

    if ( ! item )
        return empty;

    YTableItem * tableItem = dynamic_cast<YTableItem *>( item );

    if ( ! tableItem )
        return empty;

    YTableCell * tableCell = tableItem->cell( _sortCol );

    if ( ! tableCell )
        return empty;

    NCstring result;

    if ( tableCell->hasSortKey() )
        result = NCstring( tableCell->sortKey() );
    else
        result = NCstring( tableCell->label() );

    return result.str();
}

