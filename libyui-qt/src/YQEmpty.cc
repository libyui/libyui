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

  File:	      YQEmpty.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include "YQEmpty.h"

YQEmpty::YQEmpty(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt)
    : QWidget(parent)
    , YEmpty(opt)
{
    setWidgetRep((QWidget *)this);
}

#include "YQEmpty.moc.cc"
