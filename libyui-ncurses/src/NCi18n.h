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

// -*- c++ -*-

#ifndef NCi18n_h
#define NCi18n_h

#include <libintl.h>

#include "NCstring.h"


/**
 *  define _(...) for gettext
 *  @param msgid	text which has to be translated
 *  @return NCstring
 */
NCstring _(const char * msgid)
{
    NCstring str( "" );

    return str.assignUtf8( gettext(msgid) );
}

/**
 * define _(...) for ngettext
 * @param msgid1 	message singular
 * @param msgid2	message plural form
 * @param n 	choose singular/plural form based on this value
 * @return NCstring
 */
NCstring _(const char * msgid1, const char * msgid2, unsigned long int n)
{
    NCstring str( "" );

    return str.assignUtf8( ngettext(msgid1, msgid2, n) );
}


#endif // NCi18n_h
