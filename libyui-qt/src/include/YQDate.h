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

  File:	      YQDate.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQDate_h
#define YQDate_h

#include <qdatetimeedit.h>
#include <qvbox.h>
#include <qlabel.h>
#include <ycp/YCPString.h>
#include <ycp/YCPInteger.h>

#include "YDate.h"


class YQDate : public QVBox, public YDate
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQDate( QWidget * 			parent,
	     const YWidgetOpt & 	opt,
	     const YCPString	&	label,
	     const YCPString	&	date );

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
     * Set the date in the entry to a new value
     */
    void setNewDate( const YCPString & text );

    /**
     * Sets the new size of the widget.
     */
    void setSize( long newWidth, long newHeight );

    /**
     * Gets the date currently entered in the Date entry
     */
    YCPString getDate();

    /**
     * Changes the label of the date entry.
     */
    void setLabel( const YCPString & label );
    
    /**
     * Change the label text.
     */
    //void setDate( const YCPString & label );
    
protected:
    QDateEdit *	_qt_dateedit;
    QLabel *	_qt_label;
};


#endif // YQDate_h
