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

  File:	      YQCheckBox.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQCheckBox_h
#define YQCheckBox_h

#include <qgroupbox.h>
#include <ycp/YCPString.h>

#include "YCheckBox.h"


class QCheckBox;
class YUIQt;

class YQCheckBox : public QGroupBox, public YCheckBox
{
    Q_OBJECT

    /**
     * Pointer to the ui class, for reporting events
     */
    YUIQt *yuiqt;

    /**
     * Pointer to the qt widget representing the text entry
     */
    QCheckBox *qt_checkbox;

    /**
     * Flag that indicates tristate condition: neither on nor off
     */
    bool dont_care;


public:
    /**
     * Creates a new YQCheckBox
     * @param yuiqt pointer to the ui class
     * @param parent pointer to the parent widget
     * @param label Label to be shown above text entry
     * @param checked true, if the checkbox starts checked
     */
    YQCheckBox(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt,
	       const YCPString& label, bool checked);

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
    void setSize(long newwidth, long newheight);

    /**
     * Sets the checked-state of the checkbox
     */
    void setValue(const YCPValue& checked);

    /**
     * Returns whether the checkbox is checked.
     * This may return 'true' or 'false' or 'nil' for a tristate check box.
     */
    YCPValue getValue();

    /**
     * Changes the label of the text entry.
     */
    void setLabel(const YCPString& label);

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Returns whether tristate condition is set (i.e. neither on nor off)
     */
    bool isTristate();

    /**
     * Set tristate condition
     */
    void setTristate(bool tristate);

private slots:

    /**
     * Triggered when the RadioButton is toggled.
     * This _may_ be of interest to the module.
     */
    void changed ( bool newState );
};

#endif // YQCheckBox_h
