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

  File:	      YQPushButton.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQPushButton_h
#define YQPushButton_h

#include <ycp/YCPString.h>

#include "YQGenericButton.h"


class YQDialog;

class YQPushButton : public YQGenericButton
{
    Q_OBJECT
    
public:
    /**
     * Constructor.
     */
    YQPushButton( QWidget * 		parent,
		  YQDialog *		dialog,
		  const YWidgetOpt & 	opt,
		  const YCPString & 	label );

    /**
     * Destructor.
     **/
    virtual ~YQPushButton();

    /**
     * Preferred size of the widget.
     * Reimplemented from YWidget.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     */
    long nicesize( YUIDimension dim );

    /**
     * Sets the new size of the widget.
     * Reimplemented from YWidget.
     */
    void setSize( long newWidth, long newHeight );

    
public slots:

    /**
     * Triggered when the button is activated ( clicked or via keyboard ). Informs
     * the ui about this.
     */
    void hit();
};

#endif // YQPushButton_h
