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
#include <yui/YItem.h>

class NCTableLine;


/**
 * Support classes for sorting by column in a table for use in an NCTablePad
 **/


/**
 * Abstract base class for sorting strategies.
 **/
class NCTableSortStrategyBase
{
public:

    NCTableSortStrategyBase( int sortCol = 0, bool reverse = false )
        : _sortCol( sortCol )
        , _reverse( reverse )
        {}

    virtual ~NCTableSortStrategyBase()
        {}

    /**
     * Sort items between 'begin' and 'end' in place.
     *
     * Derived classes are required to implement this.
     **/
    virtual void sort( YItemIterator begin, YItemIterator end ) = 0;


    int  sortCol() const                { return _sortCol;    }
    void setSortCol( int col )          { _sortCol = col;     }

    bool reverse() const		{ return _reverse;    }
    void setReverse( bool reverse )	{ _reverse = reverse; }


private:

    int  _sortCol;
    bool _reverse;
};


/**
 * Default sort strategy
 **/
class NCTableSortDefault: public NCTableSortStrategyBase
{
public:
    virtual void sort( YItemIterator begin, YItemIterator end ) override;

private:

    /**
     * Comparison functor.
     *
     * This uses the sort key of the cell if it has one, the label if not.
     *
     * It also tries to convert strings to numbers to do a numeric comparison
     * if possible.
     **/
    class Compare
    {
    public:
	Compare( int sortCol, bool reverse )
	    : _sortCol( sortCol )
            , _reverse( reverse )
	    {}

        /**
         * The comparison itself: Return the result of  item1 < item2
         **/
	bool operator() ( YItem * item1, YItem * item2 ) const;

    protected:

        /**
         * Return the sort key of column no. _sortCol for an item or, if it
         * doesn't have one, its label in that column.
         **/
	std::wstring smartSortKey( YItem * item ) const;

        /**
         * Try to convert a string to a number. Return the number and set the
         * 'ok' flag to 'true' on success, to 'false' on failure.
         **/
	long long toNumber( const std::wstring& str, bool * ok ) const;


        // Data members

	const int  _sortCol;
	const bool _reverse;
    };

};


#endif  // NCTableSort_h
