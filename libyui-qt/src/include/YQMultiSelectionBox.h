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

  File:	      YQMultiSelectionBox.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQMultiSelectionBox_h
#define YQMultiSelectionBox_h

#include <qvbox.h>
#include <qlistview.h>
#include <ycp/YCPString.h>

#include "YMultiSelectionBox.h"


class QLabel;

class YQMultiSelectionBox : public QVBox, public YMultiSelectionBox
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQMultiSelectionBox( QWidget * 		parent,
			 const YWidgetOpt & 	opt,
			 const YCPString &	label );

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
    virtual void setLabel( const YCPString & label );

    /**
     * Notification that a new item has been added.
     *
     * Reimplemented from YMultiSelectionBox.
     */
    virtual void itemAdded( const YCPString & itemText, bool selected );

    /**
     * Returns the index of the item that currently has the keyboard focus.
     *
     * Reimplemented from YMultiSelectionBox
     */
    virtual int getCurrentItem();

    /**
     * Set the keyboard focus to one item.
     *
     * Reimplemented from YMultiSelectionBox
     */
    virtual void setCurrentItem( int index );

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Delete all items.
     *
     * Reimplemented from YMultiSelectionBox
     */
    virtual void deleteAllItems();

    /**
     * Sends a ValueChanged signal.
     **/
    void sendValueChanged();


signals:

    /**
     * Emitted upon when an item changes its state.
     **/
    void valueChanged();
    
    
protected:

    /**
     * Check whether or not item #index is selected.
     *
     * Reimplemented from YMultiSelectionBox
     */
    virtual bool itemIsSelected( int index );

    /**
     * Select item #index.
     *
     * Reimplemented from YMultiSelectionBox
     */
    virtual void selectItem( int index );

    /**
     * Deselect all items.
     *
     * Reimplemented from YMultiSelectionBox
     */
    virtual void deselectAllItems();



protected slots:

    /**
     * Send SelectionChanged event if `opt(`notify) is set.
     **/
    void slotSelected();
    
    /**
     * Send ValueChanged event if `opt(`notify) is set.
     **/
    void slotValueChanged();

    
protected:
    
    QLabel *	_qt_label;
    QListView * _qt_listview;

    bool 	_shrinkable;
};



class YQMultiSelectionBoxItem: public QCheckListItem
{
public:
    /**
     * Constructor.
     **/
    YQMultiSelectionBoxItem( YQMultiSelectionBox * 	parent,
			     QListView * 		listView,
			     const QString &		text );

    /**
     * Destructor.
     **/
    virtual ~YQMultiSelectionBoxItem() {}

    
protected:

    /**
     * Called when the on/off state changes.
     * Triggers a 'valueChanged' event in the associated YQMultiSelectionBox.
     *
     * Reimplemented from QChecklistItem.
     **/
    void stateChange( bool newState );

    /**
     * Returns the sort key for any column.
     * Reimplemented from @ref QListViewItem.
     *
     * This method is the true reason for this class - a pure QListViewItem is
     * too dumb to retain insertion order even if sorting is explicitly
     * disabled: It gets sorted in reverse insertion order. :-(
     **/ 
    virtual QString key ( int	column,
			  bool	ascending ) const;


    // Data members
    
    int 			_serial;
    YQMultiSelectionBox * 	_multiSelectionBox;
    

    /**
     * The next serial number to use
     **/
    static int _item_count;
};

#endif // YQLabel_h
