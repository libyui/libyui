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


#ifndef YQEmpty_h
#define YQEmpty_h

#include <qwidget.h>
#include "YEmpty.h"


class YQEmpty : public QWidget, public YEmpty
{
    Q_OBJECT

public:
    /**
     * Constructor
     */
    YQEmpty( QWidget * parent, const YWidgetOpt & opt );
    
    /**
     * Sets the new size of the widget.
     * Reimplemented from YWidget.
     */
    void setSize( long newWidth, long newHeight );
};

#endif // YQEmpty_h
