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


class YUIQt;
class QLabel;

class YQMultiSelectionBox : public QVBox, public YMultiSelectionBox
{
    Q_OBJECT

public:

    YQMultiSelectionBox(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt, const YCPString& label);

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
    virtual void setLabel(const YCPString& label);

    /**
     * Called when an item has been added.
     *
     * Reimplemented from YMultiSelectionBox
     *
     * @param string text of the new item
     * @param selected true if the item should be selected.
     */
    virtual void itemAdded( const YCPString & text, bool selected );

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



private slots:

    /**
     * Tells the ui that an item has been selected.
     * This is only relevant if the `notify option is set.
     */
    void slotSelected();

    
protected:
    
    /**
     * Pointer to the ui
     */
    YUIQt *yuiqt;

    /**
     * Title label of the box
     */
    QLabel *qt_label;

    /**
     * The actual Qt list view
     */
    QListView *qt_listview;

    bool shrinkable;
};



class YQMultiSelectionBoxItem: public QCheckListItem
{
public:
    /**
     * Constructor.
     **/
    YQMultiSelectionBoxItem( QListView *parent, const QString &text );

    /**
     * Destructor.
     **/
    virtual ~YQMultiSelectionBoxItem() {}

    
protected:
    
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

    /**
     * This item's serial number - just for sorting purposes.
     **/
    int serial;

    /**
     * The next serial number to use
     **/
    static int item_count;
};

#endif // YQLabel_h
