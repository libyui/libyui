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

  File:	      YQEmpty.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQEmpty_h
#define YQEmpty_h

#include <qwidget.h>
#include "YEmpty.h"

class YUIQt;

class YQEmpty : public QWidget, public YEmpty
{
    Q_OBJECT

public:
    /**
     * Constructor
     */
    YQEmpty(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt);
};

#endif // YQEmpty_h
