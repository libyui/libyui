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

  File:	      YQPkgYouPatchList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgYouPatchList_h
#define YQPkgYouPatchList_h

#include <YQPkgObjList.h>
#include <y2pm/PMYouPatch.h>


class YQPkgYouPatchListItem;


class YQPkgYouPatchList : public YQPkgObjList
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgYouPatchList( QWidget *parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgYouPatchList();


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
    void addYouPatchItem( PMYouPatchPtr sel );

    /**
     * Emit an updatePackages() signal.
     **/
    void sendUpdatePackages() { emit updatePackages(); }

public:

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgYouPatchListItem * selection() const;


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



class YQPkgYouPatchListItem: public YQPkgObjListItem
{
public:

    /**
     * Constructor. Creates a YQPkgYouPatchList item that corresponds to the package
     * manager object that 'pkg' refers to.
     **/
    YQPkgYouPatchListItem( YQPkgYouPatchList * youPatchList, PMYouPatchPtr youPatch );

    /**
     * Destructor
     **/
    virtual ~YQPkgYouPatchListItem();

    /**
     * Returns the original object within the package manager backend.
     **/
    PMYouPatchPtr pmYouPatch() { return _pmYouPatch; }

    /**
     * Returns the original object within the package manager backend.
     **/
    const PMYouPatchPtr constPmYouPatch() const { return _pmYouPatch; }

    /**
     * Returns the (binary RPM) package status.
     **/
    PMSelectable::UI_Status status() const;

    /**
     * Cycle the package status to the next valid value.
     * Reimplemented from YQPkgObjList.
     **/
    virtual void cycleStatus();
    
    /**
     * Comparison function used for sorting the list.
     * Returns:
     * -1 if this <  other
     *	0 if this == other
     * +1 if this >  other
     *
     * Reimplemented from QListViewItem.
     **/
    virtual int compare( QListViewItem *	other,
			 int			col,
			 bool			ascending ) const;

    // Columns

    int statusCol()	const	{ return _youPatchList->statusCol();	}
    int summaryCol()	const	{ return _youPatchList->summaryCol();	}


protected:

    // Data members

    YQPkgYouPatchList	*	_youPatchList;
    PMYouPatchPtr		_pmYouPatch;
};


#endif // ifndef YQPkgYouPatchList_h
