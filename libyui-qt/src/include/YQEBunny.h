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

  File:	      YQEBunny.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#include "YFrameBuffer.h"

#ifndef YQEasterBunny_h
#define YQEasterBunny_h

class YQEasterBunny
{
public:

    static void layEgg();

protected:

    static void draw(YFrameBuffer *fb, int sw, int sh);
};


#endif // ifndef YQEasterBunny_h
