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

  File:	      YQReplacePoint.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qwidget.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YQReplacePoint.h"


YQReplacePoint::YQReplacePoint(YUIQt *, QWidget *parent, YWidgetOpt &opt)
    : QWidget(parent)
    , YReplacePoint(opt)
{
    setWidgetRep((QWidget *)this);
}


void YQReplacePoint::childAdded(YWidget *child)
{
    ((QWidget *)(child->widgetRep()))->show();
}


void YQReplacePoint::setSize(long newwidth, long newheight)
{
    resize(newwidth, newheight);
    YReplacePoint::setSize(newwidth, newheight);
}

#include "YQReplacePoint.moc.cc"
