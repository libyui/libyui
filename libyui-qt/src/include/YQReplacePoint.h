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

  File:	      YQReplacePoint.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQReplacePoint_h
#define YQReplacePoint_h

#include <qwidget.h>

#include "YReplacePoint.h"

class QWidget;

class YQReplacePoint : public QWidget, public YReplacePoint
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQReplacePoint( QWidget * parent, YWidgetOpt & opt);

    /**
     * Inherited from YContainerWidget. Called when a child
     * has been added. We call show() for the new widget.
     */
    void childAdded(YWidget * child);

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


#endif // YQReplacePoint_h
