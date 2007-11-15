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

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


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
     **/
    YQReplacePoint( YWidget * parent );

    /**
     * Show a newly added child.
     *
     * Reimplemented from YReplacePoint.
     **/
    virtual void showChild();

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );
    
    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );
};


#endif // YQReplacePoint_h
