/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

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
