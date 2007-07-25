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

  File:		YQi18n.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQi18n_h
#define YQi18n_h

#include <libintl.h>
#include <qstring.h>


inline QString _( const char * msgid )
{
	return ( !msgid || !*msgid ) ? "" : QString::fromUtf8( gettext(msgid ) );
}

inline QString _( const char * msgid1, const char * msgid2, unsigned long int n )
{
	return QString::fromUtf8( ngettext(msgid1, msgid2, n ) );
}


#endif // YQi18n_h
