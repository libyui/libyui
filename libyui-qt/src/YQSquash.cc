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

  File:	      YQSquash.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include "YUIQt.h"
#include "YQSquash.h"

YQSquash::YQSquash(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt,
		   bool hsquash, bool vsquash)
    : QWidget(parent)
    , YSquash(opt, hsquash, vsquash)
{
    setWidgetRep((QWidget *)this);
}


void YQSquash::setEnabling(bool enabled)
{
    QWidget::setEnabled(enabled);
}


void YQSquash::setSize(long newwidth, long newheight)
{
    resize(newwidth, newheight);
    YSquash::setSize(newwidth, newheight);
}

#include "YQSquash.moc.cc"
