/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       position.cc

   Author:     Michael Andres <ma@suse.de>

/-*/
#include <iostream>
using namespace std;

#include "position.h"


ostream & operator<<( ostream & STREAM, const wpos & OBJ )
{
  return STREAM << '(' << OBJ.L << ',' << OBJ.C << ')';
}


ostream & operator<<( ostream & STREAM, const wpair & OBJ )
{
  return STREAM << '(' << OBJ.A << ',' << OBJ.B << ')';
}


ostream & operator<<( ostream & STREAM, const wsze & OBJ )
{
  return STREAM << '[' << OBJ.H << 'x' << OBJ.W << ']';
}

ostream & operator<<( ostream & STREAM, const wrect & OBJ )
{
  return STREAM << '{' << OBJ.Pos << OBJ.Sze << '}';
}
