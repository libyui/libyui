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
#include "Y2NCursesUIComponent.h"

Y2CCNCursesUI::Y2CCNCursesUI()
    : Y2ComponentCreator(Y2ComponentBroker::BUILTIN)
{
}

bool Y2CCNCursesUI::isServerCreator() const
{
  return true;
}

Y2Component *Y2CCNCursesUI::create(const char *name) const
{
  if (!strcmp(name, "ncurses"))
    return new Y2NCursesUIComponent();
  else
    return 0;
}

Y2CCNCursesUI g_y2ccncurses;
