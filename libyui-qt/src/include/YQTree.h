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

  File:	      YQTree.h

  Author:     Stefan Hundhammer <sh@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQTree_h
#define YQTree_h

#include <qwidget.h>
#include <qlistview.h>
#include <qptrdict.h>
#include <ycp/YCPString.h>

#include "YTree.h"


class QLabel;
class YQTreeItem;


class YQTree : public QWidget, public YTree
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQTree ( QWidget *parent, YWidgetOpt &opt, const YCPString & label );

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling ( bool enabled );

    /**
     * Minimum size the widget should have to make it look and feel
     * nice.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     */
    long nicesize ( YUIDimension dim );

    /**
     * Sets the new size of the widget.
     */
    void setSize ( long newWidth, long newHeight );

    /**
     * Change the label text.
     */
    void setLabel ( const YCPString & label );

    /**
     * Inherited fro YTree. Called once after items have been added.
     */
    void rebuildTree();

    /**
     * Inherited from YTree. Returns the the currently
     * selected item or 0 if no item is selected.
     */
    virtual const YTreeItem * getCurrentItem() const;

    /**
     * Inherited from YTree. Selects an item.
     */
    virtual void setCurrentItem ( YTreeItem * it );

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();


    /**
     * Register a newly created item.
     * Calling this method is mandatory for all items.
     */
    void registerItem ( const YTreeItem *	orig,
			const YQTreeItem *	clone );


    /**
     * Find the corresponding YQTreeItem for a given YTreeItem.
     * Returns 0 if not found.
     */
    YQTreeItem *findYQTreeItem ( const YTreeItem * orig ) const;


protected slots:

    /**
     * Tells the ui that an item has been selected. This is only
     * important if the `notify option is set.
     */
    void slotSelected ( void );


protected:

    /**
     * Title label of the box.
     */
    QLabel * _qt_label;

    /**
     * The real tree: The QListView widget.
     */
    QListView * _listView;

    /**
     * The next item serial number.
     */
    int _nextSerialNo;

    /**
     * Map from YTreeItem to the corresponding (cloned) YQTreeItem.
     * A Qt collection class is used here rather than an STL class
     * because the STL map requires an operator< to be defined which is
     * not very natural for pointers to objects.
     *
     * registerItem() adds a pair to this map.
     */
    QPtrDict <YQTreeItem> _yTreeItemToYQTreeItem;
};


class YQTreeItem: public QListViewItem
{
public:

    /**
     * Constructor for a top level item.
     **/
    YQTreeItem ( YQTree	*		tree,
		 QListView *		parent,
		 const YTreeItem *	origItem,
		 int			serial	);


    /**
     * Constructor for a non-top level item.
     **/
    YQTreeItem ( YQTree	*		tree,
		 YQTreeItem *		parent,
		 const YTreeItem *	origItem,
		 int			serial	);

    /**
     * Recursively build the subtree corresponding to 'items'.
     **/
    void buildSubTree ( YTreeItemList & items, int & nextSerialNo );

    /**
     * Returns the original YTreeItem of which this item is a clone.
     */
    const YTreeItem * origItem() { return _origItem; }


private:

    /**
     * Init function. All constructors end up here.
     */
    void init ( YQTree *		tree,
		const YTreeItem *	yTreeItem,
		int			serial );


protected:

    /**
     * The YQTree parent this item belongs to.
     */
    YQTree * _tree;

    /**
     * The original YTreeItem of which this item is a clone.
     */
    const YTreeItem * _origItem;

    /**
     * The serial number of this item so the insertion sort order can
     * be maintained at all times.
     */
    int _serialNo;

    /**
     * Sort key of this item. Inherited by QListViewItem.
     */
    QString key ( int column, bool ascending ) const;

};


#endif // YQLabel_h
