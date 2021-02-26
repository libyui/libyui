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

   File:       lang2encoding.cc

   Author:     auto-generated

/-*/

#include <string>

std::string language2encoding( std::string lang )
{
  using std::string;
  lang = ":" + lang + ":";
  if ( std::string( ":ca:da:de:en:es:fi:fr:gl:is:it:nl:no:pt:sv:" ).find( lang ) != std::string::npos )
    return "ISO-8859-1";
  else if ( std::string( ":lt:" ).find( lang ) != std::string::npos )
    return "ISO-8859-13";
  else if ( std::string( ":cs:hr:hu:pl:ro:sk:sl:" ).find( lang ) != std::string::npos )
    return "ISO-8859-2";
  else if ( std::string( ":ru:" ).find( lang ) != std::string::npos )
    return "ISO-8859-5";
  else if ( std::string( ":el:" ).find( lang ) != std::string::npos )
    return "ISO-8859-7";
  else if ( std::string( ":iw:" ).find( lang ) != std::string::npos )
    return "ISO-8859-8";
  else if ( std::string( ":tr:" ).find( lang ) != std::string::npos )
    return "ISO-8859-9";
  else if ( std::string( ":ja:" ).find( lang ) != std::string::npos )
    return "eucJP";
  return "";
}
//-----------------------------------------------------------------
