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
     **/
    YQSpacing( YWidget *	parent,
	       YUIDimension 	dim,
	       bool 		stretchable	= false,
	       YLayoutSize_t 	layoutUnits	= 0.0 );

    virtual ~YQSpacing();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );
};


#endif // YQSpacing_h
