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

  File:	      YQTime.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQTime_h
#define YQTime_h

#include <qdatetimeedit.h>
#include <qvbox.h>
#include <qlabel.h>
#include <ycp/YCPString.h>
#include <ycp/YCPInteger.h>

#include "YTime.h"


class YQTime : public QVBox, public YTime
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQTime( QWidget * 			parent,
	     const YWidgetOpt & 	opt,
	     const YCPString	&	label,
	     const YCPString	&	time );

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
     * Set the time in the entry to a new value
     */
    void setNewTime( const YCPString & text );

    /**
     * Sets the new size of the widget.
     */
    void setSize( long newWidth, long newHeight );

    /**
     * Gets the time currently entered in the Time entry
     */
    YCPString getTime();

    /**
     * Changes the label of the time entry.
     */
    void setLabel( const YCPString & label );
    
    /**
     * Change the label text.
     */
    //void setTime( const YCPString & label );
    
protected:
    QTimeEdit *	_qt_timeedit;
    QLabel *	_qt_label;
};


#endif // YQTime_h
