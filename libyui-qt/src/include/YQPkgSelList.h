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

// -*- c++ -*-


#ifndef YQPkgSelList_h
#define YQPkgSelList_h

#include <YQPkgObjList.h>
#include <y2pm/PMSelection.h>


class YQPkgSelListItem;


/**
 * @short Display a list of PMSelection objects.
 **/
class YQPkgSelList : public YQPkgObjList
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgSelList( QWidget * parent );

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
     * this slot. Remember to connect filterStart() to clear() ( inherited from
     * QListView ).
     **/
    void addPkgSelItem( PMSelectionPtr sel );

    /**
     * Emit an updatePackages() signal.
     **/
    void sendUpdatePackages() { emit updatePackages(); }

    /**
     * Activate current selections in the selection manager and emit an
     * updatePackages() signal. 
     **/
    void applyChanges();

public:

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgSelListItem * selection() const;


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
    void filterMatch( PMPackagePtr pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected slots:

    /**
     * Fill the selection list.
     **/
    void fillList();
};



class YQPkgSelListItem: public YQPkgObjListItem
{
public:

    /**
     * Constructor. Creates a YQPkgSelList item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkgSelListItem( YQPkgSelList * pkgSelList, PMSelectionPtr sel );

    /**
     * Destructor
     **/
    virtual ~YQPkgSelListItem();

    /**
     * Returns the original object within the package manager backend.
     **/
    PMSelectionPtr pmSel() { return _pmSel; }

    /**
     * Returns the original object within the package manager backend.
     **/
    const PMSelectionPtr constPmSel() const { return _pmSel; }

    /**
     * Set the selection status.
     *
     * Reimplemented from YQPkgObjListItem:
     * Activate selections and emit updatePackages signal for each
     * status change.
     **/
    virtual void setStatus( PMSelectable::UI_Status newStatus );


    /**
     * Comparison function used for sorting the list.
     * Returns:
     * -1 if this <  other
     *	0 if this == other
     * +1 if this >  other
     *
     * Reimplemented from QListViewItem:
     * Sort by PMSelection::order() only.
     **/
    virtual int compare( QListViewItem *	other,
			 int			col,
			 bool			ascending ) const;

    // Columns

    int statusCol()	const	{ return _pkgSelList->statusCol();	}
    int summaryCol()	const	{ return _pkgSelList->summaryCol();	}


protected:

    // Data members

    YQPkgSelList	*	_pkgSelList;
    PMSelectionPtr		_pmSel;
};


#endif // ifndef YQPkgSelList_h
