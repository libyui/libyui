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

  File:	      YQLogView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQLogView_h
#define YQLogView_h

#include <qvbox.h>
#include <qlabel.h>
#include <qmultilineedit.h>
#include <ycp/YCPString.h>

#include "YLogView.h"


class YQLogView : public QVBox, public YLogView
{
    Q_OBJECT
    
public:
    /**
     * Constructor
     */
    YQLogView( QWidget * parent, YWidgetOpt & opt,
	      const YCPString & label, int visibleLines, int maxLines );

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
     * Accept the keyboard focus.
     */
    bool setKeyboardFocus();

    /**
     * Set the widget's label header.
     */
    void setLabel( const YCPString & newLabel );

    /**
     * Set the log text.
     */
    void setLogText( const YCPString & text);


protected:

    QLabel		* qt_label;
    QMultiLineEdit	* qt_text;
};

#endif // YQLogView_h
