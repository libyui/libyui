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

class QWidget;

class YQSquash : public QWidget, public YSquash
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQSquash( QWidget *parent, YWidgetOpt & opt, bool hsquash, bool vsquash);

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling(bool enabled);

    /**
     * Sets the size
     */
    void setSize(long newWidth, long newHeight);
};


#endif // YQSquash_h
