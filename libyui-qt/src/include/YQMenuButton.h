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

  File:	      YQMenuButton.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQMenuButton_h
#define YQMenuButton_h

#include <qwidget.h>
#include <ycp/YCPString.h>

#include "YMenuButton.h"


class QPushButton;

class YQMenuButton : public QWidget, public YMenuButton
{
    Q_OBJECT

protected:

    /**
     * The actual push button
     */
    QPushButton * _qt_pushbutton;

public:
    /**
     * Constructor.
     */
    YQMenuButton( QWidget * parent, YWidgetOpt & opt, YCPString label);

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
     * Changes the label of the button
     */
    void setLabel(const YCPString & label);

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Actually create the menu structure.
     *
     * Reimplemented - inherited from YMenuButton.
     */
    virtual void createMenu();

    /**
     * Recursively create the menu structure.
     */
    void createMenu( YMenuItem * ymenu, QPopupMenu * _qt_menu );
    
protected slots:

    /**
     * Triggered when any menu item is activated.
     */
    void menuEntryActivated( int menu_item_index );

    /**
     * Triggered via menuEntryActivated() by zero timer to get back in sync
     * with the Qt event loop. You don't really need to understand this, it's
     * kind of black magic. ;-) 
     */
    void returnNow();
};

#endif // YQMenuButton_h
