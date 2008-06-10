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

   File:       NCtypes.h

   Author:     Michael Andres <ma@suse.de>

/-*/

#ifndef NCtypes_h
#define NCtypes_h

#include <iosfwd>

namespace NC
{
    enum ADJUST
    {
	CENTER = 0x00,
	TOP    = 0x01,
	BOTTOM = 0x02,
	LEFT   = 0x10,
	RIGHT  = 0x20,
	TOPLEFT = TOP | LEFT,
	TOPRIGHT = TOP | RIGHT,
	BOTTOMLEFT = BOTTOM | LEFT,
	BOTTOMRIGHT = BOTTOM | RIGHT,
    };

    enum WState
    {
	WSdumb,
	WSnormal,
	WSactive,
	WSdisabeled
    };

};

extern std::ostream & operator<<( std::ostream & str, NC::ADJUST obj );
extern std::ostream & operator<<( std::ostream & str, NC::WState obj );


#endif // NCtypes_h
