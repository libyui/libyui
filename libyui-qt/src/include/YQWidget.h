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

  File:	      YQWidget.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQWidget_h
#define YQWidget_h


#include <qstring.h>
#include "YWidget.h"


class QWidget;

/**
 * @short abstract base class for all YaST2 Qt widgets
 */
class YQWidget
{
    /**
     * Underlying qt widget that realizes the widget
     */
    QWidget *qw;

public:

    /**
     * Initializes the YQWidget
     * @param qw the underlying Qt widget
     */
    YQWidget(QWidget *qw);

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling(bool enabled);

    /**
     * Convert UTF8 encoded STL string to Unicode Qt string
     **/
    QString UTF8(const string &str) const;

    /**
     * Convert Unicode Qt string to UTF8 encoded STL string
     **/
    string UTF8(const QString &str) const;

protected:

    /**
     * Returns the underlying QWidget
     */
    QWidget *qwidget();

    /**
     * Maximum pixel size a widget can be in one dimension
     */
    static const int MAXPIZELSIZE = 8192;
};



#endif	// YQWidget_h
