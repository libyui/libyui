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

   File:       Y2Log.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef Y2Log_h
#define Y2Log_h

#include <iosfwd>

#include <iostream>
using namespace std;

#include "stdutil.h"
using namespace stdutil;

namespace Y2Log {

enum lclass {
  // update Y2Loglinestreamset::_init() on changes
  C_Y2UI = 0,
  C_NC,
  C_NC_WIDGET,
  C_NC_IO,
  C_TRACE,
  // last entry:
  C_DBG
};

extern std::ostream & get( unsigned which, unsigned level,
		      const char * fil, const char * fnc, int lne );

extern std::ostream & dbg_get( const char * dbg_class, unsigned level,
			       const char * fil, const char * fnc, int lne  );

} // namespace Y2Log

#define _LOG(c,l) Y2Log::get( c, l, __FILE__, __FUNCTION__, __LINE__ )

#define _DBG(c) _LOG( c, 0 )
#define _MIL(c) _LOG( c, 1 )
#define _WAR(c) _LOG( c, 2 )
#define _ERR(c) _LOG( c, 3 )
#define _SEC(c) _LOG( c, 4 )
#define _INT(c) _LOG( c, 5 )

#define UIDBG _DBG(Y2Log::C_Y2UI)
#define UIMIL _MIL(Y2Log::C_Y2UI)
#define UIWAR _WAR(Y2Log::C_Y2UI)
#define UIERR _ERR(Y2Log::C_Y2UI)
#define UISEC _SEC(Y2Log::C_Y2UI)
#define UIINT _INT(Y2Log::C_Y2UI)

#define NCDBG _DBG(Y2Log::C_NC)
#define NCMIL _MIL(Y2Log::C_NC)
#define NCWAR _WAR(Y2Log::C_NC)
#define NCERR _ERR(Y2Log::C_NC)
#define NCSEC _SEC(Y2Log::C_NC)
#define NCINT _INT(Y2Log::C_NC)

#define WIDDBG _DBG(Y2Log::C_NC_WIDGET)
#define WIDMIL _MIL(Y2Log::C_NC_WIDGET)
#define WIDWAR _WAR(Y2Log::C_NC_WIDGET)
#define WIDERR _ERR(Y2Log::C_NC_WIDGET)
#define WIDSEC _SEC(Y2Log::C_NC_WIDGET)
#define WIDINT _INT(Y2Log::C_NC_WIDGET)

#define IODBG _DBG(Y2Log::C_NC_IO)
#define IOMIL _MIL(Y2Log::C_NC_IO)
#define IOWAR _WAR(Y2Log::C_NC_IO)
#define IOERR _ERR(Y2Log::C_NC_IO)
#define IOSEC _SEC(Y2Log::C_NC_IO)
#define IOINT _INT(Y2Log::C_NC_IO)


// debug stuff

#define DBG_CLASS 0

#define DDBG Y2Log::dbg_get( DBG_CLASS, 0, __FILE__, __FUNCTION__, __LINE__ )
#define MDBG Y2Log::dbg_get( DBG_CLASS, 1, __FILE__, __FUNCTION__, __LINE__ )
#define WDBG Y2Log::dbg_get( DBG_CLASS, 2, __FILE__, __FUNCTION__, __LINE__ )
#define EDBG Y2Log::dbg_get( DBG_CLASS, 3, __FILE__, __FUNCTION__, __LINE__ )
#define SDBG Y2Log::dbg_get( DBG_CLASS, 4, __FILE__, __FUNCTION__, __LINE__ )
#define IDBG Y2Log::dbg_get( DBG_CLASS, 5, __FILE__, __FUNCTION__, __LINE__ )

#define D__ _DBG(Y2Log::C_TRACE)
#define M__ _MIL(Y2Log::C_TRACE)
#define W__ _WAR(Y2Log::C_TRACE)
#define E__ _ERR(Y2Log::C_TRACE)
#define S__ _SEC(Y2Log::C_TRACE)
#define I__ _INT(Y2Log::C_TRACE)

#endif // Y2Log_h
