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

  File:	      YQSelectionBox.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQSelectionBox_h
#define YQSelectionBox_h

#include <qvbox.h>
#include <qtimer.h>
#include <ycp/YCPString.h>

#include "YSelectionBox.h"


class YUIQt;
class QLabel;
class QListBox;

class YQSelectionBox : public QVBox, public YSelectionBox
{
    Q_OBJECT

    /**
     * Pointer to the ui
     */
    YUIQt *yuiqt;

    /**
     * Title label of the box
     */
    QLabel *qt_label;

    /**
     * The actual Qt list box
     */
    QListBox *qt_listbox;

public:

    YQSelectionBox(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt, const YCPString& label);

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
     * Change the label text.
     */
    void setLabel(const YCPString& label);

    /**
     * Inherited from YSelectionBox. Is called, when an item has been added.
     * @param string text of the new item
     * @param index index of the new item.
     * @param selected true if the item should be selected.
     */
    void itemAdded(const YCPString& string, int index, bool selected);

    /**
     * Inherited from YSelectionBox. Returns the index of the currently
     * selected item or -1 if no item is selected.
     */
    int getCurrentItem();

    /**
     * Inherited from YSelectionBox. Makes another item selected.
     */
    void setCurrentItem(int index);

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();


    /**
     * Event filter - inherited from QWidget
     **/
    bool eventFilter( QObject *obj, QEvent *ev );

protected slots:

    /**
     * Tells the ui that an item has been selected.
     * This is only relevant if `opt(`notify) is set.
     */
    void slotSelected( int index );

    /**
     * Return after some millseconds delay - collect multiple events.
     * This is only relevant if `opt(`notify) is set.
     **/
    void returnDelayed();
    
    /**
     * Return immediately.
     * This is only relevant if `opt(`notify) is set.
     **/
    void returnImmediately();

    
protected:

    //
    // Data members
    //
    
    // Very small default size if specified
    bool shrinkable;

    // Don't user a timer to collect events
    bool immediateMode;

    // Timer to collect multiple events before returning
    QTimer timer;
};

#endif // YQLabel_h
