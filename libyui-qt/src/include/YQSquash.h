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

  File:	      YQSquash.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQSquash_h
#define YQSquash_h

#include <qwidget.h>

#include "YSquash.h"

class YUIQt;
class QWidget;

class YQSquash : public QWidget, public YSquash
{
    Q_OBJECT

public:

    /**
     * Creates a new YQSquash.
     */
    YQSquash(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt, bool hsquash, bool vsquash);

    /**
     * Sets the size
     */
    void setSize(long newwidth, long newheight);
};


#endif // YQSquash_h
