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

  File:	      YQFrame.h

  Author:     Stefan Hundhammer <sh@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQFrame_h
#define YQFrame_h

#include <qgroupbox.h>

#include "YFrame.h"

class QWidget;

class YQFrame : public QGroupBox, public YFrame
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQFrame ( QWidget * parent, YWidgetOpt & opt, const YCPString & label );

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling(bool enabled );

    /**
     * Set the size.
     */
    void setSize ( long newWidth, long newHeight );

    /**
     * Change the Frame label.
     */
    void setLabel(const YCPString & newLabel );

    /**
     * Minimum size the widget should have to make it look and feel
     * nice.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     */
    long nicesize ( YUIDimension dim );


protected:

    /**
     * Callback function that reports to the ui specific
     * widget that a child has been added. The default implementation
     * does nothing.
     */
    void childAdded(YWidget * child );
};


#endif // YQFrame_h
