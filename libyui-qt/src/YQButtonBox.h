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

  File:	      YQButtonBox.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQButtonBox_h
#define YQButtonBox_h

#include <qwidget.h>
#include "YButtonBox.h"


class YQButtonBox : public QWidget, public YButtonBox
{
    Q_OBJECT

public:
    
    /**
     * Constructor.
     **/
    YQButtonBox( YWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQButtonBox();

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
     * Reimplemented from YButtonBox.
     **/
    virtual void moveChild( YWidget * child, int newX, int newY );
};


#endif // YQButtonBox_h
