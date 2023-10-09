/*
  Copyright (c) 2000 - 2010 Novell, Inc.
  Copyright (c) 2021 SUSE LLC

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*
  File:	      YQPkgPatternList.h
  Author:     Stefan Hundhammer <shundhammer.de>
*/


#ifndef YQPkgPatternList_h
#define YQPkgPatternList_h

#include "YQPkgObjList.h"
#include <QMap>


class YQPkgPatternListItem;
class YQPkgPatternCategoryItem;


/**
 * @short Display a list of zypp::Pattern objects.
 **/
class YQPkgPatternList : public YQPkgObjList
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * Set 'autoFill' to 'false' if you don't want the list to be filled in the
     * constructor. In that case, use fillList() (e.g., when connections are
     * set up).
     *
     * Set 'autoFilter' to 'false' if there is no need to do (expensive)
     * filtering because the 'filterMatch' signal is not connected anyway.
     **/
    YQPkgPatternList( QWidget * parent, bool autoFill = true, bool autoFilter = true );

    /**
     * Destructor
     **/
    virtual ~YQPkgPatternList();

    /**
     * Column number for the pattern order or -1 if disabled
     **/
    int orderCol()      const   { return _orderCol; }

    /**
     * Flag: Should the order column be shown?
     * (set environment variable Y2_SHOW_PATTERNS_ORDER)
     **/
    bool showOrderCol() const   { return _orderCol >= 0; }

    /**
     * Flag: Show invisible patterns, too?
     * (set environment variable Y2_SHOW_INVISIBLE_PATTERNS)
     **/
    bool showInvisiblePatterns() const { return _showInvisiblePatterns; }


public slots:

    /**
     * Filter according to the view's rules and current selection.
     * Emits those signals:
     *    filterStart()
     *    filterMatch() for each pkg that matches the filter
     *    filterFinished()
     **/
    void filter();

    /**
     * Same as filter(), but only if this widget is currently visible.
     **/
    void filterIfVisible();


    /**
     * Add a pattern to the list. Connect a filter's filterMatch() signal to
     * this slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     **/
    void addPatternItem( ZyppSel	selectable,
			 ZyppPattern 	pattern );

    /**
     * Fill the pattern list.
     **/
    void fillList();

    /**
     * Dispatcher slot for mouse click: cycle status depending on column.
     * For pattern category items, emulate tree open / close behaviour.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual void pkgObjClicked( int		button,
				QTreeWidgetItem * item,
				int		col,
				const QPoint &	pos );

    /**
     * Select the first selectable list entry that is not a pattern category.
     *
     * Reimplemented from QY2ListView.
     **/
    virtual void selectSomething();


public:

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgPatternListItem * selection() const;


signals:

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter.
     **/
    void filterMatch( ZyppSel	selectable,
		      ZyppPkg	pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();

protected:

    /**
     * Returns the category item with the specified name. Creates such a
     * category if it doesn't exist yet and categoryName is not empty. Returns
     * 0 if categoryName is empty.
     **/
    YQPkgPatternCategoryItem * category( const QString & categoryName );


    //
    // Data members
    //

    QMap<QString, YQPkgPatternCategoryItem*> _categories;

    int  _orderCol;
    bool _showInvisiblePatterns;
};



class YQPkgPatternListItem: public YQPkgObjListItem
{
public:

    /**
     * Constructor for root items
     **/
    YQPkgPatternListItem( YQPkgPatternList * 		patternList,
			  ZyppSel			selectable,
			  ZyppPattern 			zyppPattern );

    /**
     * Constructor for items that belong to a category
     **/
    YQPkgPatternListItem( YQPkgPatternList *		patternList,
			  YQPkgPatternCategoryItem *	parentCategory,
			  ZyppSel			selectable,
			  ZyppPattern			zyppPattern );

    /**
     * Destructor
     **/
    virtual ~YQPkgPatternListItem();

    /**
     * Returns the original object within the package manager backend.
     **/
    ZyppPattern zyppPattern() const { return _zyppPattern; }

    /**
     * Cycle the package status to the next valid value.
     * Reimplemented from YQPkgObjList.
     **/
    virtual void cycleStatus();

    /**
     * sorting function
     */
    virtual bool operator< ( const QTreeWidgetItem & other ) const;

    // Columns

    int statusCol()	const	{ return _patternList->statusCol();	}
    int summaryCol()	const	{ return _patternList->summaryCol();	}
    int orderCol()      const   { return _patternList->orderCol();      }

    int totalPackages() const { return _total; }
    int installedPackages() const { return _installed; }

    // setters for installed and total packages values
    void setTotalPackages(int v) { _total = v; }
    void setInstalledPackages(int v) { _installed = v; }

    /**
     * resets the tooltip with the current available information
     */
    void resetToolTip();

protected:

    /**
     * Initialize things common to all constructors.
     **/
    void init();

    /**
     * Propagate status changes in this list to other lists:
     * Have the solver transact all patterns.
     *
     * Reimplemented from YQPkgObjListItem.
     **/
    virtual void applyChanges();


    // Data members

    YQPkgPatternList *	_patternList;
    ZyppPattern		_zyppPattern;


    // Cached values

    int _total;
    int _installed;
};



class YQPkgPatternCategoryItem: public QY2ListViewItem
{
public:

    /**
     * Constructor
     **/
    YQPkgPatternCategoryItem( YQPkgPatternList *	patternList,
			      const QString &		category	);

    /**
     * Destructor
     **/
    virtual ~YQPkgPatternCategoryItem();

    /**
     * Returns the first pattern. This should be the first in sort order.
     **/
    ZyppPattern firstPattern() const { return _firstPattern; }

    /**
     * Add a pattern to this category. This method sets firstPattern() if necessary.
     **/
    void addPattern( ZyppPattern pattern );

    /**
     * sorting function
     */
    virtual bool operator< ( const QTreeWidgetItem & other ) const;

    /*
     * Open or close this subtree
     *
     * Reimplemented from QListViewItem to force categories open at all times
     **/
    virtual void setExpanded( bool open );


protected:

    /**
     * Set a suitable tree open/close icon depending on this category's
     * open/close status.
     *
     * The default QListView plus/minus icons would require treeStepSize() to
     * be set >0 and rootItemDecorated( true ), but that would look very ugly
     * in this context, so the pattern categories paint their own tree open /
     * close icons.
     **/
    void setTreeIcon( void );
    //
    // Data members
    //

    YQPkgPatternList *	_patternList;
    ZyppPattern		_firstPattern;
};


#endif // ifndef YQPkgPatternList_h
