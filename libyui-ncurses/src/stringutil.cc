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

#include <ostream>
#include "stringutil.h"
#include "NCstring.h"


unsigned strutil::split( const std::string	  line_tv,
			 std::vector<std::string> & words_Vtr,
			 const std::string	  sep_tv,
			 const bool	  singlesep_bv )
{
    words_Vtr.clear();

    if ( line_tv.empty() )
	return words_Vtr.size();

    struct sepctrl
    {
	const std::string & sep_t;
	sepctrl( const std::string & sep_tv ) : sep_t( sep_tv ) {}

	// Note that '\0' ist neither Sep nor NonSep
	inline bool isSep( const char c )    const { return( sep_t.find( c ) != std::string::npos ); }

	inline bool isNonSep( const char c )	const { return( c && !isSep( c ) ); }

	inline void skipSep( const char *& p ) const { while ( isSep( *p ) ) ++p; }

	inline void skipNonSep( const char *& p ) const { while ( isNonSep( *p ) ) ++p; }
    };

    sepctrl	 sep_Ci( sep_tv );

    const char * s_pci = line_tv.c_str();

    const char * c_pci = s_pci;

    // Start with c_pci at the beginning of the 1st field to add.
    // In singlesep the beginning might be equal to the next sep,
    // which makes an empty field before the sep.
    if ( !singlesep_bv && sep_Ci.isSep( *c_pci ) )
    {
	sep_Ci.skipSep( c_pci );
    }

    for ( s_pci = c_pci; *s_pci; s_pci = c_pci )
    {
	sep_Ci.skipNonSep( c_pci );
	words_Vtr.push_back( std::string( s_pci, c_pci - s_pci ) );

	if ( *c_pci )
	{
	    if ( singlesep_bv )
	    {
		if ( !*( ++c_pci ) )
		{
		    // line ends with a sep -> add the empty field behind
		    words_Vtr.push_back( "" );
		}
	    }
	    else
		sep_Ci.skipSep( c_pci );
	}
    }

    return words_Vtr.size();
}

namespace std
{
    ostream & operator<<( ostream & stream, const wstring & text )
    {
	string utf8text;
	NCstring::RecodeFromWchar( text, "UTF-8", &utf8text );

	return stream << utf8text;
    }
}
