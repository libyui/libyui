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

  File:		NCi18n.h

  Authors:	Gabriele Strattner <gs@suse.de>

/-*/

#ifndef NCi18n_h
#define NCi18n_h

#include <libintl.h>

#include <yui/Libyui_config.h>


/**
 *  define _(...) for gettext
 *  msgid:  text which has to be translated
 *  return: NCstring
 */

inline std::string _( const char * msgid )
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
inline std::string _( const char * msgid1, const char * msgid2, unsigned long int n )
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
