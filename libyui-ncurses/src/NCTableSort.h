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


#ifndef NCTableSort_h
#define NCTableSort_h

#include <string>
#include <vector>

class NCTableLine;


/**
 * Support classes for sorting by column in a table for use in an NCTablePad
 **/

/// Just a pair: column number, reverse flag.
class NCTableSortStrategyBase
{
public:

    NCTableSortStrategyBase()
        : _column(0)
        , _reverse(false)
        {}

    NCTableSortStrategyBase( int column )
        : _column(column)
        , _reverse(false)
        {}

    virtual ~NCTableSortStrategyBase()
        {}

    virtual void sort( std::vector<NCTableLine *>::iterator itemsBegin,
		       std::vector<NCTableLine *>::iterator itemsEnd ) = 0;

    int  getColumn() const		{ return _column; }
    void setColumn( int column )	{ _column = column; }

    bool isReverse() const		{ return _reverse; }
    void setReverse( bool reverse )	{ _reverse = reverse; }

private:

    int  _column;
    bool _reverse;

};


class NCTableSortDefault : public NCTableSortStrategyBase
{
public:
    /// sort in place
    virtual void sort( std::vector<NCTableLine *>::iterator itemsBegin,
                       std::vector<NCTableLine *>::iterator itemsEnd ) override;

private:
    class Compare
    {
    public:
	Compare( int column, bool reverse )
	    : column(column)
            , reverse(reverse)
	    {}

	bool operator() ( const NCTableLine * first,
			  const NCTableLine * second ) const;

    private:

	// if available returns the sort key otherwise the first line of the label
	std::wstring smartSortKey( const NCTableLine * tableLine ) const;

	long long toNumber(const std::wstring& s, bool* ok) const;

	const int  column;
	const bool reverse;
    };

};


#endif  // NCTableSort_h
