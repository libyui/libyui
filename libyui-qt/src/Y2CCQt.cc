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

  File:	      Y2CCQt.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

/**
 * This file is part of YaST2.
 *
 * $Id$
 *
 * Author: Waldo Bastian <bastian@suse.de>
 * Maintainer: Stefan Hundhammer <sh@suse.de>
 *
 * Description: Implementation of the YaST2 Qt Component Creator
 *
 * Copyright:
 *
 * Licence:
 *
 **/

#include "Y2CCQt.h"

// This is very import: We create one global variable of
// Y2CCQt. Its constructor will register it automatically to
// the Y2ComponentBroker, so that will be able to find it.
// This all happens before main() is called!

Y2CCQt g_y2ccqt;

