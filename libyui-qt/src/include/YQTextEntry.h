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

  File:	      YQTextEntry.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQTextEntry_h
#define YQTextEntry_h

#include <qvbox.h>
#include <ycp/YCPString.h>

#include "YTextEntry.h"


class QString;
class QLabel;
class QLineEdit;
class QY2CharValidator;

class YQTextEntry : public QVBox, public YTextEntry
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    YQTextEntry( QWidget *		parent,
		 YWidgetOpt &		opt,
		 const YCPString & 	label,
		 const YCPString & 	text );

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling(bool enabled);

    /**
     * Minimum size the widget should have to make it look and feel
     * nice.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     */
    long nicesize(YUIDimension dim);

    /**
     * Sets the new size of the widget.
     */
    void setSize(long newWidth, long newHeight);

    /**
     * Set the text in the entry to a new value
     */
    void setText(const YCPString & text);

    /**
     * Gets the text currently entered in the text entry
     */
    YCPString getText();

    /**
     * Changes the label of the text entry.
     */
    void setLabel(const YCPString & label);

    /**
     * Change the valid input characters.
     *
     * Reimplemented from @ref YTextEntry.
     */
    void setValidChars(const YCPString & validChars);

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();


protected slots:
    /**
     * Triggered when the text in the TextEntry changes.
     * This _may_ be of interest to the module.
     */
    void changed(const QString &);


protected:

    /**
     * Pointer to the qt widget representing the label
     */
    QLabel * _qt_label;

    /**
     * Pointer to the qt widget representing the text entry
     */
    QLineEdit * _qt_lineedit;

    /**
     * Pointer to the validator object
     **/
    QY2CharValidator *	_validator;

    /**
     * true if the horizontal nice size should be very small
     */
    bool shrinkable;
};

#endif // YQTextEntry_h
