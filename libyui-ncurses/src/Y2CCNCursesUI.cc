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

   File:       Y2CCNCursesUI.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/


#include "Y2CCNCursesUI.h"

// This is very important: We create one global variable of
// Y2CCQt. Its constructor will register it automatically to
// the Y2ComponentBroker, so that will be able to find it.
// This all happens before main() is called!

Y2CCNcursesUI g_y2ccncurses;

