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

  File:	      YQTable.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YQTable_h
#define YQTable_h

#include <qvbox.h>
#include <ycp/YCPString.h>

#include "YTable.h"


class QListView;
class QListViewItem;
class YQListViewItem;

class YQTable : public QVBox, public YTable
{
    Q_OBJECT

public:

    /**
     * Constructor.
     */
    YQTable( QWidget *parent, YWidgetOpt &opt, vector<string> header);

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
     * Notification that new items have been added.
     *
     * Reimplemented from YTable.
     */
    void itemAdded( vector<string> elements, int index);

    /**
     * Inherited from YTable. Is called, when alls items are cleared.
     */
    void itemsCleared();

    /**
     * Inherited from YTable. is called, when the text of a cell has changed.
     */
    void cellChanged(int index, int colnum, const YCPString & newtext);

    /**
     * Inherited from YTable. Returns the index of the currently
     * selected item or -1 if no item is selected.
     */
    int getCurrentItem();

    /**
     * Inherited from YTable. Makes another item selected.
     */
    void setCurrentItem(int index);

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Returns 'true' if items should be ordered by insertion order
     **/
    bool sortByInsertionOrder() const { return sort_by_insertion_order; }



protected slots:

    /**
     * Tells the ui that an item has been selected. This is only
     * interesting, if the `notify option is set.
     */
    void slotSelected(QListViewItem *);


    /**
     * Notification that the user wishes to change the sort order.
     **/
    void userSort( int column );


protected:
    /**
     * Helper function. Find item with given id.
     */
    QListViewItem *findItem(int index);

    /**
     * The Qt widget representing the table
     */
    QListView *qt_listview;

    
    /**
     * Sorting strategy
     **/
    bool sort_by_insertion_order;

    /**
     * Flag: User may re-sort the table by clicking on a header?
     **/
    bool enable_user_sort;

    /**
     * For maintaining insertion order
     **/
    YQListViewItem * last_item;
};

#endif // YQLabel_h
