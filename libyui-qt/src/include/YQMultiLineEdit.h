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

  File:	      YQMultiLineEdit.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQMultiLineEdit_h
#define YQMultiLineEdit_h

#include <qvbox.h>
#include <qlabel.h>
#include <qmultilineedit.h>
#include <ycp/YCPString.h>

#include "YMultiLineEdit.h"

#define DEFAULT_VISIBLE_LINES 3


class YQMultiLineEdit : public QVBox, public YMultiLineEdit
{
    Q_OBJECT

public:
    /**
     * Constructor
     */
    YQMultiLineEdit( QWidget * parent, YWidgetOpt & opt,
		    const YCPString & label, const YCPString & initialText );

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
     * Set the edited text.
     * Inherited from YMultiLineEdit.
     */
    void setText( const YCPString & text);

    /**
     * Get the edited text.
     * Inherited from YMultiLineEdit.
     */
    YCPString text();

    /**
     * Get the number of lines visible by default
     */
    int visibleLines() const { return DEFAULT_VISIBLE_LINES; }



protected slots:
    /**
     * Triggered when the text changes.
     */
    void changed();


protected:

    QLabel		* qt_label;
    QMultiLineEdit	* qt_multiLineEdit;
};


#endif // YQMultiLineEdit_h
