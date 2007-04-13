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

  File:	      YQAlignment.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQAlignment_h
#define YQAlignment_h

#include <qwidget.h>

#include "YAlignment.h"

class QWidget;

class YQAlignment : public QWidget, public YAlignment
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQAlignment( QWidget *		parent,
		 const YWidgetOpt & 	opt,
		 YAlignmentType 	horAlign,
		 YAlignmentType 	vertAlign );

    /**
     * Moves a child widget to a new position.
     */
    void moveChild( YWidget * child, long newX, long newY );

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling( bool enabled );

    /**
     * Sets the size
     */
    void setSize( long newWidth, long newHeight );

    /**
     * Set the background pixmap.
     * Reimplemented from YAlignment.
     **/
    virtual void setBackgroundPixmap( string px );
    
};


#endif // YQAlignment_h
