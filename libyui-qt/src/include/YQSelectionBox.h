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


class QLabel;
class QListBox;
class QListBoxItem;

class YQSelectionBox : public QVBox, public YSelectionBox
{
    Q_OBJECT
    
public:

    YQSelectionBox( QWidget * parent, YWidgetOpt & opt, const YCPString & label );

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
     * Change the label text.
     */
    void setLabel( const YCPString & label );

    /**
     * Notification that a new item has been added.
     *
     * Reimplemented from YSelectionBox.
     */
    void itemAdded( const YCPString & itemText, int itemIndex, bool selected );

    /**
     * Inherited from YSelectionBox. Returns the index of the currently
     * selected item or -1 if no item is selected.
     */
    int getCurrentItem();

    /**
     * Inherited from YSelectionBox. Makes another item selected.
     */
    void setCurrentItem( int index );

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Event filter - inherited from QWidget
     **/
    bool eventFilter( QObject * obj, QEvent * ev );

    
protected slots:

    /**
     * Notification that an item has been selected.
     * This is only relevant if `opt(`notify ) is set.
     */
    void slotSelected( int index );

    /**
     * Notification that an item has been activated (double clicked).
     */
    void slotActivated( QListBoxItem * item );

    /**
     * Return after some millseconds delay - collect multiple events.
     * This is only relevant if `opt( `notify ) is set.
     **/
    void returnDelayed();
    
    /**
     * Return immediately.
     * This is only relevant if `opt( `notify ) is set.
     **/
    void returnImmediately();

    
protected:

    //
    // Data members
    //

    // Widgets
    
    QLabel *	_qt_label;
    QListBox *	_qt_listbox;

    
    // Very small default size if specified
    bool _shrinkable;

    // Don't use a timer to collect events
    bool _immediateMode;

    // Timer to collect multiple events before returning
    QTimer _timer;
};

#endif // YQLabel_h
