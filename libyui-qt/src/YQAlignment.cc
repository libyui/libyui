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

  File:	      YQAlignment.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include "YQAlignment.h"


YQAlignment::YQAlignment(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt,
			 YAlignmentType halign, YAlignmentType valign)
    : QWidget(parent)
    , YAlignment(opt, halign, valign)
{
    setWidgetRep((QWidget *)this);
}


void YQAlignment::moveChild(YWidget *child, long newx, long newy)
{
    QWidget *qw = (QWidget *)(child->widgetRep());
    qw->move(newx, newy);
}


void YQAlignment::setSize(long newwidth, long newheight)
{
    resize(newwidth, newheight);
    YAlignment::setSize(newwidth, newheight);
}

#include "YQAlignment.moc.cc"
