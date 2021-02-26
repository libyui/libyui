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

   File:       stdutil.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef stdutil_h
#define stdutil_h

#include <iosfwd>

#include <cstdio>
#include <cstdarg>

#include <string>
#include <vector>
#include <iostream>


namespace stdutil
{
    inline std::string vform( const char * format, va_list ap, va_list ap1 )
    {
	char * buf = new char[vsnprintf( NULL, 0, format, ap ) + 1];
	vsprintf( buf, format, ap1 );
	std::string val( buf );
	delete [] buf;
	return val;
    }

    inline std::string form( const char * format, ... )
	__attribute__(( format( printf, 1, 2 ) ) );

    inline std::string form( const char * format, ... )
    {
	va_list ap;
	va_list ap1;
	va_start( ap, format );
	va_start( ap1, format );
	std::string val( vform( format, ap, ap1 ) );
	va_end( ap );
	va_end( ap1 );
	return val;
    }

    inline std::string numstring( char n,	   int w = 0 ) { return form( "%*hhd", w, n ); }

    inline std::string numstring( unsigned char n, int w = 0 ) { return form( "%*hhu", w, n ); }

    inline std::string numstring( int n,	   int w = 0 ) { return form( "%*d",   w, n ); }

    inline std::string numstring( unsigned n,	   int w = 0 ) { return form( "%*u",   w, n ); }

    inline std::string numstring( long n,	   int w = 0 ) { return form( "%*ld",  w, n ); }

    inline std::string numstring( unsigned long n, int w = 0 ) { return form( "%*lu",  w, n ); }

    inline std::string hexstring( char n,	   int w = 4 ) { return form( "%#0*hhx", w, n ); }

    inline std::string hexstring( unsigned char n, int w = 4 ) { return form( "%#0*hhx", w, n ); }

    inline std::string hexstring( int n,	   int w = 10 ) { return form( "%#0*x",   w, n ); }

    inline std::string hexstring( unsigned n,	   int w = 10 ) { return form( "%#0*x",   w, n ); }

    inline std::string hexstring( long n,	   int w = 10 ) { return form( "%#0*lx",  w, n ); }

    inline std::string hexstring( unsigned long n, int w = 10 ) { return form( "%#0*lx",  w, n ); }

    inline std::string octstring( char n,	   int w = 4 ) { return form( "%#0*hho", w, n ); }

    inline std::string octstring( unsigned char n, int w = 4 ) { return form( "%#0*hho", w, n ); }

    inline std::string octstring( int n,	   int w = 0 ) { return form( "%#*o",	 w, n ); }

    inline std::string octstring( unsigned n,	   int w = 0 ) { return form( "%#*o",	 w, n ); }

    inline std::string octstring( long n,	   int w = 0 ) { return form( "%#*lo",	 w, n ); }

    inline std::string octstring( unsigned long n, int w = 0 ) { return form( "%#*lo",	 w, n ); }

} // namespace stdutil

#endif // stdutil_h
