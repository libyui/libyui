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

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQPushButton_h
#define YQPushButton_h

#include <qwidget.h>
#include <ycp/YCPString.h>

#include "YPushButton.h"


class QPushButton;
class YUIQt;

class YQPushButton : public QWidget, public YPushButton
{
    Q_OBJECT

protected:
    /**
     * Pointer to the ui for reporting events.
     */
    YUIQt *yuiqt;

    /**
     * The actual push button
     */
    QPushButton *qt_pushbutton;

public:
    /**
     * Creates a new YQPushButton.
     * @param yuiqt the ui object
     * @param parent the parent widget
     * @param opt the widget options
     * @param label the button label
     */
    YQPushButton(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt, YCPString label);

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
     * Changes the label of the button
     */
    void setLabel(const YCPString& label);

    /**
     * Makes this button the default button, i.e. the one that gets activated
     * when [Return] is pressed anywhere within the dialog.
     */
    void makeDefaultButton();

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Set this button's icon.
     * Inherited from YPushButton.
     **/
    virtual void setIcon( const YCPString & icon_name );

    
protected:

    /**
     * Inherited from QObject
     **/
    bool eventFilter( QObject *obj, QEvent *event );


public slots:

    /**
     * Triggered when the button is activated (clicked or via keyboard). Informs
     * the ui about this.
     */
    void hit();
};

#endif // YQPushButton_h
