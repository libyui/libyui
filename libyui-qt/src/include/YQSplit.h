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

  File:	      YQSplit.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQSplit_h
#define YQSplit_h

#include <qwidget.h>

#include "YSplit.h"


class YQSplit : public QWidget, public YSplit
{
    Q_OBJECT

public:

    /**
     * Creates a new YQSplit.
     * @param dimension dimension in which to split. YD_HORIZ or YD_VERT
     */
    YQSplit(QWidget *parent, YWidgetOpt &opt, YUIDimension dimension);

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling(bool enabled);

    /**
     * Inherited from YWidget. Sets the new size of the widget.
     */
    void setSize(long newwidth, long newheight);

    /**
     * Inherited from YSplit. Moves a child to a new position
     */
    void moveChild(YWidget *child, long newx, long newy);
};


#endif // YQSplit_h
