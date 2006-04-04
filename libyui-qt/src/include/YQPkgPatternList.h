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

  File:	      YQPkgPatternList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgPatternList_h
#define YQPkgPatternList_h

#include "YQPkgObjList.h"
#include <qdict.h>


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

    QDict<YQPkgPatternCategoryItem> _categories;
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
     * Comparison function used for sorting the list.
     * Returns:
     * -1 if this <  other
     *	0 if this == other
     * +1 if this >  other
     *
     * Reimplemented from QListViewItem:
     * Sort by zypp::Pattern::order() only.
     **/
    virtual int compare( QListViewItem *	other,
			 int			col,
			 bool			ascending ) const;

    // Columns

    int statusCol()	const	{ return _patternList->statusCol();	}
    int summaryCol()	const	{ return _patternList->summaryCol();	}


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
     * Comparison function used for sorting the list.
     * Returns:
     * -1 if this <  other
     *	0 if this == other
     * +1 if this >  other
     *
     * Reimplemented from QListViewItem:
     * Sort by zypp::Pattern::order() only.
     **/
    virtual int compare( QListViewItem *	other,
			 int			col,
			 bool			ascending ) const;

    /*
     * Open or close this subtree
     *
     * Reimplemented from QListViewItem to force categories open at all times
     */
    virtual void setOpen( bool open );

protected:

    /**
     * Paint method. Reimplemented from @ref QListViewItem so a different
     * font can be used.
     *
     * Reimplemented from QY2ListViewItem.
     **/
    virtual void paintCell( QPainter *		painter,
			    const QColorGroup &	colorGroup,
			    int			column,
			    int			width,
			    int			alignment );

    //
    // Data members
    //

    ZyppPattern _firstPattern;
};


#endif // ifndef YQPkgPatternList_h
