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

  File:	      layoututils.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include "layoututils.h"

#include <qwidget.h>
#include <qlayout.h>


void oneWithBorder(QWidget *parent, QWidget *widget)
{
    QGridLayout *layout = new QGridLayout(parent, 1,1, YQWIDGET_BORDER);
    layout->addWidget(widget, 0,0);
}


// +---------------+
// |upper widget   |
// +---------------+
// |lower widget   |
// +---------------+


void oneUponTheOther(QWidget *parent, QWidget *upper, QWidget *lower)
{
    QGridLayout *layout = new QGridLayout(parent, 2,1, YQWIDGET_BORDER);
    layout->addWidget(upper, 0,0);
    layout->addWidget(lower, 1,0);
}


