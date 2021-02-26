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

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef utf8_h
#define utf8_h

#include <qstring.h>
#include <string>

inline QString fromUTF8(const string &str)
{
    return QString::fromUtf8(str.c_str() );
}


inline string toUTF8(const QString &str)
{
    QCString result = str.isEmpty() ? QCString("") : str.utf8();
    return string(result.data() );
}

#endif // utf8_h
