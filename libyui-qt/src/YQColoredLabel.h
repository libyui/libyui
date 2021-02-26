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

  File:	      YQColoredLabel.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQColoredLabel_h
#define YQColoredLabel_h

#include <qlabel.h>
#include <ycp/YCPString.h>

#include "YLabel.h"


class YQColoredLabel : public QLabel, public YLabel
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQColoredLabel( QWidget * 		parent,
		    const YWidgetOpt & 	opt,
		    YCPString 		text,
		    YColor & 		fg,
		    YColor & 		bg,
		    int			margin );

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling( bool enabled );

    /**
     * Minimum size the widget should have to make it look and feel
     * nice.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     */
    long nicesize( YUIDimension dim );

    /**
     * Sets the new size of the widget.
     */
    void setSize( long newWidth, long newHeight );

    /**
     * Change the label text.
     */
    void setLabel( const YCPString & label );
};

#endif // YQColoredLabel_h
