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


class YUIQt;
class QListView;
class QListViewItem;
class YQListViewItem;

class YQTable : public QVBox, public YTable
{
    Q_OBJECT

public:

    /**
     * Creates a new YQTable widget.
     * @param header the texts for the column headers
     * @param immediate If you set this flag to true, the `notify option
     * triggers a user input each time the current item has changed. If you set
     * it to false, `notify triggers user inputs only when the user clicks on
     * another item.
     * @param hshrinkable sets the horizontal nice size very small
     * @param vshrinkable sets the vertical nice size to one row */
    YQTable(YUIQt *yuiqt, QWidget *parent, YWidgetOpt &opt, vector<string> header);

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
     * Inherited from YTable. Is called, when an item has been added.
     * @param string text of the new item
     * @param index index of the new item.
     * @param selected true if the item should be selected.
     */
    void itemAdded(vector<string> elements, int index);

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
     * Pointer to the ui
     */
    YUIQt *yuiqt;

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
