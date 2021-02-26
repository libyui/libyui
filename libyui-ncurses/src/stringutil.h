/*
  Copyright (C) 2000-2012 Novell, Inc
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


/*-/

   File:       stringutil.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef stringutil_h
#define stringutil_h

#include <iosfwd>
#include <sstream>

#include <string>
#include <vector>

namespace strutil
{
    /** StringStream with autoconversion to std::string. */
    struct StrStr
    {
	template<class _Tp>
	StrStr & operator<<( const _Tp & val )
	{ _str << val; return *this; }

	operator std::string() const
	{ return _str.str(); }

	std::ostringstream _str;
    };


    extern unsigned split( const std::string	      line_tv,
			   std::vector<std::string> & words_Vtr,
			   const std::string	      sep_tv = " \t",
			   const bool		      singlesep_bv = false );

} // namespace strutil

namespace std
{
  ostream & operator<<( ostream &, const wstring & );
}

#endif // stringutil_h
