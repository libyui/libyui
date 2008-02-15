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

  File:	      YQPkgSelList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgSelList_h
#define YQPkgSelList_h

#include <YQPkgObjList.h>


class YQPkgSelListItem;


/**
 * @short Display a list of zypp::Selection objects.
 **/
class YQPkgSelList : public YQPkgObjList
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
    YQPkgSelList( QWidget * parent, bool autoFill = true, bool autoFilter = true );

    /**
     * Destructor
     **/
    virtual ~YQPkgSelList();


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
     * Add a selection to the list. Connect a filter's filterMatch() signal to
     * this slot. Remember to connect filterStart() to clear() (inherited from
     * QListView).
     **/
    void addPkgSelItem( ZyppSel		selectable,
			ZyppSelection 	selection );

    /**
     * Fill the selection list.
     **/
    void fillList();

public:

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgSelListItem * selection() const;


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
};



class YQPkgSelListItem: public YQPkgObjListItem
{
public:

    /**
     * Constructor
     **/
    YQPkgSelListItem( YQPkgSelList * 	pkgSelList,
		      ZyppSel		selectable,
		      ZyppSelection 	zyppSelection );

    /**
     * Destructor
     **/
    virtual ~YQPkgSelListItem();

    /**
     * Returns the original object within the package manager backend.
     **/
    ZyppSelection zyppSelection() const { return _zyppSelection; }

    /**
     * Comparison function used for sorting the list.
     * Returns:
     * -1 if this <  other
     *	0 if this == other
     * +1 if this >  other
     *
     * Reimplemented from QListViewItem:
     * Sort by zypp::Selection::order() only.
     **/
    virtual int compare( QTreeWidgetItem *	other,
			 int			col,
			 bool			ascending ) const;

    // Columns

    int statusCol()	const	{ return _pkgSelList->statusCol();	}
    int summaryCol()	const	{ return _pkgSelList->summaryCol();	}


protected:

    /**
     * Propagate status changes in this list to other lists:
     * Have the solver transact all selections.
     *
     * Reimplemented from YQPkgObjListItem.
     **/
    virtual void applyChanges();

    
    // Data members

    YQPkgSelList *	_pkgSelList;
    ZyppSelection	_zyppSelection;
};


#endif // ifndef YQPkgSelList_h
