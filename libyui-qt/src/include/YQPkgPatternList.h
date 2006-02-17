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

#include <YQPkgObjList.h>


class YQPkgPatternListItem;


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
    void addPkgSelItem( ZyppSel		selectable,
			ZyppPattern 	pattern );

    /**
     * Emit an updatePackages() signal.
     **/
    void sendUpdatePackages() { emit updatePackages(); }

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
     * Emitted when it's time to update displayed package information,
     * e.g., package states.
     **/
    void updatePackages();

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



class YQPkgPatternListItem: public YQPkgObjListItem
{
public:

    /**
     * Constructor. Creates a YQPkgPatternList item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkgPatternListItem( YQPkgPatternList * 	pkgSelList,
			  ZyppSel		selectable,
			  ZyppPattern 		zyppPattern );

    /**
     * Destructor
     **/
    virtual ~YQPkgPatternListItem();

    /**
     * Returns the original object within the package manager backend.
     **/
    ZyppPattern zyppPattern() const { return _zyppPattern; }

    /**
     * Set the pattern status.
     *
     * Reimplemented from YQPkgObjListItem:
     * Activate patterns and emit updatePackages signal for each
     * status change.
     **/
    virtual void setStatus( ZyppStatus newStatus );

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

    int statusCol()	const	{ return _pkgSelList->statusCol();	}
    int summaryCol()	const	{ return _pkgSelList->summaryCol();	}


protected:

    /**
     * Propagate changes in the pattern status to the affected packages
     * via the solver.
     **/
    void applyChanges();


    // Data members

    YQPkgPatternList *	_pkgSelList;
    ZyppPattern		_zyppPattern;
};


#endif // ifndef YQPkgPatternList_h
