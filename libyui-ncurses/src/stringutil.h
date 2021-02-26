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

   File:       stringutil.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef stringutil_h
#define stringutil_h

#include <iosfwd>

#include <string>
#include <vector>


namespace strutil
{

    extern unsigned split( const std::string	      line_tv,
			   std::vector<std::string> & words_Vtr,
			   const std::string	      sep_tv = " \t",
			   const bool		      singlesep_bv = false );

} // namespace strutil

std::ostream & operator<<( std::ostream &, const std::wstring & );


#endif // stringutil_h
