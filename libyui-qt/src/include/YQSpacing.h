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

  File:	      YQSpacing.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQSpacing_h
#define YQSpacing_h

#include <qwidget.h>
#include "YSpacing.h"


class YQSpacing : public QWidget, public YSpacing
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    YQSpacing( QWidget *		parent,
	       const YWidgetOpt & 	opt,
	       float 			size,
	       bool 			horizontal,
	       bool 			vertical );

    /**
     * Inherited from YSpacing:
     * Return size in pixels. Assume one basic unit to be equivalent to
     * 80*25 characters at dialog default size ( 640*480 ).
     */
    long absoluteSize( YUIDimension dim, float relativeSize );
    
    /**
     * Sets the new size of the widget.
     * Reimplemented from YWidget.
     */
    void setSize( long newWidth, long newHeight );
};

#endif // YQSpacing_h
