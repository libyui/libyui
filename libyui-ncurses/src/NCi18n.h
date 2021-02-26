/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
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
|***************************************************************************/

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

  File:		NCi18n.h

  Authors:	Gabriele Strattner <gs@suse.de>

/-*/

#ifndef NCi18n_h
#define NCi18n_h

#include <libintl.h>


/**
 *  define _(...) for gettext
 *  msgid:  text which has to be translated
 *  return: NCstring
 */

inline string _( const char * msgid )
{
    return gettext( msgid );
}

/**
 * define _(...) for ngettext
 * msgid1: message singular
 * msgid2: message plural form
 * n:	   choose singular/plural form based on this value
 * return: NCstring
 */
inline string _( const char * msgid1, const char * msgid2, unsigned long int n )
{
    return ngettext( msgid1, msgid2, n );
}

inline void setTextdomain( const char * domain )
{
    bindtextdomain( domain, LOCALEDIR );
    bind_textdomain_codeset( domain, "utf8" );
    textdomain( domain );

    // Make change known
    {
	extern int _nl_msg_cat_cntr;
	++_nl_msg_cat_cntr;
    }
}


#endif // NCi18n_h
