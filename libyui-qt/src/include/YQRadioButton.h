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

  File:	      YQRadioButton.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQRadioButton_h
#define YQRadioButton_h

#include <qgroupbox.h>
#include <ycp/YCPString.h>

#include "YRadioButton.h"


class QRadioButton;
class YUIQt;

class YQRadioButton : public QGroupBox, public YRadioButton
{
    Q_OBJECT

    /**
     * Pointer to the ui class, for reporting events
     */
    YUIQt *yuiqt;

    /**
     * Pointer to the qt widget representing the text entry
     */
    QRadioButton *qt_radiobutton;

public:
    /**
     * Creates a new YQRadioButton
     * @param yuiqt pointer to the ui class
     * @param parent pointer to the parent widget
     * @param label Label to be shown above text entry
     */
    YQRadioButton(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt,
		  YRadioButtonGroup *rbg, const YCPString& label, bool checked);

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
     * Sets the checked-state of the RadioButton
     */
    void setValue(const YCPBoolean& checked);

    /**
     * Returns, whether the RadioButton is checked.
     */
    YCPBoolean getValue();

    /**
     * Changes the label of the text entry.
     */
    void setLabel(const YCPString& label);

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Returns the actual qt button. Used by
     * @ref YQButtonGroup
     */
    QRadioButton *getQtButton();

private slots:
    /**
     * Triggered when the RadioButton is toggled.
     * This _may_ be of interest to the module.
     */
    void changed ( bool newState );
};

#endif // YQRadioButton_h
