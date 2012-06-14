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
