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


#include "NCTableSort.h"
#include "NCTable.h"


/**
 * Support classes for sorting by column in a table for use in an NCTablePad
 **/


void
NCTableSortDefault::sort ( std::vector<NCTableLine *>::iterator itemsBegin,
			   std::vector<NCTableLine *>::iterator itemsEnd )
{
    std::sort( itemsBegin, itemsEnd, Compare( getColumn(), isReverse() ) );
}


bool
NCTableSortDefault::Compare::operator() ( const NCTableLine * first,
					  const NCTableLine * second ) const
{
    std::wstring w1 = smartSortKey( first );
    std::wstring w2 = smartSortKey( second );

    bool ok1, ok2;
    long long number1 = toNumber( w1, &ok1 );
    long long number2 = toNumber( w2, &ok2 );

    if ( ok1 && ok2 )
    {
	// both are numbers
	return !reverse ? number1 < number2 : number1 > number2;
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

	return !reverse ? result < 0 : result > 0;
    }
}


long long
NCTableSortDefault::Compare::toNumber( const std::wstring& s, bool* ok ) const
{
    try
    {
	*ok = true;
	return std::stoll(s);
    }
    catch (...)
    {
	*ok = false;
	return 0;
    }
}


std::wstring
NCTableSortDefault::Compare::smartSortKey( const NCTableLine * tableLine ) const
{
    const YTableCell* tableCell = tableLine->origItem()->cell( column );

    if (tableCell->hasSortKey())
	return NCstring(tableCell->sortKey()).str();
    else
	return tableLine->GetCol( column )->Label().getText().begin()->str();
}

