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


#ifndef CyclicContainer_h
#define CyclicContainer_h

#include <algorithm>
#include <iterator>
#include <vector>

template <class T>
class CyclicContainer
{
public:

    using Iterator = typename std::vector<T *>::iterator;
    using ReverseIterator = std::reverse_iterator<Iterator>;

    Iterator begin() { return _elements.begin(); }
    Iterator end() { return _elements.end(); }

    CyclicContainer() : _elements(), _current( nullptr )
    {}


    ~CyclicContainer()
    {
	clear();
    }


    void clear()
    {
	_elements.clear();
	_current = nullptr;
    }


    void add( T * element )
    {
	_elements.push_back(element);
    }


    void setCurrent(Iterator element)
    {
	if ( element != _elements.end() )
	    _current = *element;
    }


    Iterator current()
    {
	return std::find( _elements.begin(), _elements.end(), _current );
    }


    Iterator next()
    {
	Iterator current = this->current();

	if ( current == _elements.end() )
	    return findNext( _elements.begin() );

	Iterator next = findNext( std::next( current, 1 ) );

	if ( next == _elements.end() )
	    return findNext( _elements.begin() );

	return next;
    }


    Iterator previous()
    {
	Iterator current = this->current();

	ReverseIterator rbegin;

	if ( current == _elements.end() )
	    rbegin = _elements.rbegin();
	else
	    rbegin = ReverseIterator( current );

	ReverseIterator previous = findPrevious( rbegin );

	if ( previous == _elements.rend() && rbegin != _elements.rbegin() )
	    previous = findPrevious( _elements.rbegin() );

	if ( previous == _elements.rend() )
	    return _elements.end();

	return find( _elements.begin(), _elements.end(), *previous );
    }

protected:

    Iterator findNext( Iterator begin )
    {
	return find_if( begin, _elements.end(), [](const T * element) {
	    return element->isSelectable();
	});
    }


    ReverseIterator findPrevious( ReverseIterator rbegin )
    {
	return find_if( rbegin, _elements.rend(), [](const T * element) {
	    return element->isSelectable();
	});
    }

private:

    std::vector<T *> _elements;

    T * _current;
};

#endif // CyclicContainer_h
