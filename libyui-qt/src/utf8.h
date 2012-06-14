/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/


/*-/

  File:	      utf8.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef utf8_h
#define utf8_h

#include <qstring.h>
#include <string>
#include <iosfwd>


inline QString fromUTF8( const std::string & str )
{
    return QString::fromUtf8( str.c_str() );
}


inline std::string toUTF8( const QString & str )
{
    if ( str.isEmpty() )
	return std::string( "" );
    else
	return std::string( str.toUtf8().data() );
}


inline std::ostream & operator<<( std::ostream & stream, const QString & str )
{
    return stream << str.toUtf8().data();
}


#endif // utf8_h
