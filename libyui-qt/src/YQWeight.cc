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

  File:	      YQWeight.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

#include "YUIQt.h"
#include "YQWeight.h"

YQWeight::YQWeight(YUIQt *yuiqt, QWidget *parent, YUIDimension dim, long weight)
    : QWidget(parent)
    , YWeight(dim, weight)
{
    setWidgetRep((QWidget *)this);
}


void YQWeight::setSize(long newwidth, long newheight)
{
    resize(newwidth, newheight);
    YWeight::setSize(newwidth, newheight);
}


#include "YQWeight.moc.cc"
