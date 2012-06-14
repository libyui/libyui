/*
  Copyright (C) 1970-2012 Novell, Inc
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
  if ( string( ":ca:da:de:en:es:fi:fr:gl:is:it:nl:no:pt:sv:" ).find( lang ) != string::npos )
    return "ISO-8859-1";
  else if ( string( ":lt:" ).find( lang ) != string::npos )
    return "ISO-8859-13";
  else if ( string( ":cs:hr:hu:pl:ro:sk:sl:" ).find( lang ) != string::npos )
    return "ISO-8859-2";
  else if ( string( ":ru:" ).find( lang ) != string::npos )
    return "ISO-8859-5";
  else if ( string( ":el:" ).find( lang ) != string::npos )
    return "ISO-8859-7";
  else if ( string( ":iw:" ).find( lang ) != string::npos )
    return "ISO-8859-8";
  else if ( string( ":tr:" ).find( lang ) != string::npos )
    return "ISO-8859-9";
  else if ( string( ":ja:" ).find( lang ) != string::npos )
    return "eucJP";
  return "";
}
//-----------------------------------------------------------------
