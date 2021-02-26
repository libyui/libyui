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

  File:	      YQLayoutBox.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQLayoutBox_h
#define YQLayoutBox_h

#include <qwidget.h>

#include "YLayoutBox.h"


class YQLayoutBox : public QWidget, public YLayoutBox
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * Creates a VBox for dim == YD_VERT or a HBox for YD_HORIZ.
     **/
    YQLayoutBox( YWidget * parent, YUIDimension dim );

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

    /**
     * Move a child widget to a new position.
     *
     * Reimplemented from YLayoutBox.
     **/
    virtual void moveChild( YWidget * child, int newX, int newY );
};


typedef YQLayoutBox	YQVBox;
typedef YQLayoutBox	YQHBox;


#endif // YQLayoutBox_h
