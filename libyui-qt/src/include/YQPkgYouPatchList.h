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


class QAction;
class YQPkgYouPatchListItem;

/**
 * @short Display a list of PMYouPatch objects.
 **/
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

    /**
     * Fill the selection list.
     **/
    void fillList();

    /**
     * Show the raw patch info for the currently selected patch in a popup dialog.
     **/
    void showRawPatchInfo();


public:

    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgYouPatchListItem * selection() const;

    /**
     * Returns the column for the patch kind
     **/
    int kindCol() const { return _kindCol; }

    /**
     * Returns whether or not the list should include patches that are installed.
     **/
    bool showInstalledPatches() const { return _showInstalledPatches; }

    /**
     * Switch display of installed patches on or off.
     * This does NOT trigger redisplaying the list - use fillList() for that.
     * Default is off.
     **/
    void setShowInstalledPatches( bool show ) { _showInstalledPatches = show; }

    /**
     * Add a submenu "All in this list..." to 'menu'.
     * Returns the newly created submenu.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual QPopupMenu * addAllInListSubMenu( QPopupMenu * menu );

    /**
     * Action that shows the raw patch info for the currently selected patch in
     * a popup dialog. 
     **/
    QAction * actionShowRawPatchInfo;

    
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


protected:

    /**
     * Create the context menu for items that are not installed.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual void createNotInstalledContextMenu();

    /**
     * Create the context menu for installed items.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual void createInstalledContextMenu();

    /**
     * Event handler for keyboard input.
     * Only very special keys are processed here.
     *
     * Reimplemented from YQPkgObjList / QWidget.
     */
    virtual void keyPressEvent( QKeyEvent * ev );


    // Data members

    int		_kindCol;
    bool	_showInstalledPatches;
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
    const PMYouPatchPtr constPMYouPatch() const { return _pmYouPatch; }

    /**
     * Set the patch status.
     *
     * Reimplemented from YQPkgObjListItem:
     * Emit updatePackages signals after changing status.
     **/
    virtual void setStatus( PMSelectable::UI_Status newStatus );

    /**
     * Cycle the package status to the next valid value.
     * Reimplemented from YQPkgObjList.
     **/
    virtual void cycleStatus();

    /**
     * Returns a tool tip text for a specific column of this item.
     * 'column' is -1 if the mouse pointer is in the tree indentation area.
     *
     * Reimplemented from YQPkgObjList.
     **/
    virtual QString toolTip( int column );

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
    int kindCol()	const	{ return _youPatchList->kindCol();	}


protected:


    // Data members

    YQPkgYouPatchList	*	_youPatchList;
    PMYouPatchPtr		_pmYouPatch;
};


#endif // ifndef YQPkgYouPatchList_h
