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

  File:	      YQSplit.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


//#include <qlayout.h>

#define y2log_component "qt-ui"
#include <ycp/y2log.h>
#include "YUIQt.h"
#include "YQSplit.h"


YQSplit::YQSplit(QWidget *parent, YWidgetOpt &opt, YUIDimension dimension)
    : QWidget(parent)
    , YSplit(opt, dimension)
{
    setWidgetRep(this);
}


void YQSplit::setEnabling(bool enabled)
{
    QWidget::setEnabled(enabled);
}


void YQSplit::setSize(long newwidth, long newheight)
{
    resize(newwidth, newheight);
    YSplit::setSize(newwidth, newheight);
}


void YQSplit::moveChild(YWidget *child, long newx, long newy)
{
    QWidget *qw = (QWidget *)(child->widgetRep());
    qw->move(newx, newy);
}

#include "YQSplit.moc.cc"
